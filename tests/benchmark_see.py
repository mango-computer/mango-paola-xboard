#!/usr/bin/env python3
"""Microbenchmark for full and thresholded static exchange evaluation."""

from __future__ import annotations

import argparse
import json
import statistics
import subprocess
import tempfile
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FIXTURES = json.loads((ROOT / "tests/fixtures/eet_cases.json").read_text())
CORPUS = json.loads((ROOT / "tests/eval_corpus.json").read_text())


def compile_engine(output: Path, apply_undo: bool) -> None:
    command = ["gcc", "-O3", "-DNDEBUG", "-DPRUEBAS_HCE"]
    if apply_undo:
        command.append("-DEET_APLICAR_DESHACER")
    command += [str(ROOT / "bitmma3.c"), "-lm", "-o", str(output)]
    subprocess.run(command, cwd=ROOT, check=True, capture_output=True, text=True)


def sample(
    engine: Path,
    fens: list[str],
    repetitions: int,
    threshold: int,
    threshold_mode: bool,
    temp: Path,
) -> dict[str, float | int]:
    commands: list[str] = []
    for index, fen in enumerate(fens):
        epd = temp / f"see-{index}.epd"
        epd.write_text(f'[White "SEE"]\n[Black "SEE"]\n[FEN "{fen}"]\n')
        commands += [
            f"readfen {epd} 1",
            f"eetbench {repetitions} {threshold} {int(threshold_mode)}",
        ]
    commands.append("quit")
    start = time.perf_counter_ns()
    result = subprocess.run(
        [str(engine)],
        cwd=ROOT,
        input="\n".join(commands) + "\n",
        capture_output=True,
        text=True,
        timeout=120,
        check=True,
    )
    elapsed_ms = (time.perf_counter_ns() - start) / 1_000_000
    rows = []
    for line in result.stdout.splitlines():
        if "SEEBench:" not in line:
            continue
        fields = line.split("SEEBench:", 1)[1].split()
        rows.append(
            {
                key: int(value)
                for key, value in (
                    field.split("=", 1) for field in fields if "=" in field
                )
            }
        )
    if len(rows) != len(fens):
        raise RuntimeError(result.stdout)
    return {
        "elapsed_ms": elapsed_ms,
        "checksum": sum(row["checksum"] for row in rows),
        "calls": sum(row["full_calls"] + row["threshold_calls"] for row in rows),
        "branches": sum(
            row["full_branches"] + row["threshold_branches"] for row in rows
        ),
        "cuts": sum(row["threshold_cuts"] for row in rows),
    }


def profile_search(engine: Path, fens: list[str], temp: Path, depth: int) -> dict[str, int]:
    commands: list[str] = []
    for index, fen in enumerate(fens):
        epd = temp / f"profile-{index}.epd"
        epd.write_text(f'[White "SEE"]\n[Black "SEE"]\n[FEN "{fen}"]\n')
        commands += [
            f"readfen {epd} 1",
            "eetstatsreset",
            f"searchprobe {depth} -100000 100000",
            "eetstats",
        ]
    commands.append("quit")
    result = subprocess.run(
        [str(engine)],
        cwd=ROOT,
        input="\n".join(commands) + "\n",
        capture_output=True,
        text=True,
        timeout=120,
        check=True,
    )
    totals = {"threshold_calls": 0, "eetpos_calls": 0}
    for line in result.stdout.splitlines():
        if "SEEStats:" not in line:
            continue
        fields = line.split("SEEStats:", 1)[1].split()
        row = {
            key: int(value)
            for key, value in (
                field.split("=", 1) for field in fields if "=" in field
            )
        }
        for key in totals:
            totals[key] += row[key]
    return totals


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--rounds", type=int, default=7)
    parser.add_argument("--inner-repetitions", type=int, default=3000)
    parser.add_argument("--threshold", type=int, default=-15)
    args = parser.parse_args()

    fixture_fens = [row["fen"] for row in FIXTURES]
    corpus_fens = [row["fen"] for row in CORPUS if row["family"] != "opening"][:64]
    fens = list(dict.fromkeys(fixture_fens + corpus_fens))

    with tempfile.TemporaryDirectory() as directory:
        temp = Path(directory)
        copied = temp / "mangoac-see-copy"
        delta = temp / "mangoac-see-delta"
        compile_engine(copied, apply_undo=False)
        compile_engine(delta, apply_undo=True)
        variants = {
            "full-copy": (copied, False),
            "threshold-copy": (copied, True),
            "threshold-delta": (delta, True),
        }
        samples: dict[str, list[dict[str, float | int]]] = {
            name: [] for name in variants
        }
        for round_index in range(args.rounds):
            names = list(variants)
            if round_index % 2:
                names.reverse()
            for name in names:
                engine, threshold_mode = variants[name]
                samples[name].append(
                    sample(
                        engine,
                        fens,
                        args.inner_repetitions,
                        args.threshold,
                        threshold_mode,
                        temp,
                    )
                )

        search_profile = profile_search(copied, fens[:24], temp, depth=3)

    checksums = {
        name: {int(row["checksum"]) for row in rows}
        for name, rows in samples.items()
    }
    if len({next(iter(values)) for values in checksums.values()}) != 1:
        raise RuntimeError(f"non-equivalent benchmark results: {checksums}")

    baseline = statistics.median(
        float(row["elapsed_ms"]) for row in samples["full-copy"]
    )
    for name, rows in samples.items():
        elapsed = statistics.median(float(row["elapsed_ms"]) for row in rows)
        branches = statistics.median(int(row["branches"]) for row in rows)
        cuts = statistics.median(int(row["cuts"]) for row in rows)
        print(
            f"{name}: median_ms={elapsed:.3f} relative={elapsed / baseline:.3f} "
            f"branches={branches:.0f} cuts={cuts:.0f}"
        )
    print(
        "search-profile: "
        f"threshold_calls={search_profile['threshold_calls']} "
        f"eetpos_calls={search_profile['eetpos_calls']}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
