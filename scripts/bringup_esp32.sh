#!/usr/bin/env bash
# Build, flash, and monitor the Lolin ESP32 prototype.
# Usage: ./scripts/bringup_esp32.sh [--port /dev/cu.usbserial-XXXX]
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
FW="$ROOT/firmware"
ENV_NAME="lolin-esp32"
PIO="${PIO:-$HOME/.platformio/penv/bin/pio}"
PORT=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --port) PORT="$2"; shift 2 ;;
    *) echo "Unknown arg: $1" >&2; exit 2 ;;
  esac
done

if [[ ! -x "$PIO" ]]; then
  if command -v pio >/dev/null 2>&1; then
    PIO="$(command -v pio)"
  else
    echo "PlatformIO not found at $HOME/.platformio/penv/bin/pio" >&2
    exit 1
  fi
fi

echo "==> Building $ENV_NAME"
(cd "$FW" && "$PIO" run -e "$ENV_NAME")

DEVICES="$("$PIO" device list 2>/dev/null || true)"
echo "$DEVICES"

if [[ -z "$PORT" ]]; then
  # Prefer common USB-UART adapters; skip macOS system debug ports.
  PORT="$(echo "$DEVICES" | awk '
    /^\/dev\/cu\./ {
      port=$1
      getline; getline
      if (port !~ /Bluetooth|debug-console|wlan-debug/) { print port; exit }
    }')"
fi

if [[ -z "$PORT" ]]; then
  echo ""
  echo "No ESP32 serial port detected."
  echo "Wire BME280 + HC-SR04 (see docs/wiring.md + docs/bringup.md),"
  echo "plug in the Lolin USB cable, then re-run:"
  echo "  $0 --port /dev/cu.usbserial-XXXX"
  exit 3
fi

echo "==> Uploading to $PORT"
(cd "$FW" && "$PIO" run -e "$ENV_NAME" -t upload --upload-port "$PORT")

echo "==> Serial monitor (Ctrl-C to stop). Expect temp/hum/dist + Advertising lines."
(cd "$FW" && "$PIO" device monitor -b 115200 --port "$PORT")
