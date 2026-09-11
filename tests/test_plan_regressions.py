"""Focused regression gates derived from the gradual evaluation plan."""

from __future__ import annotations

import os
from pathlib import Path
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]


class PlanRegressionTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.temp = tempfile.TemporaryDirectory()
        cls.audit = Path(cls.temp.name) / "plan-audit"
        subprocess.run(
            [
                "gcc",
                "-std=gnu11",
                "-O1",
                "-g",
                "-fsanitize=undefined",
                "-fno-omit-frame-pointer",
                "-I",
                str(ROOT),
                str(ROOT / "tests" / "plan_audit.c"),
                "-lm",
                "-o",
                str(cls.audit),
            ],
            cwd=ROOT,
            check=True,
            capture_output=True,
            text=True,
        )

    @classmethod
    def tearDownClass(cls) -> None:
        cls.temp.cleanup()

    def test_repetition_history_is_bounded_by_available_entries(self) -> None:
        environment = os.environ.copy()
        environment["UBSAN_OPTIONS"] = "halt_on_error=1"
        result = subprocess.run(
            [str(self.audit), "repetition"],
            cwd=ROOT,
            env=environment,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn("repetition_search=", result.stdout)

    def test_rule50_capture_quiet_move_and_undo(self) -> None:
        result = subprocess.run(
            [str(self.audit), "rule50"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn("capture=0 capture_undo=25", result.stdout)
        self.assertIn("quiet=301 quiet_undo=300", result.stdout)

    def test_loading_fen_resets_position_history(self) -> None:
        result = subprocess.run(
            [str(self.audit), "fen_reset"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn(
            "castled_white=0 castled_black=0 history=0", result.stdout
        )

    def test_null_move_clears_ep_and_restores_exact_state(self) -> None:
        result = subprocess.run(
            [str(self.audit), "null_state"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn(
            "during_ep_clear=1 during_clock=0 side_changed=1 restored=1",
            result.stdout,
        )

    def test_knnk_mate_is_not_declared_dead_material(self) -> None:
        result = subprocess.run(
            [str(self.audit), "material"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn("insufficient=0 status=2 expected=2", result.stdout)


if __name__ == "__main__":
    unittest.main()
