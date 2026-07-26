---
name: wireviz-wiring
description: >-
  Author and regenerate WireViz assembly wiring diagrams for this repo
  (MCU pin → sensor pin). Use when editing docs/wiring/*.yml, updating
  pin maps, PIN_* in platformio.ini, or when the user asks for wiring
  diagrams, assembly diagrams, or WireViz renders.
---

# WireViz assembly wiring

## Layout

| Path | Role |
|------|------|
| `docs/wiring/*.yml` | Source of truth for diagrams |
| `docs/wiring/generated/*.svg` | Committed SVG output |
| `docs/wiring/_layout.yml` | GraphViz layout tweak (TB, spacing); prepended on render |
| `docs/wiring.md` | Embeds SVGs + pin tables |
| `scripts/render_wiring.sh` | Batch render (creates `.venv` if needed) |

Pins must match `PIN_*` in `firmware/platformio.ini`.

## When changing pins

1. Edit the matching YAML (`esp32.yml`, `feather.yml`, or `xiao.yml`).
2. Update the pin table in `docs/wiring.md`.
3. Update `PIN_*` in `firmware/platformio.ini` if firmware changed.
4. Run `./scripts/render_wiring.sh`.
5. Commit YAML + regenerated SVG together.

## Shared I2C (BME280 + VL53L3CX)

Both sensors are 3.3 V I2C on the same SDA/SCL. Model two power cables and two I2C cables from the same MCU pins (WireViz has no native bus tee).

Default addresses: BME280 `0x76`/`0x77`, VL53L3CX `0x29`.

## Deps

```bash
brew install graphviz   # system `dot`
./scripts/render_wiring.sh   # uses project `.venv` + root requirements.txt
```
