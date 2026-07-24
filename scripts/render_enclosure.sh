#!/usr/bin/env bash
# Render OpenSCAD enclosure parts to STL.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SCAD="$ROOT/enclosure/adapter/lid_insert_vl53l3cx_bme280.scad"
OUT_DIR="$ROOT/enclosure/adapter"

OPENSCAD=""
for candidate in \
  /Applications/OpenSCAD-2021.01.app/Contents/MacOS/OpenSCAD \
  /Applications/OpenSCAD.app/Contents/MacOS/OpenSCAD \
  openscad \
  /opt/homebrew/bin/openscad \
  /usr/local/bin/openscad; do
  if [[ -x "$candidate" ]] || command -v "$candidate" >/dev/null 2>&1; then
    OPENSCAD="$candidate"
    break
  fi
done

if [[ -z "$OPENSCAD" ]]; then
  echo "OpenSCAD not found; falling back to scripts/generate_stl.py" >&2
  python3 "$ROOT/scripts/generate_stl.py"
  exit 0
fi

# macOS Gatekeeper may kill quarantined OpenSCAD (signal 9)
if [[ "$OPENSCAD" == /Applications/* ]]; then
  app="${OPENSCAD%/Contents/MacOS/OpenSCAD}"
  xattr -dr com.apple.quarantine "$app" 2>/dev/null || true
fi

echo "==> Rendering lid_insert → STL"
"$OPENSCAD" -o "$OUT_DIR/lid_insert_vl53l3cx_bme280.stl" \
  -D 'PART="lid_insert"' "$SCAD"

echo "==> Rendering board_adapter → STL"
"$OPENSCAD" -o "$OUT_DIR/board_adapter_nrf52840.stl" \
  -D 'PART="board_adapter"' "$SCAD"

ls -la "$OUT_DIR"/*.stl
