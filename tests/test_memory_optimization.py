"""Pruebas de memoria para la configuración de torneo."""

from __future__ import annotations

import struct
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BOOK = ROOT / "polyglotbooks" / "mangoac-book.bin"
PROJECTED_BOOK_BYTES = 58_880 * 200


class MemoryOptimizationTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.temp_dir = tempfile.TemporaryDirectory()
        cls.engine = Path(cls.temp_dir.name) / "mangoac"
        cls.harness = Path(cls.temp_dir.name) / "polyglot-harness"
        subprocess.run(
            [
                "gcc",
                "-lm",
                str(ROOT / "bitmma3.c"),
                "-Wall",
                "-O2",
                "-o",
                str(cls.engine),
            ],
            cwd=ROOT,
            check=True,
            capture_output=True,
            text=True,
        )
        subprocess.run(
            [
                "gcc",
                "-Wall",
                "-Wextra",
                "-O2",
                str(ROOT / "tests" / "polyglot_harness.c"),
                "-o",
                str(cls.harness),
            ],
            cwd=ROOT,
            check=True,
            capture_output=True,
            text=True,
        )

    @classmethod
    def tearDownClass(cls) -> None:
        cls.temp_dir.cleanup()

    def run_engine(
        self, commands: str, cwd: Path = ROOT
    ) -> subprocess.CompletedProcess:
        return subprocess.run(
            [str(self.engine)],
            cwd=cwd,
            input=commands,
            capture_output=True,
            text=True,
            timeout=15,
            check=True,
        )

    def run_stats(
        self, book: Path, max_mb: int, *, check: bool = True
    ) -> subprocess.CompletedProcess:
        return subprocess.run(
            [str(self.harness), "stats", str(book), str(max_mb)],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=10,
            check=check,
        )

    def test_uci_advertises_modern_hash_options(self) -> None:
        result = self.run_engine("uci\nquit\n")
        self.assertIn(
            "option name Hash type spin default 256 min 1 max 8192",
            result.stdout,
        )
        self.assertIn("option name Clear Hash type button", result.stdout)

    def test_uci_resizes_hash_before_ready(self) -> None:
        result = self.run_engine(
            "uci\n"
            "setoption name Hash value 64\n"
            "isready\n"
            "setoption name Clear Hash\n"
            "ucinewgame\n"
            "position startpos\n"
            "go depth 1\n"
            "quit\n"
        )
        self.assertIn(
            "info string Hash solicitado 64 MB, reservado 64 MB",
            result.stdout,
        )
        self.assertIn("readyok", result.stdout)
        self.assertIn("bestmove ", result.stdout)

    def test_legacy_hash_level_remains_a_fallback(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            cwd = Path(directory)
            (cwd / "mangoac.ini").write_text(
                "UsarTablaHash 1\n"
                "TamanioTablaHash 1\n"
                "UsarLibroAperturas 0\n",
                encoding="utf-8",
            )
            result = self.run_engine("quit\n", cwd)
        self.assertIn(
            "Hash movimientos      4 MB reales (6 MB solicitados)",
            result.stdout,
        )

    def test_hash_clear_uses_generations_not_full_memset_per_game(self) -> None:
        source = (ROOT / "ini.c").read_text(encoding="utf-8")
        self.assertIn("generacionHash++", source)
        self.assertEqual(source.count("memset(tabla_hash"), 1)
        self.assertIn("ptabla->generacion == generacionHash", (
            ROOT / "hash.c"
        ).read_text(encoding="utf-8"))

    def test_transposition_depth_preserves_tablebase_sentinel(self) -> None:
        types = (ROOT / "tipoDatos.h").read_text(encoding="utf-8")
        search = (ROOT / "busquedad.c").read_text(encoding="utf-8")
        self.assertIn("uint16\t\tprofundidad;", types)
        self.assertIn(
            "agregarMovTablaHash(256, capa, alfa, BANDERA_HASH_EXACTO, 0)",
            search,
        )

    def test_current_book_is_loaded_completely_in_ram(self) -> None:
        result = self.run_stats(BOOK, 64)
        self.assertEqual(result.stdout.strip(), "3680 58880")
        source = (ROOT / "libro3.c").read_text(encoding="utf-8")
        self.assertEqual(source.count("fread("), 1)
        self.assertIn("unsigned char *data", source)

    def test_engine_book_path_hashes_the_internal_state_directly(self) -> None:
        search = (ROOT / "busquedad.c").read_text(encoding="utf-8")
        self.assertIn("buscarMovimientoLibro3DesdeEstado", search)
        self.assertNotIn("converTabler2FEN", search)

    def test_projected_200x_book_fits_the_ram_loader(self) -> None:
        with tempfile.NamedTemporaryFile() as synthetic:
            synthetic.truncate(PROJECTED_BOOK_BYTES)
            synthetic.flush()
            result = self.run_stats(Path(synthetic.name), 64)
        self.assertEqual(
            result.stdout.strip(),
            f"{PROJECTED_BOOK_BYTES // 16} {PROJECTED_BOOK_BYTES}",
        )

    def test_book_over_configured_limit_is_rejected(self) -> None:
        with tempfile.NamedTemporaryFile() as oversized:
            oversized.truncate(1024 * 1024 + 16)
            oversized.flush()
            result = self.run_stats(Path(oversized.name), 1, check=False)
        self.assertNotEqual(result.returncode, 0)

    def test_unsorted_polyglot_book_is_rejected(self) -> None:
        with tempfile.NamedTemporaryFile() as unsorted:
            unsorted.write(struct.pack(">QHHI", 2, 1, 1, 0))
            unsorted.write(struct.pack(">QHHI", 1, 1, 1, 0))
            unsorted.flush()
            result = self.run_stats(Path(unsorted.name), 1, check=False)
        self.assertNotEqual(result.returncode, 0)


if __name__ == "__main__":
    unittest.main()
