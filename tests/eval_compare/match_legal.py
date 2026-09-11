#!/usr/bin/env python3
"""Paired legal match between two UCI Mango AC binaries."""

from __future__ import annotations

import argparse
import concurrent.futures
import hashlib
import json
import math
import os
import select
import time
from pathlib import Path
import subprocess

import chess
import chess.pgn


OPENINGS = [
    "e2e4 e7e5",
    "e2e4 c7c5",
    "e2e4 e7e6",
    "e2e4 c7c6",
    "e2e4 d7d6",
    "e2e4 g7g6",
    "d2d4 d7d5",
    "d2d4 g8f6",
    "d2d4 e7e6",
    "d2d4 f7f5",
    "c2c4 e7e5",
    "c2c4 c7c5",
    "c2c4 g8f6",
    "g1f3 d7d5",
    "g1f3 g8f6",
    "g1f3 c7c5",
    "e2e4 e7e5 g1f3 b8c6 f1b5",
    "e2e4 e7e5 g1f3 b8c6 f1c4",
    "e2e4 e7e5 g1f3 g8f6",
    "e2e4 c7c5 g1f3 d7d6 d2d4",
    "d2d4 d7d5 c2c4",
    "d2d4 g8f6 c2c4 e7e6",
    "d2d4 g8f6 c2c4 g7g6",
    "e2e4 e7e6 d2d4 d7d5",
]


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def elo_from_score(score: float) -> float:
    score = min(1 - 1e-9, max(1e-9, score))
    return 400.0 * math.log10(score / (1.0 - score))


class Engine:
    def __init__(self, path: Path, cwd: Path, hash_mb: int, name: str) -> None:
        self.name = name
        self._buffer = b""
        self.process = subprocess.Popen(
            [str(path)],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            cwd=str(cwd),
        )
        self.send("uci")
        self.read_until("uciok", 15)
        self.send("setoption name Threads value 1")
        self.send(f"setoption name Hash value {hash_mb}")
        self.send("isready")
        self.read_until("readyok", 15)
        self.send("ucinewgame")
        self.send("isready")
        self.read_until("readyok", 15)

    def send(self, command: str) -> None:
        assert self.process.stdin
        self.process.stdin.write((command + "\n").encode("ascii"))
        self.process.stdin.flush()

    def read_until(self, prefix: str, timeout: float) -> str:
        assert self.process.stdout
        deadline = time.monotonic() + timeout
        while True:
            for line in self._drain_lines():
                if line.startswith(prefix):
                    return line
            remaining = deadline - time.monotonic()
            if remaining <= 0:
                raise TimeoutError(f"{self.name} timed out waiting for {prefix}")
            ready, _, _ = select.select([self.process.stdout], [], [], remaining)
            if not ready:
                raise TimeoutError(f"{self.name} timed out waiting for {prefix}")
            chunk = os.read(self.process.stdout.fileno(), 4096)
            if not chunk:
                raise RuntimeError(f"{self.name} terminated before {prefix}")
            self._buffer += chunk

    def _drain_lines(self) -> list[str]:
        lines = []
        while b"\n" in self._buffer:
            raw, self._buffer = self._buffer.split(b"\n", 1)
            line = raw.decode("utf-8", "replace").strip()
            if line:
                lines.append(line)
        return lines

    def bestmove(
        self,
        board: chess.Board,
        depth: int | None,
        movetime: int | None,
        timeout: float,
    ) -> str:
        self.send(f"position fen {board.fen()}")
        if movetime is not None:
            self.send(f"go movetime {movetime}")
        else:
            self.send(f"go depth {depth}")
        line = self.read_until("bestmove", timeout)
        parts = line.split()
        if len(parts) < 2:
            return "0000"
        return parts[1]

    def close(self) -> None:
        if self.process.poll() is None:
            try:
                self.send("quit")
                self.process.wait(timeout=5)
            except Exception:
                self.process.kill()


def play_game(
    white_path: Path,
    black_path: Path,
    cwd: Path,
    opening: str,
    depth: int | None,
    movetime: int | None,
    max_plies: int,
    hash_mb: int,
    timeout: float,
) -> dict:
    board = chess.Board()
    for token in opening.split():
        board.push_uci(token)
    opening_plies = len(board.move_stack)
    white = Engine(white_path, cwd, hash_mb, "white")
    black = Engine(black_path, cwd, hash_mb, "black")
    termination = "unknown"
    try:
        while True:
            if board.is_checkmate():
                termination = "checkmate"
                break
            if board.is_stalemate():
                termination = "stalemate"
                break
            if board.is_insufficient_material():
                termination = "insufficient"
                break
            if board.can_claim_fifty_moves() or board.is_fifty_moves():
                termination = "fifty"
                break
            if board.can_claim_threefold_repetition() or board.is_repetition():
                termination = "repetition"
                break
            if len(board.move_stack) - opening_plies >= max_plies:
                termination = "max_plies"
                break
            engine = white if board.turn == chess.WHITE else black
            try:
                move_uci = engine.bestmove(board, depth, movetime, timeout)
            except Exception as error:
                termination = f"crash:{error}"
                winner = chess.BLACK if board.turn == chess.WHITE else chess.WHITE
                return finish(board, opening, white_path, black_path, termination, winner)
            try:
                move = chess.Move.from_uci(move_uci)
            except ValueError:
                move = None
            if move is None or move not in board.legal_moves:
                termination = f"illegal:{move_uci}"
                winner = chess.BLACK if board.turn == chess.WHITE else chess.WHITE
                return finish(board, opening, white_path, black_path, termination, winner)
            board.push(move)
        winner = None
        if board.is_checkmate():
            winner = not board.turn
        return finish(board, opening, white_path, black_path, termination, winner)
    finally:
        white.close()
        black.close()


def finish(
    board: chess.Board,
    opening: str,
    white_path: Path,
    black_path: Path,
    termination: str,
    winner: bool | None,
) -> dict:
    if winner is True:
        result = "1-0"
    elif winner is False:
        result = "0-1"
    else:
        result = "1/2-1/2"
    game = chess.pgn.Game.from_board(board)
    game.headers["White"] = white_path.name
    game.headers["Black"] = black_path.name
    game.headers["Result"] = result
    game.headers["Opening"] = opening
    game.headers["Termination"] = termination
    return {
        "result": result,
        "termination": termination,
        "plies": len(board.move_stack),
        "pgn": str(game),
        "fen": board.fen(),
    }


def classify(result: str, candidate_white: bool) -> str:
    if result == "1/2-1/2":
        return "draw"
    if result == "1-0":
        return "candidate" if candidate_white else "base"
    return "base" if candidate_white else "candidate"


def summarize(rows: list[dict], args: argparse.Namespace, elapsed: float) -> dict:
    outcomes = [1.0 if row["winner"] == "candidate" else 0.0 if row["winner"] == "base" else 0.5 for row in rows]
    score = sum(outcomes) / len(outcomes) if outcomes else 0.0
    pair_scores = [
        outcomes[index] + outcomes[index + 1]
        for index in range(0, len(outcomes) - 1, 2)
    ]
    pair_mean = sum(pair_scores) / len(pair_scores) if pair_scores else 0.0
    pair_variance = (
        sum((value - pair_mean) ** 2 for value in pair_scores) / (len(pair_scores) - 1)
        if len(pair_scores) > 1 else 0.0
    )
    paired_error = math.sqrt(pair_variance / len(pair_scores)) / 2 if pair_scores else 0.0
    lower = max(1e-9, score - 1.96 * paired_error)
    upper = min(1 - 1e-9, score + 1.96 * paired_error)
    return {
        "base": sum(1 for row in rows if row["winner"] == "base"),
        "candidate": sum(1 for row in rows if row["winner"] == "candidate"),
        "draw": sum(1 for row in rows if row["winner"] == "draw"),
        "games": len(rows),
        "candidate_score": round(score, 5),
        "paired_score_standard_error": round(paired_error, 5),
        "elo_difference": round(elo_from_score(score), 2) if outcomes else 0.0,
        "elo_95ci": [round(elo_from_score(lower), 2), round(elo_from_score(upper), 2)],
        "significant_95_percent": lower > 0.5 or upper < 0.5,
        "illegal_or_crash": sum(
            1 for row in rows if row["termination"].startswith(("illegal", "crash"))
        ),
        "elapsed_seconds": round(elapsed, 1),
        "configuration": {
            "base": str(args.base),
            "base_sha256": sha256(args.base),
            "candidate": str(args.candidate),
            "candidate_sha256": sha256(args.candidate),
            "concurrency": args.concurrency,
            "depth": args.depth,
            "movetime": args.movetime,
            "max_plies": args.max_plies,
            "hash_mb": args.hash_mb,
            "games": args.games,
        },
    }


def run_one(args: argparse.Namespace, game: int) -> dict:
    opening = OPENINGS[(game // 2) % len(OPENINGS)]
    candidate_white = game % 2 == 0
    white = args.candidate if candidate_white else args.base
    black = args.base if candidate_white else args.candidate
    timeout = args.move_timeout
    played = play_game(
        white,
        black,
        args.cwd,
        opening,
        args.depth,
        args.movetime,
        args.max_plies,
        args.hash_mb,
        timeout,
    )
    played["winner"] = classify(played["result"], candidate_white)
    played["candidate_white"] = candidate_white
    played["opening"] = opening
    played["game"] = game
    return played


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--base", type=Path, required=True)
    parser.add_argument("--candidate", type=Path, required=True)
    parser.add_argument("--games", type=int, default=1000)
    parser.add_argument("--concurrency", type=int, default=max(1, (os.cpu_count() or 2) // 2))
    parser.add_argument("--depth", type=int, default=8)
    parser.add_argument("--movetime", type=int)
    parser.add_argument("--max-plies", type=int, default=120)
    parser.add_argument("--hash-mb", type=int, default=64)
    parser.add_argument("--move-timeout", type=float, default=60.0)
    parser.add_argument("--cwd", type=Path, required=True)
    parser.add_argument("--pgn", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    args.base = args.base.resolve()
    args.candidate = args.candidate.resolve()
    args.cwd = args.cwd.resolve()
    args.pgn = args.pgn.resolve()
    args.output = args.output.resolve()
    args.games += args.games % 2
    started = time.monotonic()
    rows = []
    progress = args.output.with_suffix(".progress.json")
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.concurrency) as pool:
        futures = [pool.submit(run_one, args, game) for game in range(args.games)]
        for future in concurrent.futures.as_completed(futures):
            rows.append(future.result())
            if len(rows) % max(1, args.concurrency) == 0 or len(rows) == args.games:
                progress.write_text(
                    json.dumps(
                        {
                            "completed": len(rows),
                            "games": args.games,
                            "elapsed_seconds": round(time.monotonic() - started, 1),
                        }
                    )
                    + "\n",
                    encoding="utf-8",
                )
    rows.sort(key=lambda row: row["game"])
    report = summarize(rows, args, time.monotonic() - started)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.pgn.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    with args.pgn.open("w", encoding="utf-8") as stream:
        for row in rows:
            stream.write(row["pgn"].rstrip() + "\n\n")
    print(json.dumps(report, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
