"""Regenera el corpus de regresión desde el libro Polyglot versionado.

Requiere python-chess 1.11.2. El test normal consume únicamente el JSON
generado y no necesita dependencias Python externas.
"""

from __future__ import annotations

import argparse
import json
from collections import deque
from pathlib import Path

import chess
import chess.polyglot

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_BOOK = ROOT / "polyglotbooks" / "mangoac-book.bin"
DEFAULT_OUTPUT = ROOT / "tests" / "fixtures" / "polyglot_positions.json"
CASE_COUNT = 140


def collect_positions(book_path: Path) -> list[dict[str, object]]:
    queue: deque[tuple[chess.Board, list[str]]] = deque([(chess.Board(), [])])
    seen: set[int] = set()
    positions: list[dict[str, object]] = []

    with chess.polyglot.open_reader(book_path) as reader:
        while queue:
            board, sequence = queue.popleft()
            key = chess.polyglot.zobrist_hash(board)
            if key in seen:
                continue
            seen.add(key)

            entries = list(reader.find_all(board))
            fen = board.fen(en_passant="fen")
            positions.append(
                {
                    "fen": fen,
                    "key": f"{key:016x}",
                    "ply": len(sequence),
                    "sequence": sequence,
                    "moves": [
                        {"uci": entry.move.uci(), "weight": entry.weight}
                        for entry in entries
                    ],
                }
            )

            for entry in entries:
                move = entry.move
                if move not in board.legal_moves:
                    raise ValueError(f"Jugada ilegal {move.uci()} en {board.fen()}")
                child = board.copy(stack=False)
                child.push(move)
                queue.append((child, [*sequence, move.uci()]))

    return positions


def select_cases(positions: list[dict[str, object]]) -> list[dict[str, object]]:
    positions = [item for item in positions if item["moves"]]
    selected: list[dict[str, object]] = []
    selected_keys: set[str] = set()

    categories = (
        lambda item: item["fen"].split()[3] != "-",
        lambda item: len(item["moves"]) > 1,
        lambda item: item["fen"].split()[2] == "-",
        lambda item: item["ply"] >= 16,
        lambda _item: True,
    )
    limits = (20, 35, 20, 35, CASE_COUNT)

    for predicate, limit in zip(categories, limits):
        added = 0
        for item in positions:
            key = str(item["key"])
            if key in selected_keys or not predicate(item):
                continue
            selected.append(item)
            selected_keys.add(key)
            added += 1
            if len(selected) >= CASE_COUNT or added >= limit:
                break
        if len(selected) >= CASE_COUNT:
            break

    if len(selected) < CASE_COUNT:
        raise ValueError(
            f"El libro solo produjo {len(selected)} casos; se requieren {CASE_COUNT}"
        )
    return selected


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--book", type=Path, default=DEFAULT_BOOK)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    args = parser.parse_args()

    positions = collect_positions(args.book)
    cases = select_cases(positions)
    miss_cases = [item for item in positions if not item["moves"]][:3]
    if len(miss_cases) != 3:
        raise ValueError("No se encontraron tres posiciones sin entrada")
    payload = {
        "book_sha256": (
            "cb9ca1419b11d3001716f31b0977637963"
            "b0d71471d20f71e694aa0fb7463355"
        ),
        "case_count": len(cases),
        "cases": cases,
        "miss_cases": miss_cases,
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(
        json.dumps(payload, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
