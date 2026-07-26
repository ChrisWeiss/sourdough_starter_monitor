#!/usr/bin/env bash
# Create/refresh the project virtualenv with PlatformIO + WireViz.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
VENV="$ROOT/.venv"
REQ="$ROOT/requirements.txt"

# PlatformIO 6.1+ needs Python 3.10+ (library resolver). Prefer Homebrew 3.11+.
PY=""
for candidate in python3.14 python3.13 python3.12 python3.11 python3.10 python3; do
  if command -v "$candidate" >/dev/null 2>&1; then
    ver="$("$candidate" -c 'import sys; print("%d.%d" % sys.version_info[:2])')"
    major="${ver%%.*}"
    minor="${ver#*.}"
    if [[ "$major" -gt 3 || ( "$major" -eq 3 && "$minor" -ge 10 ) ]]; then
      PY="$candidate"
      break
    fi
  fi
done
if [[ -z "$PY" ]]; then
  echo "Need Python 3.10+ (PlatformIO). Install e.g. brew install python@3.11" >&2
  exit 1
fi

echo "==> Creating $VENV with $PY ($("$PY" --version))"
"$PY" -m venv "$VENV"
# shellcheck disable=SC1091
source "$VENV/bin/activate"
pip install --upgrade pip
pip install -r "$REQ"

echo ""
echo "Activate with:"
echo "  source .venv/bin/activate"
echo ""
echo "Then:"
echo "  cd firmware && pio run -e lolin-esp32"
echo "  ./scripts/bringup_esp32.sh"
echo "  ./scripts/render_wiring.sh"
