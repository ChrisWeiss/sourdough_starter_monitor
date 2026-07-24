#!/usr/bin/env bash
# Create/refresh the project virtualenv with PlatformIO + WireViz.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
VENV="$ROOT/.venv"
REQ="$ROOT/requirements.txt"

echo "==> Creating $VENV"
python3 -m venv "$VENV"
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
