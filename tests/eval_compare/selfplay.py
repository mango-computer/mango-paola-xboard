#!/usr/bin/env python3
"""Launch a balanced, process-parallel base/candidate match with cutechess."""

from __future__ import annotations

import argparse
import concurrent.futures
import hashlib
import json
import math
import os
import re
import shutil
import subprocess
from pathlib import Path

import chess.pgn

OPENINGS = [
    "e2e4 e7e5", "d2d4 d7d5", "c2c4 e7e5", "g1f3 d7d5",
    "e2e4 c7c5", "e2e4 e7e6", "d2d4 g8f6", "c2c4 g8f6",
]
SCORE_RE = re.compile(r"\bscore cp (-?\d+)")


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def summarize(results: list[str], args: argparse.Namespace, mode: str) -> dict:
    outcomes = [1.0 if result == "candidate" else 0.0 if result == "base" else 0.5
                for result in results]
    score = sum(outcomes) / len(outcomes) if outcomes else 0.0
    variance = (
        sum((value - score) ** 2 for value in outcomes) / (len(outcomes) - 1)
        if len(outcomes) > 1 else 0.0
    )
    pair_scores = [
        outcomes[index] + outcomes[index + 1]
        for index in range(0, len(outcomes) - 1, 2)
    ]
    pair_mean = sum(pair_scores) / len(pair_scores) if pair_scores else 0.0
    pair_variance = (
        sum((value - pair_mean) ** 2 for value in pair_scores) /
        (len(pair_scores) - 1)
        if len(pair_scores) > 1 else 0.0
    )
    paired_score_error = (
        math.sqrt(pair_variance / len(pair_scores)) / 2 if pair_scores else 0.0
    )
    lower_score = max(1e-9, score - 1.96 * paired_score_error)
    upper_score = min(1 - 1e-9, score + 1.96 * paired_score_error)

    def elo(value: float) -> float:
        return 400.0 * math.log10(value / (1.0 - value))

    return {
        "base": results.count("base"),
        "candidate": results.count("candidate"),
        "draw": results.count("draw"),
        "games": len(results),
        "candidate_score": round(score, 5),
        "score_standard_error": round(math.sqrt(variance / len(outcomes)), 5)
        if outcomes else 0.0,
        "paired_score_standard_error": round(paired_score_error, 5),
        "pentanomial": {
            str(index): sum(
                1 for value in pair_scores if int(round(value * 2)) == index
            )
            for index in range(5)
        },
        "elo_difference": round(elo(score), 2) if outcomes else 0.0,
        "elo_95ci": (
            [round(elo(lower_score), 2), round(elo(upper_score), 2)]
            if outcomes else [0.0, 0.0]
        ),
        "significant_95_percent": lower_score > 0.5 or upper_score < 0.5,
        "mode": mode,
        "configuration": {
            "base": str(args.base),
            "base_sha256": sha256(args.base),
            "candidate": str(args.candidate),
            "candidate_sha256": sha256(args.candidate),
            "concurrency": args.concurrency,
            "depth": args.depth,
            "max_plies": args.max_plies,
            "tc": args.tc,
            "openings": str(args.openings) if args.openings else "builtin-balanced",
        },
    }


class Engine:
    def __init__(self, path: Path):
        self.process = subprocess.Popen(
            [str(path)], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL, text=True, bufsize=1
        )
        self.send("uci")
        self.read_until("uciok")
        self.send("isready")
        self.read_until("readyok")

    def send(self, command: str) -> None:
        assert self.process.stdin
        self.process.stdin.write(command + "\n")
        self.process.stdin.flush()

    def read_until(self, prefix: str) -> tuple[str, int]:
        assert self.process.stdout
        score = 0
        while True:
            line = self.process.stdout.readline()
            if not line:
                raise RuntimeError("engine terminated before " + prefix)
            match = SCORE_RE.search(line)
            if match:
                score = int(match.group(1))
            if line.startswith(prefix):
                return line.strip(), score

    def move(self, moves: list[str], depth: int, initial_fen: str | None = None) -> tuple[str, int]:
        suffix = " moves " + " ".join(moves) if moves else ""
        position = "position fen " + initial_fen if initial_fen else "position startpos"
        self.send(position + suffix)
        self.send(f"go depth {depth}")
        line, score = self.read_until("bestmove")
        return line.split()[1], score

    def close(self) -> None:
        if self.process.poll() is None:
            self.send("quit")
            self.process.wait(timeout=5)


def smoke_game(base: Path, candidate: Path, candidate_white: bool,
               opening: str, depth: int, max_plies: int) -> str:
    white_path = candidate if candidate_white else base
    black_path = base if candidate_white else candidate
    white, black = Engine(white_path), Engine(black_path)
    initial_fen = opening[4:] if opening.startswith("fen:") else None
    moves = [] if initial_fen else opening.split()
    initial_white = not initial_fen or initial_fen.split()[1] == "w"
    plies = 0
    decisive = []
    try:
        while plies < max_plies:
            side_white = initial_white if plies % 2 == 0 else not initial_white
            move, score = (white if side_white else black).move(
                moves, depth, initial_fen
            )
            white_score = score if side_white else -score
            if move == "0000":
                return "draw"
            moves.append(move)
            plies += 1
            decisive.append(1 if white_score > 800 else -1 if white_score < -800 else 0)
            if len(decisive) >= 4 and len(set(decisive[-4:])) == 1:
                if decisive[-1] == 1:
                    return "candidate" if candidate_white else "base"
                if decisive[-1] == -1:
                    return "base" if candidate_white else "candidate"
        return "draw"
    finally:
        white.close()
        black.close()


def load_openings(path: Path | None) -> list[str]:
    if path and path.suffix.lower() == ".json":
        rows = json.loads(path.read_text())
        return ["fen:" + row["fen"] for row in rows if "fen" in row]
    return OPENINGS


def builtin_smoke(args: argparse.Namespace) -> int:
    openings = load_openings(args.openings)
    tasks = [
        (args.base, args.candidate, game % 2 == 0,
         openings[(game // 2) % len(openings)], args.depth, args.max_plies)
        for game in range(args.games)
    ]
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.concurrency) as pool:
        results = list(pool.map(lambda values: smoke_game(*values), tasks))
    report = summarize(results, args, "builtin-smoke")
    print(json.dumps(report, indent=2, sort_keys=True))
    return 0


def read_cutechess_results(path: Path) -> list[str]:
    results = []
    with path.open(errors="replace") as stream:
        while True:
            game = chess.pgn.read_game(stream)
            if game is None:
                break
            result = game.headers.get("Result", "*")
            white = game.headers.get("White", "")
            if result == "1/2-1/2" or result == "*":
                results.append("draw")
            elif (result == "1-0" and white == "mangoac_candidato") or (
                result == "0-1" and white != "mangoac_candidato"
            ):
                results.append("candidate")
            else:
                results.append("base")
    return results


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--base", type=Path, required=True)
    parser.add_argument("--candidate", type=Path, required=True)
    parser.add_argument("--games", type=int, default=200)
    parser.add_argument("--concurrency", type=int, default=max(1, (os.cpu_count() or 2) // 2))
    parser.add_argument("--openings", type=Path)
    parser.add_argument("--pgn", type=Path, default=Path("hce-match.pgn"))
    parser.add_argument("--tc", default="10+0.1")
    parser.add_argument("--depth", type=int, default=4)
    parser.add_argument("--max-plies", type=int, default=100)
    args = parser.parse_args()
    cutechess = shutil.which("cutechess-cli")
    if not cutechess:
        return builtin_smoke(args)
    command = [
        cutechess,
        "-engine", f"cmd={args.base}", "name=mangoac_base", "proto=uci",
        "-engine", f"cmd={args.candidate}", "name=mangoac_candidato", "proto=uci",
        "-each", f"tc={args.tc}", "restart=on",
        "-games", "2", "-rounds", str(max(1, args.games // 2)),
        "-repeat", "-concurrency", str(args.concurrency),
        "-draw", "movenumber=40", "movecount=8", "score=10",
        "-resign", "movecount=4", "score=600",
        "-pgnout", str(args.pgn),
    ]
    if args.openings:
        command.extend(("-openings", f"file={args.openings}", "order=random"))
    completed = subprocess.run(command, capture_output=True, text=True, check=False)
    if completed.returncode:
        print(completed.stdout)
        print(completed.stderr)
        return completed.returncode
    results = read_cutechess_results(args.pgn)
    print(json.dumps(summarize(results, args, "cutechess"), indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
