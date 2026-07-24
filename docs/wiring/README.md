# Assembly wiring diagrams (WireViz)

YAML sources here describe board ↔ sensor assembly wiring. Rendered SVGs live in [`generated/`](generated/).

## Prerequisites

1. System GraphViz (provides `dot`):

```bash
brew install graphviz
```

2. Project virtualenv (PlatformIO + WireViz). Prefer the setup script once:

```bash
./scripts/setup_venv.sh
source .venv/bin/activate
```

Or let `./scripts/render_wiring.sh` create `.venv` from the repo-root [`requirements.txt`](../../requirements.txt).

## Regenerate

```bash
./scripts/render_wiring.sh
```

Creates `.venv` if needed, then writes `docs/wiring/generated/{esp32,feather,xiao}.svg`.

Layout is controlled by [`_layout.yml`](_layout.yml) (prepended on every render): GraphViz `rankdir=TB` for a taller/narrower harness instead of WireViz’s default left-to-right. Adjust `ranksep` / `nodesep` there if needed.

Pin numbers must stay aligned with `PIN_*` in [`firmware/platformio.ini`](../../firmware/platformio.ini) and the tables in [`../wiring.md`](../wiring.md).
