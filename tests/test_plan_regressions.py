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
                "-DPRUEBAS_HCE",
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

    def test_eval_result_class_distinguishes_full_lazy_and_cache(self) -> None:
        result = subprocess.run(
            [str(self.audit), "eval_result_class"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn("full=0 cache=2 lazy=1", result.stdout)

    def test_compiler_bit_intrinsics_match_reference(self) -> None:
        result = subprocess.run(
            [str(self.audit), "bitops"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn("bitops_samples=100000", result.stdout)

    def test_full_evaluation_clears_all_pin_rays(self) -> None:
        result = subprocess.run(
            [str(self.audit), "pin_clear"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn("dirty_pin_rays=0", result.stdout)

    def test_bitboard_printer_does_not_index_past_board(self) -> None:
        result = subprocess.run(
            [str(self.audit), "print_bits"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn("printed_bitboard=1", result.stdout)

    def test_tt_exact_hit_is_reused_at_equal_depth(self) -> None:
        result = subprocess.run(
            [str(self.audit), "tt_equal_depth"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn(
            "score=123 flag=4 expected_flag=4 static_eval=17",
            result.stdout,
        )

    def test_tt_rejects_rule50_and_eval_history_mismatch(self) -> None:
        result = subprocess.run(
            [str(self.audit), "tt_context"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertRegex(
            result.stdout, r"clock_miss=1 history_miss=1 size=(24|32)"
        )

    def test_tt_mate_score_normalization_is_symmetric(self) -> None:
        result = subprocess.run(
            [str(self.audit), "tt_mate"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn(
            "positive_delta=4 negative_delta=-4 ordinary=123",
            result.stdout,
        )

    def test_incremental_eval_state_matches_reconstruction_and_undo(self) -> None:
        result = subprocess.run(
            [str(self.audit), "eval_state"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        match = re.search(r"eval_state_transitions=(\d+)", result.stdout)
        self.assertIsNotNone(match, result.stdout)
        self.assertGreater(int(match.group(1)), 50)

    def test_pawn_hash_restores_reusable_king_shelter(self) -> None:
        result = subprocess.run(
            [str(self.audit), "pawn_shelter"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertRegex(result.stdout, r"shelter_restored=1 pawn_hits=[1-9]\d*")

    def test_material_cache_reuses_complete_pure_result(self) -> None:
        result = subprocess.run(
            [str(self.audit), "material_cache"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertRegex(result.stdout, r"same_score=1 material_hits=[1-9]\d*")

    def test_qsearch_does_not_evaluate_while_in_check(self) -> None:
        result = subprocess.run(
            [str(self.audit), "qsearch_check"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        match = re.search(
            r"evals_in_check=(\d+) evals=(\d+) score=(-?\d+)",
            result.stdout,
        )
        self.assertIsNotNone(match, result.stdout)
        self.assertEqual(match.group(1), "0")
        self.assertGreater(int(match.group(2)), 0)

    def test_tt_layout_and_epoch_invalidation(self) -> None:
        result = subprocess.run(
            [str(self.audit), "tt_epoch"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=30,
            check=True,
        )
        self.assertIn("epoch_miss=1 static_unknown=1 size=32", result.stdout)


if __name__ == "__main__":
    unittest.main()
