#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT"

(cd polyglotbooks && sha256sum -c mangoac-book.bin.sha256)
python3 -m unittest discover -s tests -p 'test_*.py' -v
