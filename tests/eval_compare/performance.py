#!/usr/bin/env python3
"""Median-based fixed-depth speed comparison with optional CPU affinity."""

from __future__ import annotations

import argparse
import json
import os
import re
import statistics
import subprocess
import time
from pathlib import Path

from compare import mango_evals

FENS = [
    "r2q1rk1/pp2bppp/2n1pn2/2pp4/3P4/2PBPN2/PPQ2PPP/R1B1K2R w KQ - 4 10",
    "2rq1rk1/pp1nbppp/2p1pn2/3p4/3P4/2NBPN2/PPQ2PPP/2RR2K1 w - - 7 14",
    "8/5pk1/3p2p1/1p1Pp2p/1P2P2P/2P2PP1/5K2/8 w - - 0 35",
    "8/4rpk1/3p2p1/7p/3P3P/5PP1/4RK2/8 b - - 0 42",
]
NODES_RE = re.compile(r"\bnodes\s+(\d+)")
NPS_RE = re.compile(r"\bnps\s+(\d+)")


def sample(engine: Path, depth: int, cpu: int | None) -> dict:
    command = [str(engine)]
    if cpu is not None and os.name == "posix":
        command = ["taskset", "-c", str(cpu), *command]
    stdin = "uci\nisready\n" + "".join(
        f"position fen {fen}\ngo depth {depth}\n" for fen in FENS
    ) + "quit\n"
    start = time.perf_counter_ns()
    result = subprocess.run(
        command, input=stdin, capture_output=True, text=True, timeout=120, check=True
    )
    elapsed = (time.perf_counter_ns() - start) / 1_000_000
    nodes = [int(value) for value in NODES_RE.findall(result.stdout)]
    nps = [int(value) for value in NPS_RE.findall(result.stdout)]
    return {
        "elapsed_ms": elapsed,
        "nodes": sum(nodes[-len(FENS):]),
        "nps": statistics.median(nps[-len(FENS):]) if nps else 0,
    }


def summarize(samples: list[dict]) -> dict:
    return {
        "elapsed_median_ms": round(statistics.median(s["elapsed_ms"] for s in samples), 3),
        "nodes_median": round(statistics.median(s["nodes"] for s in samples)),
        "nps_median": round(statistics.median(s["nps"] for s in samples)),
        "samples": samples,
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--base", type=Path, required=True)
    parser.add_argument("--candidate", type=Path, required=True)
    parser.add_argument("--depth", type=int, default=7)
    parser.add_argument("--repetitions", type=int, default=7)
    parser.add_argument("--cpu", type=int)
    parser.add_argument(
        "--corpus",
        type=Path,
        default=Path(__file__).resolve().parents[1] / "eval_corpus.json",
    )
    args = parser.parse_args()
    sample(args.base, args.depth, args.cpu)
    sample(args.candidate, args.depth, args.cpu)
    search_samples = {"base": [], "candidate": []}
    for repetition in range(args.repetitions):
        order = (
            (("base", args.base), ("candidate", args.candidate))
            if repetition % 2 == 0
            else (("candidate", args.candidate), ("base", args.base))
        )
        for name, engine in order:
            search_samples[name].append(sample(engine, args.depth, args.cpu))
    base = summarize(search_samples["base"])
    candidate = summarize(search_samples["candidate"])
    corpus = json.loads(args.corpus.read_text())
    base_eval, candidate_eval = [], []
    for repetition in range(args.repetitions):
        order = (
            (("base", args.base), ("candidate", args.candidate))
            if repetition % 2 == 0
            else (("candidate", args.candidate), ("base", args.base))
        )
        for name, engine in order:
            elapsed = mango_evals(engine.resolve(), corpus)[1]
            (base_eval if name == "base" else candidate_eval).append(elapsed)
    speed_ratio = base["elapsed_median_ms"] / candidate["elapsed_median_ms"]
    nodes_change = (
        100 * (candidate["nodes_median"] - base["nodes_median"]) /
        max(1, base["nodes_median"])
    )
    nodes_comparable = abs(nodes_change) <= 10
    report = {
        "configuration": {
            "depth": args.depth,
            "repetitions": args.repetitions,
            "cpu": args.cpu,
        },
        "base": base,
        "candidate": candidate,
        "candidate_speed_ratio": round(speed_ratio, 5),
        "nps_regression_percent": round(
            100 * (base["nps_median"] - candidate["nps_median"]) /
            max(1, base["nps_median"]), 3
        ),
        "nodes_change_percent": round(nodes_change, 3),
        "nps_comparable": nodes_comparable,
        "passes_nps_gate": (
            candidate["elapsed_median_ms"] <= base["elapsed_median_ms"] * 1.02 and
            (
                not nodes_comparable or
                candidate["nps_median"] >= base["nps_median"] * 0.98
            )
        ),
        "eval_batch": {
            "base_median_ms": round(statistics.median(base_eval), 3),
            "candidate_median_ms": round(statistics.median(candidate_eval), 3),
            "candidate_ratio": round(
                statistics.median(base_eval) / statistics.median(candidate_eval), 5
            ),
        },
    }
    report["passes_eval_gate"] = (
        report["eval_batch"]["candidate_median_ms"]
        <= report["eval_batch"]["base_median_ms"] * 1.01
    )
    print(json.dumps(report, indent=2, sort_keys=True))
    return 0 if report["passes_nps_gate"] and report["passes_eval_gate"] else 2


if __name__ == "__main__":
    raise SystemExit(main())
