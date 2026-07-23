#!/usr/bin/env bash
# Render WireViz assembly diagrams under docs/wiring/ → docs/wiring/generated/
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC_DIR="$ROOT/docs/wiring"
OUT_DIR="$SRC_DIR/generated"

WIREVIZ=""
for candidate in \
  wireviz \
  "$HOME/Library/Python/3.9/bin/wireviz" \
  "$HOME/Library/Python/3.10/bin/wireviz" \
  "$HOME/.local/bin/wireviz"; do
  if command -v "$candidate" >/dev/null 2>&1 || [[ -x "$candidate" ]]; then
    WIREVIZ="$candidate"
    break
  fi
done

if [[ -z "$WIREVIZ" ]]; then
  echo "wireviz not found. Install GraphViz and WireViz:" >&2
  echo "  brew install graphviz" >&2
  echo "  python3 -m pip install --user wireviz" >&2
  exit 1
fi

if ! command -v dot >/dev/null 2>&1; then
  echo "GraphViz 'dot' not found. Install with: brew install graphviz" >&2
  exit 1
fi

mkdir -p "$OUT_DIR"

shopt -s nullglob
files=("$SRC_DIR"/*.yml)
if [[ ${#files[@]} -eq 0 ]]; then
  echo "No docs/wiring/*.yml files found" >&2
  exit 1
fi

for yml in "${files[@]}"; do
  name="$(basename "$yml" .yml)"
  echo "==> Rendering $name"
  "$WIREVIZ" -f s -o "$OUT_DIR" -O "$name" "$yml"
done

ls -la "$OUT_DIR"/*.svg
echo "Done. Embed paths: docs/wiring/generated/<name>.svg"
