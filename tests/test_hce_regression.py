"""Correctness gates for the classical evaluation work."""

from __future__ import annotations

import json
import re
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CORPUS = json.loads((ROOT / "tests" / "eval_corpus.json").read_text())


def mirror_fen(fen: str) -> str:
    fields = fen.split()
    rows = fields[0].split("/")
    swap = str.maketrans("PNBRQKpnbrqk", "pnbrqkPNBRQK")
    fields[0] = "/".join(row.translate(swap) for row in reversed(rows))
    fields[1] = "b" if fields[1] == "w" else "w"
    rights = fields[2].translate(str.maketrans("KQkq", "kqKQ"))
    fields[2] = "".join(char for char in "KQkq" if char in rights) or "-"
    if fields[3] != "-":
        fields[3] = fields[3][0] + str(9 - int(fields[3][1]))
    return " ".join(fields)


class HceRegressionTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.temp = tempfile.TemporaryDirectory()
        cls.engine = Path(cls.temp.name) / "mangoac"
        subprocess.run(
            [
                "gcc", "-O2", "-Wall", "-DPRUEBAS_HCE",
                str(ROOT / "bitmma3.c"), "-lm", "-pthread", "-o", cls.engine,
            ],
            cwd=ROOT,
            check=True,
            capture_output=True,
            text=True,
        )

    @classmethod
    def tearDownClass(cls) -> None:
        cls.temp.cleanup()

    def evaluate(self, fens: list[str], extra: str = "") -> tuple[list[int], str]:
        fen_file = Path(self.temp.name) / "positions.epd"
        with fen_file.open("w") as stream:
            for fen in fens:
                stream.write('[White "test"]\n[Black "test"]\n')
                stream.write(f'[FEN "{fen}"]\n\n')
        commands = ""
        for index in range(1, len(fens) + 1):
            commands += f"readfen {fen_file} {index}\neval\n{extra}"
        commands += "quit\n"
        result = subprocess.run(
            [str(self.engine)],
            cwd=ROOT,
            input=commands,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        values = [int(value) for value in re.findall(r"Eval:\s*(-?\d+)", result.stdout)]
        white_values = [
            value if fen.split()[1] == "w" else -value
            for value, fen in zip(values, fens)
        ]
        return white_values, result.stdout

    def probe(self, fens: list[str], arguments: str = "") -> list[dict[str, int]]:
        fen_file = Path(self.temp.name) / "probe.epd"
        with fen_file.open("w") as stream:
            for fen in fens:
                stream.write('[White "probe"]\n[Black "probe"]\n')
                stream.write(f'[FEN "{fen}"]\n\n')
        commands = "".join(
            f"readfen {fen_file} {index}\nevalprobe {arguments}\n"
            for index in range(1, len(fens) + 1)
        ) + "quit\n"
        output = subprocess.run(
            [str(self.engine)], cwd=ROOT, input=commands, capture_output=True,
            text=True, timeout=30, check=True,
        ).stdout
        rows = []
        for line in output.splitlines():
            marker = line.find("EvalProbe:")
            if marker < 0:
                continue
            line = line[marker:]
            rows.append({
                key: int(value)
                for key, value in re.findall(r"(\w+)=(-?\d+)", line)
            })
        self.assertEqual(len(rows), len(fens), output)
        return rows

    def test_eval_is_deterministic_and_cache_exact(self) -> None:
        fen = CORPUS[6]["fen"]
        values, _ = self.evaluate([fen, fen])
        self.assertEqual(values[0], values[1])

    def test_color_mirror_inverts_white_score(self) -> None:
        for row in CORPUS[:18]:
            with self.subTest(row=row["id"]):
                values, _ = self.evaluate([row["fen"], mirror_fen(row["fen"])])
                self.assertLessEqual(abs(values[0] + values[1]), 40)
                if abs(values[0]) > 40 and abs(values[1]) > 40:
                    self.assertLessEqual(values[0] * values[1], 0)

    def test_fifty_move_scaling_is_progressive_even_on_cache_hits(self) -> None:
        base = "8/5pk1/3p2p1/4P2p/3P3P/5PP1/5K2/8 w - - {} 80"
        values, _ = self.evaluate([base.format(counter) for counter in (80, 85, 90, 95, 100)])
        magnitudes = [abs(value) for value in values]
        self.assertEqual(magnitudes, sorted(magnitudes, reverse=True))
        self.assertEqual(values[-1], 0)
        self.assertGreater(magnitudes[1], magnitudes[2])
        self.assertGreater(magnitudes[2], magnitudes[3])

    def test_lazy_result_is_not_reused_as_exact(self) -> None:
        fen = CORPUS[4]["fen"]
        fen_file = Path(self.temp.name) / "lazy.epd"
        fen_file.write_text(f'[White "test"]\n[Black "test"]\n[FEN "{fen}"]\n')
        commands = (
            f"readfen {fen_file} 1\n"
            "evalwindow 10000 10001\n"
            "eval\nquit\n"
        )
        output = subprocess.run(
            [str(self.engine)], cwd=ROOT, input=commands, capture_output=True,
            text=True, check=True
        ).stdout
        lazy = int(re.search(r"EvalWindow:\s*(-?\d+)", output).group(1))
        exact = int(re.search(r"Eval:\s*(-?\d+)", output).group(1))
        self.assertNotEqual(lazy, exact)
        exact_again, _ = self.evaluate([fen])
        self.assertEqual(exact, exact_again[0])

    def test_start_position_golden_perft(self) -> None:
        output = subprocess.run(
            [str(self.engine)], cwd=ROOT,
            input="perft 1\nperft 2\nperft 3\nquit\n",
            capture_output=True, text=True, timeout=30, check=True
        ).stdout
        self.assertEqual(
            [int(value) for value in re.findall(r"Nodos\s+(\d+)", output)],
            [20, 400, 8902],
        )

    def test_pawn_cache_reports_hits_for_shared_structures(self) -> None:
        first = CORPUS[3]["fen"]
        second = CORPUS[3]["fen"].replace("R1B1K2R", "2KR1B1R")
        _, output = self.evaluate([first, second], extra="evalstats\n")
        stats = re.findall(r"pawn_hits=(\d+).*pawn_queries=(\d+)", output)
        self.assertTrue(stats)
        self.assertGreaterEqual(int(stats[-1][0]), 1)

    def test_pawn_cache_hit_matches_miss_with_different_king_context(self) -> None:
        near = "8/8/3k4/8/3P4/8/8/K7 w - - 0 1"
        far = "7k/8/8/8/3P4/8/8/K7 w - - 0 1"
        combined = self.probe([near, far])[1]
        isolated = self.probe([far])[0]
        for field in (
            "eval", "passedW", "passedB", "pawnW", "pawnB",
            "attacksW", "attacksB", "threatW", "threatB",
        ):
            self.assertEqual(combined[field], isolated[field], field)

    def test_absolute_pin_uses_only_the_real_pin_ray(self) -> None:
        fen = "4r1k1/p7/8/8/8/8/P3R3/4K3 w - - 0 1"
        probe = self.probe([fen], "12 3 0")[0]
        self.assertTrue(probe["pinsW"] & (1 << 12))
        self.assertFalse(probe["pieceEffective"] & (1 << 11))
        self.assertFalse(probe["pieceEffective"] & (1 << 13))
        # The aggregate map also includes the king's legitimate d2/f2 attacks.
        # The piece-specific fields above are the pin-ray oracle.
        self.assertTrue(probe["pseudoW"] & (1 << 11))
        self.assertTrue(probe["pseudoW"] & (1 << 13))
        self.assertTrue(probe["pieceEffective"] & (1 << 60))
        self.assertNotEqual(probe["piecePseudo"], probe["pieceEffective"])

    def test_passed_pawn_can_have_enemy_pawn_behind_it(self) -> None:
        fen = "7k/8/8/4P3/4p3/8/8/K7 w - - 0 1"
        probe = self.probe([fen])[0]
        self.assertTrue(probe["passedW"] & (1 << 36))

    def test_fifty_move_scaling_covers_lazy_and_pawnless_returns(self) -> None:
        lazy = "7k/8/8/8/8/8/4Q3/K7 w - - {} 1"
        pawnless = "7k/8/8/8/8/8/4R3/K7 w - - {} 1"
        for fen, command in ((lazy, "evalwindow 10000 10001"), (pawnless, "eval")):
            values = []
            for counter in (80, 90, 95, 100):
                fen_file = Path(self.temp.name) / "fifty.epd"
                fen_file.write_text(
                    f'[White "fifty"]\n[Black "fifty"]\n[FEN "{fen.format(counter)}"]\n'
                )
                output = subprocess.run(
                    [str(self.engine)], cwd=ROOT,
                    input=f"readfen {fen_file} 1\n{command}\nquit\n",
                    capture_output=True, text=True, timeout=30, check=True,
                ).stdout
                match = re.search(r"(?:EvalWindow|Eval):\s*(-?\d+)", output)
                self.assertIsNotNone(match, output)
                values.append(abs(int(match.group(1))))
            self.assertEqual(values, sorted(values, reverse=True))
            self.assertEqual(values[-1], 0)

    def test_distant_opposition_depends_on_side_to_move(self) -> None:
        white = "8/8/8/4k3/8/8/8/4K3 w - - 0 1"
        black = "8/8/8/4k3/8/8/8/4K3 b - - 0 1"
        white_probe, black_probe = self.probe([white, black])
        self.assertEqual((white_probe["oppositionW"], white_probe["oppositionB"]), (0, 1))
        self.assertEqual((black_probe["oppositionW"], black_probe["oppositionB"]), (1, 0))


if __name__ == "__main__":
    unittest.main()
