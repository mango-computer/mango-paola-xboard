"""Benchmark manual y no bloqueante de las rutas de memoria."""

from __future__ import annotations

import json
import re
import statistics
import subprocess
import tempfile
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BOOK = ROOT / "polyglotbooks" / "mangoac-book.bin"
PROJECTED_BOOK_BYTES = 58_880 * 200
BENCH_FEN = "r2q1rk1/pp2bppp/2n1pn2/2pp4/3P4/2PBPN2/PPQ2PPP/R1B1K2R w KQ - 4 10"
CORPUS = ROOT / "tests" / "eval_corpus.json"
EVAL_RE = re.compile(r"Eval:\s*(-?\d+)")


def elapsed_ms(command: list[str], *, stdin: str | None = None) -> float:
    start = time.perf_counter()
    subprocess.run(
        command,
        cwd=ROOT,
        input=stdin,
        capture_output=True,
        text=True,
        timeout=30,
        check=True,
    )
    return round((time.perf_counter() - start) * 1000, 3)


def eval_benchmark(engine: Path, corpus: list[dict], repetitions: int = 7) -> dict:
    with tempfile.NamedTemporaryFile("w", suffix=".epd") as stream:
        for row in corpus:
            stream.write('[White "bench"]\n[Black "bench"]\n')
            stream.write(f'[FEN "{row["fen"]}"]\n\n')
        stream.flush()
        cold, hot, batch, checksums = [], [], [], []
        for _ in range(repetitions):
            commands = ""
            for index in range(1, len(corpus) + 1):
                commands += f"readfen {stream.name} {index}\neval\neval\n"
            commands += "quit\n"
            start = time.perf_counter_ns()
            completed = subprocess.run(
                [str(engine)],
                cwd=ROOT,
                input=commands,
                capture_output=True,
                text=True,
                timeout=30,
                check=True,
            )
            batch.append((time.perf_counter_ns() - start) / 1_000_000)
            values = [int(value) for value in EVAL_RE.findall(completed.stdout)]
            if len(values) != len(corpus) * 2:
                raise RuntimeError("the engine did not return every benchmark evaluation")
            cold.append(sum(abs(values[i]) for i in range(0, len(values), 2)))
            hot.append(sum(abs(values[i]) for i in range(1, len(values), 2)))
            checksums.append(sum((i + 1) * value for i, value in enumerate(values)))
    if len(set(checksums)) != 1:
        raise RuntimeError("non-deterministic evaluation checksum")
    return {
        "eval_batch_median_ms": round(statistics.median(batch), 3),
        "eval_batch_min_ms": round(min(batch), 3),
        "eval_cold_checksum": cold[0],
        "eval_hot_checksum": hot[0],
        "eval_ordered_checksum": checksums[0],
        "eval_positions": len(corpus),
        "eval_repetitions": repetitions,
    }


def main() -> int:
    with tempfile.TemporaryDirectory() as directory:
        temp = Path(directory)
        engine = temp / "mangoac"
        harness = temp / "polyglot-harness"
        projected = temp / "projected-book.bin"
        with projected.open("wb") as projected_file:
            projected_file.truncate(PROJECTED_BOOK_BYTES)

        subprocess.run(
            ["gcc", "-O2", "-Wall", str(ROOT / "bitmma3.c"), "-lm", "-o", engine],
            cwd=ROOT,
            check=True,
            capture_output=True,
            text=True,
        )
        subprocess.run(
            [
                "gcc",
                "-O2",
                "-Wall",
                "-Wextra",
                str(ROOT / "tests" / "polyglot_harness.c"),
                "-o",
                harness,
            ],
            cwd=ROOT,
            check=True,
            capture_output=True,
            text=True,
        )

        report = {
            "book_current_load_ms": elapsed_ms(
                [str(harness), "stats", str(BOOK), "64"]
            ),
            "book_projected_200x_load_ms": elapsed_ms(
                [str(harness), "stats", str(projected), "64"]
            ),
        }
        report.update(eval_benchmark(engine, json.loads(CORPUS.read_text())))
        for hash_mb in (64, 256):
            commands = (
                "uci\n"
                f"setoption name Hash value {hash_mb}\n"
                "isready\n"
                f"position fen {BENCH_FEN}\n"
                "go depth 6\n"
                "quit\n"
            )
            report[f"search_depth_6_hash_{hash_mb}_ms"] = elapsed_ms(
                [str(engine)],
                stdin=commands,
            )

    print(json.dumps(report, indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
