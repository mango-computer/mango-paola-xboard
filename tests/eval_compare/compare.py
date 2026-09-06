#!/usr/bin/env python3
"""Reproducible Mango base/candidate/Stockfish-HCE evaluation comparison."""

from __future__ import annotations

import argparse
import hashlib
import json
import math
import random
import re
import statistics
import subprocess
import tempfile
import time
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_CORPUS = ROOT / "tests" / "eval_corpus.json"
MANGO_EVAL_RE = re.compile(r"Eval:\s*(-?\d+)")
SF_EVAL_RE = re.compile(r"Final evaluation\s+([+-]?\d+(?:\.\d+)?)")


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def run(command: list[str], stdin: str, timeout: int = 120) -> tuple[str, float]:
    start = time.perf_counter()
    completed = subprocess.run(
        command, input=stdin, text=True, capture_output=True, timeout=timeout
    )
    if completed.returncode:
        raise RuntimeError(
            f"{' '.join(command)} failed ({completed.returncode}):\n{completed.stderr}"
        )
    return completed.stdout, (time.perf_counter() - start) * 1000


def mango_evals(engine: Path, rows: list[dict]) -> tuple[list[int], float]:
    with tempfile.NamedTemporaryFile("w", suffix=".epd", delete=False) as stream:
        fen_file = Path(stream.name)
        for row in rows:
            stream.write('[White "corpus"]\n[Black "corpus"]\n')
            stream.write(f'[FEN "{row["fen"]}"]\n\n')
    try:
        commands = "".join(
            f"readfen {fen_file} {index}\neval\n"
            for index in range(1, len(rows) + 1)
        ) + "quit\n"
        output, elapsed = run([str(engine)], commands)
    finally:
        fen_file.unlink(missing_ok=True)
    values = [int(match) for match in MANGO_EVAL_RE.findall(output)]
    if len(values) != len(rows):
        raise RuntimeError(f"Mango returned {len(values)} evaluations for {len(rows)} FENs")
    # Mango reports side-to-move; all stored values use White's perspective.
    return [
        value if row["fen"].split()[1] == "w" else -value
        for value, row in zip(values, rows)
    ], elapsed


def stockfish_evals(engine: Path, rows: list[dict]) -> tuple[list[int], float]:
    commands = ["uci", "setoption name Use NNUE value false", "isready"]
    for row in rows:
        commands.extend((f'position fen {row["fen"]}', "eval"))
    commands.append("quit")
    output, elapsed = run([str(engine)], "\n".join(commands) + "\n")
    # Stockfish's classical eval output is in pawns and already from White's view.
    values = [round(float(match) * 100) for match in SF_EVAL_RE.findall(output)]
    if len(values) != len(rows):
        raise RuntimeError(
            f"Stockfish returned {len(values)} evaluations for {len(rows)} FENs; "
            "verify commit 6a8767a and that `eval`/`Use NNUE=false` are available"
        )
    return values, elapsed


def fit_linear(x: list[int], y: list[int]) -> tuple[float, float]:
    mean_x, mean_y = statistics.fmean(x), statistics.fmean(y)
    denominator = sum((value - mean_x) ** 2 for value in x)
    slope = (
        sum((a - mean_x) * (b - mean_y) for a, b in zip(x, y)) / denominator
        if denominator
        else 1.0
    )
    return slope, mean_y - slope * mean_x


def percentile(values: list[float], fraction: float) -> float:
    ordered = sorted(values)
    if not ordered:
        return 0.0
    index = min(len(ordered) - 1, round((len(ordered) - 1) * fraction))
    return ordered[index]


def rank(values: list[float]) -> list[float]:
    order = sorted(range(len(values)), key=values.__getitem__)
    result = [0.0] * len(values)
    start = 0
    while start < len(order):
        end = start + 1
        while end < len(order) and values[order[end]] == values[order[start]]:
            end += 1
        average = (start + end - 1) / 2
        for position in order[start:end]:
            result[position] = average
        start = end
    return result


def correlation(x: list[float], y: list[float]) -> float:
    mx, my = statistics.fmean(x), statistics.fmean(y)
    numerator = sum((a - mx) * (b - my) for a, b in zip(x, y))
    denominator = math.sqrt(
        sum((a - mx) ** 2 for a in x) * sum((b - my) ** 2 for b in y)
    )
    return numerator / denominator if denominator else 0.0


def bootstrap_mae_ci(
    calibrated: list[float], teacher: list[int], samples: int = 1000
) -> list[float]:
    rng = random.Random(0x4D414E474F)
    errors = [abs(value - target) for value, target in zip(calibrated, teacher)]
    if not errors:
        return [0.0, 0.0]
    estimates = [
        statistics.fmean(rng.choice(errors) for _ in errors)
        for _ in range(samples)
    ]
    return [
        round(percentile(estimates, 0.025), 3),
        round(percentile(estimates, 0.975), 3),
    ]


def metrics(values: list[int], teacher: list[int], a: float, b: float) -> dict:
    calibrated = [a * value + b for value in values]
    errors = [abs(value - target) for value, target in zip(calibrated, teacher)]
    return {
        "mae": round(statistics.fmean(errors), 3),
        "mae_ci95": bootstrap_mae_ci(calibrated, teacher),
        "rmse": round(math.sqrt(statistics.fmean(error * error for error in errors)), 3),
        "median": round(statistics.median(errors), 3),
        "p90": round(percentile(errors, 0.90), 3),
        "p99": round(percentile(errors, 0.99), 3),
        "pearson": round(correlation(calibrated, teacher), 5),
        "spearman": round(correlation(rank(calibrated), rank(teacher)), 5),
        "sign_error": round(
            sum((value > 0) != (target > 0) for value, target in zip(calibrated, teacher))
            / len(teacher),
            5,
        ),
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--base", type=Path, required=True)
    parser.add_argument("--candidate", type=Path, required=True)
    parser.add_argument("--stockfish", type=Path, required=True)
    parser.add_argument("--corpus", type=Path, default=DEFAULT_CORPUS)
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()
    rows = json.loads(args.corpus.read_text())
    base, base_ms = mango_evals(args.base, rows)
    candidate, candidate_ms = mango_evals(args.candidate, rows)
    candidate_repeat, _ = mango_evals(args.candidate, rows)
    teacher, sf_ms = stockfish_evals(args.stockfish, rows)
    if candidate != candidate_repeat:
        raise RuntimeError("candidate evaluation is not deterministic")
    # Split complete source games together to avoid train/test leakage.
    groups = [row.get("source", row["id"]) for row in rows]
    buckets = [
        int(hashlib.md5(group.encode()).hexdigest(), 16) % 5
        for group in groups
    ]
    train = [i for i, bucket in enumerate(buckets) if bucket >= 2]
    validation = [i for i, bucket in enumerate(buckets) if bucket == 1]
    test = [i for i, bucket in enumerate(buckets) if bucket == 0]
    a, b = fit_linear([base[i] for i in train], [teacher[i] for i in train])
    report = {
        "binaries": {
            "base": {"path": str(args.base), "sha256": sha256(args.base)},
            "candidate": {"path": str(args.candidate), "sha256": sha256(args.candidate)},
            "stockfish": {"path": str(args.stockfish), "sha256": sha256(args.stockfish)},
            "stockfish_commit": "6a8767a0d5d9502e6d4de1bef97468b5d6fab80a",
        },
        "calibration": {"slope": a, "intercept": b, "trained_on": len(train)},
        "train": {
            "base": metrics([base[i] for i in train], [teacher[i] for i in train], a, b),
            "candidate": metrics(
                [candidate[i] for i in train], [teacher[i] for i in train], a, b
            ),
        },
        "validation_set": {
            "base": metrics(
                [base[i] for i in validation], [teacher[i] for i in validation], a, b
            ),
            "candidate": metrics(
                [candidate[i] for i in validation],
                [teacher[i] for i in validation],
                a, b,
            ),
        },
        "validation": {
            "validation_positions": len(validation),
            "tested_on": len(test),
            "group_split": "source-or-id, md5 modulo 5",
            "candidate_deterministic": True,
        },
        "test": {
            "base": metrics([base[i] for i in test], [teacher[i] for i in test], a, b),
            "candidate": metrics(
                [candidate[i] for i in test], [teacher[i] for i in test], a, b
            ),
        },
        "elapsed_ms": {"base": base_ms, "candidate": candidate_ms, "stockfish": sf_ms},
        "families": {},
        "positions": [],
    }
    families: dict[str, list[int]] = defaultdict(list)
    for i in test:
        families[rows[i]["family"]].append(i)
        report["positions"].append(
            {
                "id": rows[i]["id"],
                "family": rows[i]["family"],
                "base": base[i],
                "candidate": candidate[i],
                "stockfish": teacher[i],
                "candidate_abs_error": abs(a * candidate[i] + b - teacher[i]),
            }
        )
    for family, indexes in families.items():
        report["families"][family] = {
            "base": metrics([base[i] for i in indexes], [teacher[i] for i in indexes], a, b),
            "candidate": metrics(
                [candidate[i] for i in indexes], [teacher[i] for i in indexes], a, b
            ),
        }
    report["positions"].sort(key=lambda row: row["candidate_abs_error"], reverse=True)
    encoded = json.dumps(report, indent=2, sort_keys=True)
    if args.output:
        args.output.write_text(encoded + "\n")
    print(encoded)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
