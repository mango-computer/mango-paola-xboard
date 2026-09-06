"""Regresión del libro Polyglot y su integración con Mango AC."""

from __future__ import annotations

import hashlib
import json
import queue
import subprocess
import tempfile
import threading
import time
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BOOK = ROOT / "polyglotbooks" / "mangoac-book.bin"
FIXTURE = ROOT / "tests" / "fixtures" / "polyglot_positions.json"
EXPECTED_SHA256 = "cb9ca1419b11d3001716f31b0977637963b0d71471d20f71e694aa0fb7463355"


class UciSession:
    def __init__(self, binary: Path, cwd: Path = ROOT):
        self.process = subprocess.Popen(
            [str(binary)],
            cwd=cwd,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1,
        )
        assert self.process.stdin is not None
        assert self.process.stdout is not None
        self.lines: queue.Queue[str] = queue.Queue()
        self.reader = threading.Thread(target=self._read_output, daemon=True)
        self.reader.start()
        self.send("uci")
        self.read_until(lambda line: line == "uciok")

    def _read_output(self) -> None:
        assert self.process.stdout is not None
        for line in self.process.stdout:
            self.lines.put(line.rstrip("\r\n"))

    def send(self, command: str) -> None:
        assert self.process.stdin is not None
        self.process.stdin.write(command + "\n")
        self.process.stdin.flush()

    def read_until(self, predicate, timeout: float = 10.0) -> list[str]:
        deadline = time.monotonic() + timeout
        received: list[str] = []
        while time.monotonic() < deadline:
            try:
                line = self.lines.get(timeout=max(0.01, deadline - time.monotonic()))
            except queue.Empty:
                break
            received.append(line)
            if predicate(line):
                return received
        self.close()
        raise AssertionError(f"Salida UCI esperada no recibida: {received[-10:]}")

    def legal_moves(self) -> set[str]:
        self.send("legal")
        lines = self.read_until(lambda line: line == "legalok")
        legal = next(line for line in lines if line.startswith("legalmoves"))
        return set(legal.split()[1:])

    def bestmove(self) -> str:
        self.send("go depth 1")
        lines = self.read_until(lambda line: line.startswith("bestmove "))
        return next(
            line.split()[1] for line in lines if line.startswith("bestmove ")
        )

    def close(self) -> None:
        if self.process.poll() is None:
            try:
                self.send("quit")
                self.process.wait(timeout=3)
            except (BrokenPipeError, subprocess.TimeoutExpired):
                self.process.kill()
                self.process.wait(timeout=3)
        if self.process.stdin is not None and not self.process.stdin.closed:
            self.process.stdin.close()
        self.reader.join(timeout=1)
        if self.process.stdout is not None and not self.process.stdout.closed:
            self.process.stdout.close()


class PolyglotBookTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.fixture = json.loads(FIXTURE.read_text(encoding="utf-8"))
        cls.cases = cls.fixture["cases"]
        cls.temp_dir = tempfile.TemporaryDirectory()
        temp = Path(cls.temp_dir.name)
        cls.harness = temp / "polyglot-harness"
        cls.engine = temp / "mangoac"

        subprocess.run(
            [
                "gcc",
                "-Wall",
                "-Wextra",
                "-O2",
                str(ROOT / "tests" / "polyglot_harness.c"),
                "-o",
                str(cls.harness),
            ],
            cwd=ROOT,
            check=True,
            capture_output=True,
            text=True,
        )
        subprocess.run(
            [
                "gcc",
                "-lm",
                str(ROOT / "bitmma3.c"),
                "-Wall",
                "-O2",
                "-o",
                str(cls.engine),
            ],
            cwd=ROOT,
            check=True,
            capture_output=True,
            text=True,
        )

    @classmethod
    def tearDownClass(cls) -> None:
        cls.temp_dir.cleanup()

    def run_harness(self, *arguments: str, check: bool = True) -> subprocess.CompletedProcess:
        return subprocess.run(
            [str(self.harness), *arguments],
            cwd=ROOT,
            check=check,
            capture_output=True,
            text=True,
        )

    def test_book_artifact_is_the_pinned_cc0_build(self) -> None:
        digest = hashlib.sha256(BOOK.read_bytes()).hexdigest()
        self.assertEqual(digest, EXPECTED_SHA256)
        self.assertEqual(BOOK.stat().st_size, 58_880)
        self.assertEqual(BOOK.stat().st_size % 16, 0)
        self.assertEqual(self.fixture["book_sha256"], EXPECTED_SHA256)

    def test_more_than_100_fen_hashes_match_the_oracle(self) -> None:
        self.assertGreater(len(self.cases), 100)
        for case in self.cases:
            with self.subTest(key=case["key"]):
                result = self.run_harness("hash", case["fen"])
                self.assertEqual(result.stdout.strip(), case["key"])

    def test_more_than_100_weighted_outputs_belong_to_the_book(self) -> None:
        self.assertGreater(len(self.cases), 100)
        for index, case in enumerate(self.cases):
            expected = {move["uci"] for move in case["moves"]}
            with self.subTest(key=case["key"]):
                result = self.run_harness(
                    "probe", str(BOOK), str(index * 2_654_435_761), case["fen"]
                )
                self.assertIn(result.stdout.strip(), expected)

    def test_seeded_selection_is_deterministic(self) -> None:
        branched = [case for case in self.cases if len(case["moves"]) > 1][:20]
        self.assertEqual(len(branched), 20)
        for case in branched:
            with self.subTest(key=case["key"]):
                first = self.run_harness(
                    "probe", str(BOOK), "42", case["fen"]
                ).stdout
                second = self.run_harness(
                    "probe", str(BOOK), "42", case["fen"]
                ).stdout
                self.assertEqual(first, second)

    def test_castling_and_promotion_move_encoding(self) -> None:
        self.assertEqual(self.run_harness("move", "263").stdout.strip(), "e1g1")
        self.assertEqual(self.run_harness("move", "19772").stdout.strip(), "e7e8q")

    def test_invalid_fen_and_corrupt_book_are_rejected(self) -> None:
        invalid_fen = self.run_harness("hash", "not-a-fen", check=False)
        self.assertNotEqual(invalid_fen.returncode, 0)
        with tempfile.NamedTemporaryFile() as corrupt:
            corrupt.write(b"not-a-polyglot-book")
            corrupt.flush()
            result = self.run_harness(
                "probe",
                corrupt.name,
                "1",
                self.cases[0]["fen"],
                check=False,
            )
        self.assertNotEqual(result.returncode, 0)

    def test_more_than_100_positions_use_legal_book_moves_through_uci(self) -> None:
        session = UciSession(self.engine)
        try:
            for case in self.cases:
                expected = {move["uci"] for move in case["moves"]}
                with self.subTest(key=case["key"]):
                    session.send("ucinewgame")
                    session.send(f"position fen {case['fen']}")
                    legal = session.legal_moves()
                    bestmove = session.bestmove()
                    self.assertIn(bestmove, legal)
                    self.assertIn(bestmove, expected)
        finally:
            session.close()

    def test_missing_book_falls_back_to_search_without_crashing(self) -> None:
        with tempfile.TemporaryDirectory() as empty_directory:
            session = UciSession(self.engine, Path(empty_directory))
            try:
                session.send("position startpos")
                legal = session.legal_moves()
                self.assertIn(session.bestmove(), legal)
            finally:
                session.close()

    def test_xboard_uses_the_same_polyglot_book(self) -> None:
        result = subprocess.run(
            [str(self.engine)],
            cwd=ROOT,
            input="xboard\nprotover 2\nnew\ngo\nquit\n",
            capture_output=True,
            text=True,
            timeout=10,
            check=True,
        )
        self.assertIn("0 0 0 0 Book move", result.stdout)
        played = next(
            line.split()[1]
            for line in result.stdout.splitlines()
            if line.startswith("move ")
        )
        start_case = next(case for case in self.cases if case["ply"] == 0)
        start_moves = {move["uci"] for move in start_case["moves"]}
        self.assertIn(played, start_moves)

    def test_three_misses_disable_book_for_the_current_game(self) -> None:
        commands = ["xboard", "protover 2", "new", "sd 1"]
        for case in self.fixture["miss_cases"]:
            commands.extend(("force", f"setboard {case['fen']}", "go"))
        commands.extend(
            (
                "force",
                "setboard rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/"
                "RNBQKBNR w KQkq - 0 1",
                "go",
                "quit",
            )
        )
        result = subprocess.run(
            [str(self.engine)],
            cwd=ROOT,
            input="\n".join(commands) + "\n",
            capture_output=True,
            text=True,
            timeout=15,
            check=True,
        )
        self.assertEqual(result.stdout.count("Book move"), 0)
        self.assertEqual(
            sum(line.startswith("move ") for line in result.stdout.splitlines()),
            4,
        )

    def test_book_can_be_disabled_from_configuration(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            cwd = Path(directory)
            (cwd / "mangoac.ini").write_text(
                "UsarLibroAperturas 0\nTamanioTablaHash 1\n",
                encoding="utf-8",
            )
            result = subprocess.run(
                [str(self.engine)],
                cwd=cwd,
                input="xboard\nprotover 2\nnew\nsd 1\ngo\nquit\n",
                capture_output=True,
                text=True,
                timeout=10,
                check=True,
            )
        self.assertNotIn("Book move", result.stdout)
        self.assertTrue(
            any(line.startswith("move ") for line in result.stdout.splitlines())
        )

    def test_rebel_reader_and_configuration_do_not_exist(self) -> None:
        self.assertFalse((ROOT / "libro2.c").exists())
        source = "\n".join(
            path.read_text(encoding="utf-8", errors="replace")
            for pattern in ("*.c", "*.h", "*.ini")
            for path in ROOT.glob(pattern)
            if path.name != "comandos_.c"
        )
        for obsolete in (
            "FIND_OPENING",
            "BOEK_",
            "RutaRandom1",
            "RutaRandom2",
            "RutaMainBook",
            "RutaTourBook",
        ):
            self.assertNotIn(obsolete, source)


if __name__ == "__main__":
    unittest.main()
