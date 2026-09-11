"""Regression tests for Mango AC's static exchange evaluation (EET)."""

from __future__ import annotations

import json
import re
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CASES = json.loads((ROOT / "tests" / "fixtures" / "eet_cases.json").read_text())
CORPUS = json.loads((ROOT / "tests" / "eval_corpus.json").read_text())
MOVE_RE = re.compile(
    r"Ori=(?P<origin>\d+), Des=(?P<destination>\d+), .* "
    r"Promocion=(?P<promotion>\d+), .* EET=(?P<eet>-?\d+)"
)
COMPARE_RE = re.compile(r"SEECompare:\s*(.*)")


class EetRegressionTests(unittest.TestCase):
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

    def probe_move(
        self, fen: str, origin: int, destination: int, promotion: int = 0
    ) -> int:
        epd = Path(self.temp.name) / "eet-position.epd"
        epd.write_text(f'[White "EET"]\n[Black "EET"]\n[FEN "{fen}"]\n')
        output = subprocess.run(
            [str(self.engine)],
            cwd=ROOT,
            input=f"readfen {epd} 1\ngenmovcap\nmovimientos\nquit\n",
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        ).stdout

        moves = {
            (
                int(match["origin"]),
                int(match["destination"]),
                int(match["promotion"]),
            ): int(match["eet"])
            for match in MOVE_RE.finditer(output)
        }
        self.assertIn((origin, destination, promotion), moves, output)
        return moves[(origin, destination, promotion)]

    def compare_with_oracle(self, fen: str) -> list[dict[str, int]]:
        epd = Path(self.temp.name) / "eet-oracle.epd"
        epd.write_text(f'[White "EET"]\n[Black "EET"]\n[FEN "{fen}"]\n')
        output = subprocess.run(
            [str(self.engine)],
            cwd=ROOT,
            input=f"readfen {epd} 1\neetall\nquit\n",
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        ).stdout
        return [
            {
                key: int(value)
                for key, value in re.findall(r"(\w+)=(-?\d+)", match.group(1))
            }
            for match in COMPARE_RE.finditer(output)
        ]

    def search_probe(self, fen: str, depth: int) -> dict[str, int]:
        epd = Path(self.temp.name) / "eet-search.epd"
        epd.write_text(f'[White "EET"]\n[Black "EET"]\n[FEN "{fen}"]\n')
        output = subprocess.run(
            [str(self.engine)],
            cwd=ROOT,
            input=(
                f"readfen {epd} 1\n"
                f"searchprobe {depth} -100000 100000\n"
                "quit\n"
            ),
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        ).stdout
        match = re.search(r"SearchProbe:\s*(.*)", output)
        self.assertIsNotNone(match, output)
        return {
            key: int(value)
            for key, value in re.findall(r"(\w+)=(-?\d+)", match.group(1))
        }

    def perft(self, fen: str, depths: list[int]) -> list[int]:
        epd = Path(self.temp.name) / "eet-perft.epd"
        epd.write_text(f'[White "EET"]\n[Black "EET"]\n[FEN "{fen}"]\n')
        commands = f"readfen {epd} 1\n"
        commands += "".join(f"perft {depth}\n" for depth in depths)
        commands += "quit\n"
        output = subprocess.run(
            [str(self.engine)],
            cwd=ROOT,
            input=commands,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        ).stdout
        return [int(value) for value in re.findall(r"Nodos\s+(\d+)", output)]

    def test_golden_exchange_values(self) -> None:
        for case in CASES:
            with self.subTest(case=case["id"]):
                actual = self.probe_move(
                    case["fen"], case["origin"], case["destination"],
                    case.get("promotion", 0),
                )
                self.assertEqual(actual, case["expected"])

    def test_color_symmetric_clean_capture(self) -> None:
        white = next(case for case in CASES if case["id"] == "rook_takes_undefended_pawn")
        black = next(
            case for case in CASES
            if case["id"] == "black_rook_takes_undefended_pawn"
        )
        self.assertEqual(
            self.probe_move(white["fen"], white["origin"], white["destination"]),
            self.probe_move(black["fen"], black["origin"], black["destination"]),
        )

    def test_fast_see_matches_slow_legal_oracle(self) -> None:
        fens = [case["fen"] for case in CASES]
        fens.extend(row["fen"] for row in CORPUS[:100])
        comparisons = 0
        thresholds = {
            "geNeg1000": -1000,
            "geNeg500": -500,
            "geNeg100": -100,
            "geNeg15": -15,
            "ge0": 0,
            "ge1": 1,
            "ge100": 100,
            "ge500": 500,
            "ge1000": 1000,
        }
        for fen in dict.fromkeys(fens):
            rows = self.compare_with_oracle(fen)
            comparisons += len(rows)
            for row in rows:
                with self.subTest(fen=fen, move=(row["origin"], row["destination"])):
                    self.assertEqual(row["exact"], row["reference"])
                    for field, threshold in thresholds.items():
                        self.assertEqual(
                            row[field], int(row["exact"] >= threshold),
                            (field, threshold, row),
                        )
        self.assertGreaterEqual(comparisons, 150)

    def test_qsearch_prunes_a_losing_capture_using_pre_move_eet(self) -> None:
        fen = "r1bq1rk1/ppp5/8/8/8/6n1/PPP1p3/R1B1Q1KR w - - 0 1"
        result = self.search_probe(fen, 0)
        self.assertGreaterEqual(result["eet_prunes"], 1)

    def test_losing_checking_capture_activates_eet_extension(self) -> None:
        fen = "4k1n1/4p3/8/8/8/8/4Q3/6K1 w - - 0 1"
        result = self.search_probe(fen, 1)
        self.assertGreaterEqual(result["eet_extensions"], 1)

    def test_perft_guards_move_legality(self) -> None:
        start = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
        kiwipete = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
        self.assertEqual(self.perft(start, [4]), [197281])
        self.assertEqual(self.perft(kiwipete, [1, 2]), [48, 2039])


if __name__ == "__main__":
    unittest.main()
