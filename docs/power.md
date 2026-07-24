# Power notes

## ESP32 DevKit (prototype)

`esp_deep_sleep_start()` after each advertise window. Sleep current on Lolin/WeMos DevKits is typically **milliamps**, not microamps, because of USB-UART and LDO. Use USB power for bring-up only.

Optional battery sense: define `PIN_VBAT` (ADC1) with a 2:1 divider and calibrate `VBAT_EMPTY_MV` / `VBAT_FULL_MV` in `platformio.ini`.

## nRF52840 (production)

### Duty-cycle sleep (default)

Firmware uses **System ON low-power idle** (`NRF_POWER_MODE_LOWPWR` + `waitForEvent`) between samples. That is the correct pattern for RTC/timer wake on nRF52: **System OFF cannot wake from an internal timer** (only GPIO, NFC, or reset).

Enable SoftDevice DCDC when supported. Keep the BME280 in forced mode (already configured).

### ToF (VL53L3CX) power

The VL53L3CX is a 3.3 V I2C device. Optional `PIN_TOF_XSHUT` drives the active-low XSHUT pin for hardware standby between samples.

### True System OFF

`power_system_off()` enters System OFF (~µA). Wire an external RTC or button to a wake pin before calling it — otherwise the node will not resume on its own. Prefer the default System ON idle path for the 90 s sample loop.

### Battery %

Uses `PIN_VBAT` when the board defines it (Feather). Calibrate with a meter:

| Cell voltage | Expected % |
|--------------|------------|
| 3.3 V | ~0 |
| 3.7 V | ~44 |
| 4.2 V | ~100 |

Override `-D VBAT_EMPTY_MV=…` / `-D VBAT_FULL_MV=…` if your divider or ADC scale differs. Measure sleep current on LiPo after disconnecting USB.

### Board targets

| Env | Board |
|-----|-------|
| `nrf52840` | Adafruit Feather nRF52840 Express |
| `nrf52840-xiao` | XIAO pin map (Adafruit SoftDevice BSP build; flash via XIAO UF2 / Seeed tools) |

Adjust `PIN_*` in `platformio.ini` for your wiring. See [wiring.md](wiring.md).
