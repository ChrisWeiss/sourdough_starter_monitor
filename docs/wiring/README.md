# Assembly wiring diagrams (WireViz)

YAML sources here describe board ↔ sensor assembly wiring. Rendered SVGs live in [`generated/`](generated/).

## Prerequisites

```bash
brew install graphviz
python3 -m pip install --user wireviz
```

Ensure `wireviz` is on your `PATH` (e.g. `~/Library/Python/3.9/bin` on macOS system Python).

## Regenerate

```bash
./scripts/render_wiring.sh
```

Outputs: `docs/wiring/generated/{esp32,feather,xiao}.svg`

Pin numbers must stay aligned with `PIN_*` in [`firmware/platformio.ini`](../../firmware/platformio.ini) and the tables in [`../wiring.md`](../wiring.md).
