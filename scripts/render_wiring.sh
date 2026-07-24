#!/usr/bin/env bash
# Render WireViz assembly diagrams under docs/wiring/ → docs/wiring/generated/
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC_DIR="$ROOT/docs/wiring"
OUT_DIR="$SRC_DIR/generated"
REQ="$ROOT/requirements.txt"
VENV="$ROOT/.venv"

if ! command -v dot >/dev/null 2>&1; then
  echo "GraphViz 'dot' not found. Install with: brew install graphviz" >&2
  exit 1
fi

ensure_venv() {
  if [[ ! -x "$VENV/bin/wireviz" ]] || [[ ! -x "$VENV/bin/pio" ]]; then
    echo "==> Creating .venv and installing requirements.txt (PlatformIO + WireViz)"
    python3 -m venv "$VENV"
    # shellcheck disable=SC1091
    source "$VENV/bin/activate"
    pip install --upgrade pip
    pip install -r "$REQ"
  fi
}

ensure_venv
WIREVIZ="$VENV/bin/wireviz"

mkdir -p "$OUT_DIR"

LAYOUT="$SRC_DIR/_layout.yml"
shopt -s nullglob
files=("$SRC_DIR"/*.yml)
if [[ ${#files[@]} -eq 0 ]]; then
  echo "No docs/wiring/*.yml files found" >&2
  exit 1
fi

for yml in "${files[@]}"; do
  name="$(basename "$yml" .yml)"
  # Shared layout prepend; skip the layout file itself
  [[ "$name" == _* ]] && continue
  echo "==> Rendering $name"
  "$WIREVIZ" -f s -p "$LAYOUT" -o "$OUT_DIR" -O "$name" "$yml"
done

ls -la "$OUT_DIR"/*.svg
echo "Done. Embed paths: docs/wiring/generated/<name>.svg"
