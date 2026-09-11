"""Regression tests for long UCI input lines."""

from __future__ import annotations

import os
import queue
import subprocess
import tempfile
import threading
import time
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
INITIAL_CAPACITY = 5120
MAX_CAPACITY = 256 * 1024

ISSUE_GAME_1 = (
    "e2e4 c7c5 g1f3 b8c6 f1b5 e7e6 e1g1 f8d6 b1a3 a7a6 b5c6 d7c6 a3c4 d6c7 "
    "b2b3 b7b5 c4e3 g8f6 e4e5 f6d5 c1b2 e8g8 d2d4 d8e7 d4c5 d5e3 f2e3 e7c5 "
    "d1d4 c5d4 b2d4 h7h6 a1d1 f8d8 g1f2 c8b7 g2g3 g7g5 h2h3 a6a5 d1d3 a5a4 "
    "b3b4 d8d7 f3d2 c6c5 b4c5 c7a5 f1b1 b7c6 h3h4 g5h4 g3h4 g8h7 a2a3 a8g8 "
    "d2f3 g8g4 d3d1 h7g6 d1g1 g4g1 b1g1 g6h7 h4h5 b5b4 a3b4 a5b4 f3h2 d7d8 "
    "h2g4 b4a5 g4f6 h7h8 f2g3 a5c7 g3f4 a4a3 d4c3 a3a2 c3d4 d8a8 f6e4 c7d8 "
    "g1a1 c6d5 e4f6 d5b7 f6e4 d8h4 e4d6 b7c6 d6f7 h8g7 f7d6 h4g5 f4g4 g7f8 "
    "g4g3 f8e7 e3e4 a8a3 g3g4 g5d2 d4b2 a3a4 g4f3 d2g5 f3f2 a4a5 b2d4 a5a3 "
    "d4b2 a3a6 f2e2 a6a8 e2d3 g5f4 a1f1 f4g5 f1f7 e7d8 f7f8 d8d7 f8f1 d7e7 "
    "f1f7 e7d8 f7f8 d8d7 f8a8 c6a8 d6f7 g5f4 f7d6 a8c6 b2a1 c6a4 d6c4 d7c6 "
    "c4b6 a4b5 c2c4 b5a6 b6a4 f4g3 a1b2 g3e1 b2a1 e1b4 d3d4 b4e1 a4b2 a6c8 "
    "b2d1 c8d7 d1c3 e1c3 a1c3 d7e8 d4d3 e8h5 c3d4 h5f3 d4a1 h6h5 a1d4 h5h4 "
    "d3e3 f3d1 e3f2 d1b3 f2f3 b3c4 f3g4 c4d3 g4h4 d3e4 h4g5 c6d5 d4b2 d5c5 "
    "g5f4 c5d5 f4e3 e4f5 b2c3 d5c5 c3b2 c5c4 b2a1 f5c2 a1b2 c4b3 b2d4 c2f5 "
    "e3d2 b3c4 d4b2 f5e4 d2e3"
)

ISSUE_GAME_2 = (
    "d2d4 g8f6 c2c4 g7g6 g1f3 c7c5 d4c5 b8a6 b1c3 a6c5 b2b4 c5e6 f3d4 f8g7 "
    "e2e3 e8g8 f1e2 d7d5 e1g1 e6d4 e3d4 d5c4 c1g5 c8e6 a1c1 d8d6 d4d5 f6d5 "
    "c3d5 d6d5 d1d5 e6d5 e2c4 d5c4 c1c4 f7f6 g5e3 a7a5 a2a4 g8f7 b4a5 a8a5 "
    "c4c7 a5a4 c7b7 a4c4 f1e1 f8e8 g2g3 g6g5 e1d1 g5g4 d1d5 e8a8 h2h3 g4h3 "
    "g1h2 f7e6 d5d7 a8e8 h2h3 h7h5 e3d4 c4c2 d4e3 f6f5 b7c7 c2c7 d7c7 g7e5 "
    "c7c6 e5d6 h3h4 e8h8 c6c4 e6d5 c4d4 d5e6 d4c4 e6d5 c4d4 d5e5 f2f4 e5e6 "
    "d4c4 h8g8 e3f2 e6f6 f2d4 f6g6 c4c2 g8b8 c2e2 b8b4 d4c3 b4c4 e2e6 g6f7 "
    "e6e3 d6c5 e3d3 f7g6 h4h3 c5d6 h3h4 c4e4 d3d2 e4e6 c3a5 e6e3 d2d1 e3a3 "
    "a5b6 a3a2 b6g1 d6b4 h4h3 b4c3 g1c5 c3f6 d1d7 g6f7 d7d1 a2b2 d1d7 b2c2 "
    "c5b4 f7e6 d7d1 c2b2 b4c5 b2b5 c5a7 b5d5 d1c1 d5a5 c1e1 e6d5 a7b6 a5a2 "
    "e1d1 d5e4 d1e1 e4d5 e1d1 d5e4 d1e1 e4d3 b6c5 a2e2 e1d1 d3c4 c5g1 e2a2 "
    "d1d7 a2b2 g1a7 b2e2 a7b6 c4b5 b6g1 b5c6 d7d3 e7e6 g1d4 f6e7 d4e5 e2c2 "
    "d3d1 c2a2 d1d4 a2a5 d4d1 a5a3 d1c1 c6d5 c1d1 d5e4 e5d6 a3a7 d1e1 e4f3 "
    "d6e7 a7e7 e1b1 e7g7 b1b3 f3e2 b3b2 e2f1 b2b1 f1f2 b1b2 f2e3 b2b3 e3d2 "
    "b3b5 g7g6 b5e5 d2d3 e5e1 d3c4"
)


class UciSession:
    def __init__(self, binary: Path):
        self.process = subprocess.Popen(
            [str(binary)],
            cwd=ROOT,
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

    def read_until(self, predicate, timeout: float = 8.0) -> list[str]:
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
        raise AssertionError(f"Expected UCI output not received. Last lines: {received[-10:]}")

    def legal_moves(self) -> set[str]:
        self.send("legal")
        output = self.read_until(lambda line: line == "legalok")
        legal = next(line for line in output if line.startswith("legalmoves"))
        return set(legal.split()[1:])

    def bestmove(self) -> str:
        self.send("go depth 1")
        output = self.read_until(lambda line: line.startswith("bestmove "), timeout=15.0)
        return next(line.split()[1] for line in output if line.startswith("bestmove "))

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


class LongUciCommandTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        configured_binary = os.environ.get("MANGOAC_TEST_BINARY")
        if configured_binary:
            cls.temp_dir = None
            cls.binary = Path(configured_binary).resolve()
            if not cls.binary.is_file():
                raise FileNotFoundError(cls.binary)
            return

        cls.temp_dir = tempfile.TemporaryDirectory()
        cls.binary = Path(cls.temp_dir.name) / "mangoac"
        subprocess.run(
            [
                "gcc",
                "-lm",
                "-pthread",
                str(ROOT / "bitmma3.c"),
                "-Wall",
                "-O2",
                "-o",
                str(cls.binary),
            ],
            cwd=ROOT,
            check=True,
            capture_output=True,
            text=True,
        )

    @classmethod
    def tearDownClass(cls) -> None:
        if cls.temp_dir is not None:
            cls.temp_dir.cleanup()

    def assert_position_and_bestmove(self, moves: str, forbidden: str) -> None:
        command = f"position startpos moves {moves}"
        self.assertGreater(len(command), 1024)
        session = UciSession(self.binary)
        try:
            session.send(command)
            legal = session.legal_moves()
            best = session.bestmove()
            self.assertIn(best, legal)
            self.assertNotEqual(best, forbidden)
        finally:
            session.close()

    def test_issue_game_1_reaches_ply_201(self) -> None:
        self.assertEqual(len(ISSUE_GAME_1.split()), 201)
        self.assert_position_and_bestmove(ISSUE_GAME_1, "b2a1")

    def test_issue_game_2_reaches_ply_202(self) -> None:
        self.assertEqual(len(ISSUE_GAME_2.split()), 202)
        self.assert_position_and_bestmove(ISSUE_GAME_2, "e5e1")

    def test_command_grows_beyond_initial_capacity(self) -> None:
        cycle = "g1f3 g8f6 f3g1 f6g8"
        moves = " ".join([cycle] * 260)
        command = f"position startpos moves {moves}"
        self.assertGreater(len(command), INITIAL_CAPACITY)
        session = UciSession(self.binary)
        try:
            session.send(command)
            actual = session.legal_moves()
            session.send("position startpos")
            expected = session.legal_moves()
            self.assertEqual(actual, expected)
        finally:
            session.close()

    def test_fen_with_capture_and_promotions(self) -> None:
        session = UciSession(self.binary)
        try:
            session.send("position startpos moves e2e4 d7d5 e4d5")
            self.assertTrue(session.legal_moves())
            session.send(
                "position fen 8/P7/7k/8/8/8/6p1/K7 w - - 0 1 "
                "moves a7a8q g2g1q"
            )
            self.assertTrue(session.legal_moves())
        finally:
            session.close()

    def test_over_limit_command_is_rejected_and_protocol_recovers(self) -> None:
        session = UciSession(self.binary)
        try:
            expected = session.legal_moves()
            session.send("position " + "x" * MAX_CAPACITY)
            session.send("isready")
            output = session.read_until(lambda line: line == "readyok")
            self.assertIn("info string error: UCI command too long", output)
            self.assertEqual(session.legal_moves(), expected)
        finally:
            session.close()

    def test_final_command_without_newline_is_processed_at_eof(self) -> None:
        result = subprocess.run(
            [str(self.binary)],
            cwd=ROOT,
            input="uci\nisready",
            capture_output=True,
            text=True,
            timeout=8,
            check=True,
        )
        self.assertIn("readyok", result.stdout.splitlines())


if __name__ == "__main__":
    unittest.main()
