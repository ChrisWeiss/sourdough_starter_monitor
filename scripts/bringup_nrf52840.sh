#!/usr/bin/env bash
# Build, flash, and monitor an nRF52840 board (Adafruit UF2 / SoftDevice).
# Default env: nrf52840-promicro (Pro Micro / SuperMini / nice!nano clones).
#
# Usage:
#   ./scripts/bringup_nrf52840.sh
#   ./scripts/bringup_nrf52840.sh --env nrf52840-promicro
#   ./scripts/bringup_nrf52840.sh --port /dev/cu.usbmodemXXXX
#
# Board should enumerate as a USB CDC device (Adafruit / nice!nano). If upload
# fails, double-tap reset to enter the NICENANO UF2 bootloader, then re-run.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
FW="$ROOT/firmware"
ENV_NAME="nrf52840-promicro"
PORT=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --port) PORT="$2"; shift 2 ;;
    --env) ENV_NAME="$2"; shift 2 ;;
    *) echo "Unknown arg: $1" >&2; exit 2 ;;
  esac
done

resolve_pio() {
  if [[ -n "${PIO:-}" && -x "$PIO" ]]; then
    echo "$PIO"
    return
  fi
  for candidate in \
    "$ROOT/.venv/bin/pio" \
    "$HOME/.platformio/penv/bin/pio"; do
    if [[ -x "$candidate" ]]; then
      echo "$candidate"
      return
    fi
  done
  if command -v pio >/dev/null 2>&1; then
    command -v pio
    return
  fi
  echo "PlatformIO not found. Create the project venv first:" >&2
  echo "  ./scripts/setup_venv.sh" >&2
  echo "  source .venv/bin/activate" >&2
  exit 1
}

PIO="$(resolve_pio)"

echo "==> Building $ENV_NAME"
(cd "$FW" && "$PIO" run -e "$ENV_NAME")

DEVICES="$("$PIO" device list 2>/dev/null || true)"
echo "$DEVICES"

if [[ -z "$PORT" ]]; then
  # Prefer Adafruit / nice!nano CDC (VID 239A); skip system/debug ports and LG monitors.
  PORT="$(echo "$DEVICES" | awk '
    /^\/dev\/cu\./ {
      port=$1
      block=""
      for (i = 0; i < 4; i++) {
        if (!getline) break
        block = block $0 "\n"
      }
      if (port ~ /Bluetooth|debug-console|wlan-debug/) next
      if (block ~ /239A|Feather|nice!nano|nRF52840/) { print port; exit }
    }')"
fi

if [[ -z "$PORT" ]]; then
  PORT="$(echo "$DEVICES" | awk '
    /^\/dev\/cu\./ {
      port=$1
      getline; getline
      if (port !~ /Bluetooth|debug-console|wlan-debug/) { print port; exit }
    }')"
fi

if [[ -z "$PORT" ]]; then
  echo ""
  echo "No nRF52840 serial port detected."
  echo "Wire BMP280/BME280 + VL53L1X on I2C (see docs/wiring.md),"
  echo "plug in USB-C, then re-run:"
  echo "  $0 --port /dev/cu.usbmodemXXXX"
  echo "If needed, double-tap RST to enter the UF2 bootloader (NICENANO volume)."
  exit 3
fi

echo "==> Uploading to $PORT (env=$ENV_NAME)"
(cd "$FW" && "$PIO" run -e "$ENV_NAME" -t upload --upload-port "$PORT")

echo "==> Serial monitor (Ctrl-C to stop). Expect temp/press/dist + Advertising lines."
(cd "$FW" && "$PIO" device monitor -b 115200 --port "$PORT")
