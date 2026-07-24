# Wiring

Assembly diagrams are generated with [WireViz](https://github.com/wireviz/WireViz) from YAML under [`wiring/`](wiring/). Regenerate with `./scripts/render_wiring.sh` (see [`wiring/README.md`](wiring/README.md)).

Pin tables below stay aligned with `PIN_*` in `firmware/platformio.ini`.

**Distance sensor:** VL53L3CX ToF (I2C). Datasheet minimum range is **10 mm**; keep lid-to-dough clearance above that (recommend **≥ 30 mm** including window/foam). Needs a clear optical path through the lid (not ultrasonic holes).

## Prototype — Lolin / WeMos ESP32-WROOM

![ESP32 assembly wiring](wiring/generated/esp32.svg)

| Signal        | ESP32 GPIO | Notes                                      |
|---------------|------------|--------------------------------------------|
| BME280 SDA    | 21         | Shared I2C; address `0x76` or `0x77`       |
| BME280 SCL    | 22         |                                            |
| BME280 VCC    | 3V3        |                                            |
| BME280 GND    | GND        |                                            |
| VL53L3CX SDA  | 21         | Same bus; address `0x29`                   |
| VL53L3CX SCL  | 22         |                                            |
| VL53L3CX VCC  | 3V3        |                                            |
| VL53L3CX GND  | GND        |                                            |
| VL53L3CX XSHUT| (optional) | Active-low shutdown; leave pulled up if unused |

Override pins in `platformio.ini` `build_flags` (`PIN_BME_*`, optional `PIN_TOF_XSHUT`).

Bring-up steps: [bringup.md](bringup.md).

## Production — nRF52840

### Adafruit Feather (`env:nrf52840`)

![Feather nRF52840 assembly wiring](wiring/generated/feather.svg)

| Signal        | Feather pin | Notes                                      |
|---------------|-------------|--------------------------------------------|
| Shared SDA    | 25 (SDA)   | BME280 + VL53L3CX                          |
| Shared SCL    | 26 (SCL)   |                                            |
| Both VCC      | 3V3         |                                            |
| Both GND      | GND         |                                            |

### Seeed XIAO nRF52840 (`env:nrf52840-xiao`)

![XIAO nRF52840 assembly wiring](wiring/generated/xiao.svg)

| Signal        | XIAO pin | Notes                          |
|---------------|----------|--------------------------------|
| Shared SDA    | D4 (SDA) | Confirm silkscreen on your rev |
| Shared SCL    | D5 (SCL) |                                |
| Both VCC      | 3V3      |                                |
| Both GND      | GND      |                                |

## Power

- Both sensors are **3.3 V only** — no 5 V rail or Echo divider.
- Optional `PIN_TOF_XSHUT` for hardware standby between samples (see [power.md](power.md)).
