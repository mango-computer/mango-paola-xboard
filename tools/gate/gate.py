#!/usr/bin/env python3
"""Run one bounded Mango AC integration gate and persist its evidence."""

from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import json
import os
from pathlib import Path
import platform
import shlex
import subprocess
import sys
import time


ROOT = Path(__file__).resolve().parents[2]
REGISTRY_PATH = Path(__file__).with_name("registry.json")
ARTIFACTS = ROOT / "artifacts" / "gates"
PASSING = {"GREEN", "PROVISIONAL"}


def load_registry() -> dict:
    return json.loads(REGISTRY_PATH.read_text(encoding="utf-8"))


def save_registry(registry: dict) -> None:
    REGISTRY_PATH.write_text(
        json.dumps(registry, indent=2, sort_keys=False) + "\n", encoding="utf-8"
    )


def git(*args: str, check: bool = True) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        ["git", *args],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=check,
    )


def tracked_manifest() -> dict[str, str]:
    names = git("ls-files").stdout.splitlines()
    result = {}
    for name in names:
        path = ROOT / name
        if path.is_file():
            result[name] = hashlib.sha256(path.read_bytes()).hexdigest()
    return result


def blocked_dependencies(registry: dict, change_id: str) -> list[str]:
    changes = registry["changes"]
    return [
        dependency
        for dependency in changes[change_id]["requires"]
        if changes[dependency]["status"] not in PASSING
    ]


def block_descendants(registry: dict, failed_id: str) -> None:
    blocked = {failed_id}
    changed = True
    while changed:
        changed = False
        for change_id, item in registry["changes"].items():
            if item["status"] in {"GREEN", "PROVISIONAL", "RED"}:
                continue
            if any(dependency in blocked for dependency in item["requires"]):
                item["status"] = "BLOCKED"
                item["blocked_by"] = failed_id
                blocked.add(change_id)
                changed = True


def decide(
    command_failed: bool,
    timed_out: bool,
    base_metric: float | None,
    candidate_metric: float | None,
    correction: bool,
    registry: dict,
) -> tuple[str, float | None, str]:
    if timed_out:
        return "RED", None, "El gate excedió el límite global de tiempo."
    if command_failed:
        return "RED", None, "Falló una comprobación bloqueante."
    if base_metric is None or candidate_metric is None:
        return (
            "GREEN",
            None,
            "Todas las comprobaciones bloqueantes pasaron."
            if correction
            else "Gate funcional superado; no se declaró métrica comparativa.",
        )
    if base_metric == 0:
        return "RED", None, "La métrica base no puede ser cero."

    regression = (candidate_metric - base_metric) * 100.0 / abs(base_metric)
    neutral = float(registry["policy"]["neutral_percent"])
    strong = float(registry["policy"]["strong_regression_percent"])
    if regression > strong:
        return "RED", regression, f"Regresión fuerte de {regression:.3f} %."
    if regression > neutral:
        return (
            "PROVISIONAL",
            regression,
            f"Regresión menor de {regression:.3f} %; se evaluará con dependientes.",
        )
    if regression < -neutral:
        return "GREEN", regression, f"Mejora de {-regression:.3f} %."
    if correction:
        return (
            "GREEN",
            regression,
            f"Corrección válida dentro de la banda neutra ({regression:.3f} %).",
        )
    return (
        "INCONCLUSIVE",
        regression,
        f"Resultado dentro de la banda de ruido ({regression:.3f} %).",
    )


def run_command(command: str, timeout: float, log) -> tuple[int, bool, float]:
    started = time.monotonic()
    log.write(f"\n$ {command}\n")
    log.flush()
    try:
        completed = subprocess.run(
            ["bash", "-o", "pipefail", "-c", command],
            cwd=ROOT,
            stdout=log,
            stderr=subprocess.STDOUT,
            timeout=max(0.1, timeout),
            check=False,
        )
        return completed.returncode, False, time.monotonic() - started
    except subprocess.TimeoutExpired:
        log.write("\nTIMEOUT\n")
        return 124, True, time.monotonic() - started


def command_run(args: argparse.Namespace) -> int:
    registry = load_registry()
    if args.id not in registry["changes"]:
        raise SystemExit(f"ID no registrado: {args.id}")
    blocked = blocked_dependencies(registry, args.id)
    if blocked:
        item = registry["changes"][args.id]
        item["status"] = "BLOCKED"
        item["blocked_by"] = blocked
        save_registry(registry)
        print(f"{args.id}: BLOCKED por {', '.join(blocked)}")
        return 3

    timestamp = dt.datetime.now(dt.timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    run_dir = ARTIFACTS / args.id / timestamp
    run_dir.mkdir(parents=True, exist_ok=False)
    started = time.monotonic()
    timeout = float(registry["policy"]["timeout_seconds"])
    command_results = []
    failed = False
    timed_out = False

    with (run_dir / "commands.log").open("w", encoding="utf-8") as log:
        for command in args.command:
            remaining = timeout - (time.monotonic() - started)
            if remaining <= 0:
                timed_out = True
                failed = True
                break
            code, command_timeout, elapsed = run_command(command, remaining, log)
            command_results.append(
                {
                    "command": command,
                    "exit_code": code,
                    "elapsed_seconds": round(elapsed, 3),
                    "timed_out": command_timeout,
                }
            )
            if code != 0:
                failed = True
                timed_out = command_timeout
                break

    status, regression, reason = decide(
        failed,
        timed_out,
        args.base_metric,
        args.candidate_metric,
        args.correction,
        registry,
    )
    head = git("rev-parse", "HEAD").stdout.strip()
    summary = {
        "id": args.id,
        "objective": args.objective,
        "status": status,
        "reason": reason,
        "base": registry["current_base"],
        "commit": head,
        "metric": {
            "name": args.metric_name,
            "base": args.base_metric,
            "candidate": args.candidate_metric,
            "regression_percent": regression,
            "lower_is_better": True,
        },
        "duration_seconds": round(time.monotonic() - started, 3),
        "commands": command_results,
        "environment": {
            "python": sys.version.split()[0],
            "platform": platform.platform(),
            "machine": platform.machine(),
            "gcc": subprocess.run(
                ["gcc", "--version"],
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                check=False,
            ).stdout.splitlines()[0],
        },
    }
    (run_dir / "summary.json").write_text(
        json.dumps(summary, indent=2) + "\n", encoding="utf-8"
    )
    (run_dir / "manifest.json").write_text(
        json.dumps(tracked_manifest(), indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )

    item = registry["changes"][args.id]
    item.update(
        {
            "status": status,
            "last_commit": head,
            "last_evidence": str(run_dir.relative_to(ROOT)),
            "reason": reason,
        }
    )
    if status == "RED":
        block_descendants(registry, args.id)
    if status == "GREEN" and args.promote:
        base_number = int(registry["current_base"].split("-")[1]) + 1
        new_base = f"BASE-{base_number:02d}"
        git("tag", "-a", new_base, "-m", f"{args.id} accepted")
        registry["current_base"] = new_base
        item["promoted_base"] = new_base
    save_registry(registry)

    if status == "RED" and args.auto_revert:
        if head != registry["base_commit"]:
            result = git("revert", "--no-edit", head, check=False)
            summary["revert_output"] = result.stdout
            summary["revert_exit_code"] = result.returncode
            (run_dir / "summary.json").write_text(
                json.dumps(summary, indent=2) + "\n", encoding="utf-8"
            )

    print(f"{args.id}: {status} — {reason}")
    print(run_dir.relative_to(ROOT))
    return 0 if status in PASSING else 2


def command_status(_: argparse.Namespace) -> int:
    registry = load_registry()
    for change_id, item in registry["changes"].items():
        suffix = f" ({item.get('reason', '')})" if item.get("reason") else ""
        print(f"{change_id:6} {item['status']}{suffix}")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="subcommand", required=True)
    status_parser = subparsers.add_parser("status")
    status_parser.set_defaults(func=command_status)

    run_parser = subparsers.add_parser("run")
    run_parser.add_argument("--id", required=True)
    run_parser.add_argument("--objective", required=True)
    run_parser.add_argument("--command", action="append", default=[])
    run_parser.add_argument("--metric-name", default="elapsed")
    run_parser.add_argument("--base-metric", type=float)
    run_parser.add_argument("--candidate-metric", type=float)
    run_parser.add_argument("--correction", action="store_true")
    run_parser.add_argument("--promote", action="store_true")
    run_parser.add_argument("--auto-revert", action="store_true")
    run_parser.set_defaults(func=command_run)
    return parser


def main() -> int:
    args = build_parser().parse_args()
    return args.func(args)


if __name__ == "__main__":
    raise SystemExit(main())
