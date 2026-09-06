#!/usr/bin/env python3
"""Build a deterministic, stratified HCE corpus from project PGN games."""

from __future__ import annotations

import argparse
import json
from collections import Counter
from pathlib import Path

import chess
import chess.pgn

ROOT = Path(__file__).resolve().parents[2]
PROJECT = Path(__file__).resolve().parents[5]
DEFAULT_INPUT = PROJECT / "mango-paola-master" / "partidasPGN"
DEFAULT_SEED = ROOT / "tests" / "eval_corpus.json"


def family(board: chess.Board, ply: int) -> str:
    queens = len(board.pieces(chess.QUEEN, chess.WHITE) | board.pieces(chess.QUEEN, chess.BLACK))
    rooks = len(board.pieces(chess.ROOK, chess.WHITE) | board.pieces(chess.ROOK, chess.BLACK))
    minors = sum(
        len(board.pieces(piece, color))
        for piece in (chess.KNIGHT, chess.BISHOP)
        for color in (chess.WHITE, chess.BLACK)
    )
    pawns = len(board.pieces(chess.PAWN, chess.WHITE) | board.pieces(chess.PAWN, chess.BLACK))
    if ply <= 20:
        return "opening"
    if queens and ply <= 60:
        return "king_safety"
    if queens or rooks + minors > 6:
        return "middlegame"
    if rooks:
        return "rook_endgame"
    if minors:
        return "minor_endgame"
    if pawns:
        return "pawn_endgame"
    return "bare_kings"


def generated_rows(directory: Path, target: int, stride: int) -> list[dict]:
    rows: list[dict] = []
    seen: set[str] = set()
    for path in sorted(directory.glob("*.pgn")):
        with path.open(errors="replace") as stream:
            game_index = 0
            while len(rows) < target:
                game = chess.pgn.read_game(stream)
                if game is None:
                    break
                game_index += 1
                board = game.board()
                for ply, move in enumerate(game.mainline_moves(), start=1):
                    board.push(move)
                    if ply < 12 or ply % stride or board.is_check() or board.is_game_over():
                        continue
                    fen = board.fen()
                    identity = " ".join(fen.split()[:4])
                    if identity in seen:
                        continue
                    seen.add(identity)
                    rows.append({
                        "id": f"pgn-{path.stem.lower()}-{game_index}-{ply}",
                        "family": family(board, ply),
                        "fen": fen,
                        "source": path.name,
                        "ply": ply,
                    })
                    if len(rows) >= target:
                        break
    return rows


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", type=Path, default=DEFAULT_INPUT)
    parser.add_argument("--seed", type=Path, default=DEFAULT_SEED)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--target", type=int, default=500)
    parser.add_argument("--stride", type=int, default=6)
    args = parser.parse_args()

    seed = json.loads(args.seed.read_text())
    generated = generated_rows(args.input, max(0, args.target - len(seed)), args.stride)
    rows = seed + generated
    args.output.write_text(json.dumps(rows, indent=2) + "\n")
    print(json.dumps({
        "positions": len(rows),
        "seed_positions": len(seed),
        "generated_positions": len(generated),
        "families": dict(sorted(Counter(row["family"] for row in rows).items())),
    }, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
