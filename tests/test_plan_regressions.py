"""Focused regression gates derived from the gradual evaluation plan."""

from __future__ import annotations

import os
from pathlib import Path
import re
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

    def test_slider_raw_attacks_include_friendly_defenders(self) -> None:
        result = subprocess.run(
            [str(self.audit), "bishop_defense"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn("raw=1 map=1 threat_black=0", result.stdout)

    def test_king_attacks_defend_friendly_pieces(self) -> None:
        result = subprocess.run(
            [str(self.audit), "king_defense"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn("raw=1 map=1 threat_black=0", result.stdout)

    def test_mobility_excludes_friendly_blockers(self) -> None:
        result = subprocess.run(
            [str(self.audit), "mobility"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn("own_blocker_raw=1 own_blocker_useful=0", result.stdout)

    def test_passed_pawn_path_includes_promotion_square(self) -> None:
        result = subprocess.run(
            [str(self.audit), "promotion_path"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn("promotion_delta_mg=3 promotion_delta_eg=8", result.stdout)

    def test_general_pawnless_positions_use_full_evaluation(self) -> None:
        result = subprocess.run(
            [str(self.audit), "pawnless"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        match = re.search(
            r"queen_h1=(-?\d+) queen_h4=(-?\d+) attacks=(\d+)",
            result.stdout,
        )
        self.assertIsNotNone(match, result.stdout)
        self.assertNotEqual(match.group(1), match.group(2))
        self.assertNotEqual(match.group(3), "0")

    def test_lazy_evaluation_is_disabled_near_rule50_draw(self) -> None:
        result = subprocess.run(
            [str(self.audit), "lazy_rule50"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertRegex(result.stdout, r"full=-?\d+ narrow=-?\d+ equal=1")

    def test_eval_cache_contract_is_score_only(self) -> None:
        result = subprocess.run(
            [str(self.audit), "eval_cache"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn("same_score=1 cache_is_score_only=1", result.stdout)


if __name__ == "__main__":
    unittest.main()
