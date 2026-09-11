#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
cd "$ROOT"

OUT=${OUT:-"$ROOT/artifacts/match"}
BASE_SRC="$OUT/src-base"
JOBS=${JOBS:-8}

mkdir -p "$OUT" "$BASE_SRC"

git archive --format=tar 43ed23bfd6aa5df1e5d14a406d71a9fc35693859 | tar -C "$BASE_SRC" -xf -

gcc -std=gnu11 -O2 -DNDEBUG -Wall "$BASE_SRC/bitmma3.c" -lm -o "$OUT/mangoac_base"
gcc -std=gnu11 -O2 -DNDEBUG -Wall "$ROOT/bitmma3.c" -lm -pthread -o "$OUT/mangoac_candidato"

printf 'UsarLibroAperturas 0\nHashMB 64\n' > "$OUT/mangoac.ini"

echo "Binarios listos:"
sha256sum "$OUT/mangoac_base" "$OUT/mangoac_candidato"

echo "=== Rendimiento profundidad 7 ==="
python3 "$ROOT/tests/eval_compare/performance.py" \
  --base "$OUT/mangoac_base" \
  --candidate "$OUT/mangoac_candidato" \
  --depth 7 \
  --repetitions 7 \
  > "$OUT/performance-base00-vs-candidato.json" || true

echo "=== Match legal 1000 partidas, profundidad 8 ==="
python3 "$ROOT/tests/eval_compare/match_legal.py" \
  --base "$OUT/mangoac_base" \
  --candidate "$OUT/mangoac_candidato" \
  --games 1000 \
  --concurrency "$JOBS" \
  --depth 8 \
  --max-plies 120 \
  --hash-mb 64 \
  --move-timeout 90 \
  --cwd "$OUT" \
  --pgn "$OUT/match-depth8.pgn" \
  --output "$OUT/match-depth8.json"

echo "=== Match legal 400 partidas, 400 ms/jugada ==="
python3 "$ROOT/tests/eval_compare/match_legal.py" \
  --base "$OUT/mangoac_base" \
  --candidate "$OUT/mangoac_candidato" \
  --games 400 \
  --concurrency "$JOBS" \
  --movetime 400 \
  --max-plies 120 \
  --hash-mb 64 \
  --move-timeout 20 \
  --cwd "$OUT" \
  --pgn "$OUT/match-movetime400.pgn" \
  --output "$OUT/match-movetime400.json"

python3 - <<'PY'
import json
from pathlib import Path
out = Path("artifacts/match")
report = {}
for key, name in (
    ("performance", "performance-base00-vs-candidato.json"),
    ("match_depth8", "match-depth8.json"),
    ("match_movetime400", "match-movetime400.json"),
):
    path = out / name
    report[key] = json.loads(path.read_text()) if path.exists() and path.stat().st_size else None
(out / "veredicto.json").write_text(json.dumps(report, indent=2) + "\n")
summary = {
    "nps_ratio_candidate_over_base": (report["performance"] or {}).get("candidate_speed_ratio"),
    "nodes_change_percent": (report["performance"] or {}).get("nodes_change_percent"),
}
if report["match_depth8"]:
    summary.update({
        "depth8_score": report["match_depth8"]["candidate_score"],
        "depth8_elo": report["match_depth8"]["elo_difference"],
        "depth8_ci": report["match_depth8"]["elo_95ci"],
        "depth8_illegal_or_crash": report["match_depth8"]["illegal_or_crash"],
    })
if report["match_movetime400"]:
    summary.update({
        "time_score": report["match_movetime400"]["candidate_score"],
        "time_elo": report["match_movetime400"]["elo_difference"],
        "time_ci": report["match_movetime400"]["elo_95ci"],
        "time_illegal_or_crash": report["match_movetime400"]["illegal_or_crash"],
    })
print(json.dumps(summary, indent=2))
PY
