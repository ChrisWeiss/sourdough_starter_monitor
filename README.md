# Sourdough Starter Monitor (BLE / BTHome)

Battery-oriented remake of [twilio/sourd.io](https://github.com/twilio/sourd.io): temperature, humidity, and rise (ultrasonic distance) for a sourdough starter jar.

Instead of Twilio Narrowband cellular, the jar node **advertises BTHome v2 over BLE**. Home Assistant ingests the readings; an automation republishes **sourd.io-style JSON** to your existing MQTT broker.

## Hardware

| Role | Prototype | Production |
|------|-----------|------------|
| MCU | Lolin / WeMos ESP32-WROOM DevKit | nRF52840 (Feather or XIAO) + LiPo |
| Climate | BME280 (I2C) | same |
| Distance | HC-SR04 (trig/echo; level-shift Echo) | 3.3V-tolerant ultrasonic or 5V boost + divider |
| Enclosure | Bench wiring | sourd.io v2.5 jar stack + remixed lid insert |

See [docs/wiring.md](docs/wiring.md) (WireViz assembly diagrams) and [docs/bringup.md](docs/bringup.md). Regenerate diagrams with `./scripts/render_wiring.sh`.

## Firmware

PlatformIO project in [`firmware/`](firmware/). Use the project virtualenv (PlatformIO + WireViz):

```bash
./scripts/setup_venv.sh
source .venv/bin/activate

# One-shot build + flash + monitor (auto-detects USB serial)
./scripts/bringup_esp32.sh

cd firmware
pio run -e lolin-esp32          # build prototype
pio run -e nrf52840             # Adafruit Feather
pio run -e nrf52840-xiao        # Seeed XIAO nRF52840
```

Duty cycle: wake → read sensors → advertise ~1.5 s → sleep (default **90 s**, `SAMPLE_INTERVAL_SEC`).

## Home Assistant + MQTT

1. Discover the BTHome device (name `sourdough`).
2. Automation **Sourdough BTHome → MQTT** republishes to the broker (see [ha/](ha/)).
3. Subscribe to `sourdough/sourdough/state`:

```json
{"humidity":55.2,"temp":24.5,"distance":12,"battery":97,"pressure":1013.25}
```

Details: [docs/home-assistant.md](docs/home-assistant.md), [ha/README.md](ha/README.md).

## Enclosure

Vendored sourd.io STLs plus OpenSCAD remix and rendered adapters: [enclosure/README.md](enclosure/README.md).

```bash
./scripts/render_enclosure.sh
```

## License

Firmware and project docs: Apache-2.0 (aligned with sourd.io). Vendored enclosure STLs retain upstream Apache-2.0 copyright from Christine Sunu / Twilio sourd.io.
