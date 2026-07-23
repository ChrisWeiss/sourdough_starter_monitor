# Wiring

## Prototype — Lolin / WeMos ESP32-WROOM

| Signal        | ESP32 GPIO | Notes                                      |
|---------------|------------|--------------------------------------------|
| BME280 SDA    | 21         | 3.3V I2C; address `0x76` or `0x77`         |
| BME280 SCL    | 22         |                                            |
| BME280 VCC    | 3V3        |                                            |
| BME280 GND    | GND        |                                            |
| HC-SR04 VCC   | 5V         | Module expects 5V                          |
| HC-SR04 GND   | GND        |                                            |
| HC-SR04 Trig  | 25         | 3.3V OK out of ESP32                       |
| HC-SR04 Echo  | 26         | **Level-shift / divider to 3.3V**          |

Echo voltage divider (example): Echo → 2.2kΩ → GPIO26 → 4.7kΩ → GND.

Override pins in `platformio.ini` `build_flags` (`PIN_*`).

## Production — nRF52840 (Adafruit Feather defaults in `platformio.ini`)

| Signal        | Feather pin | Notes                                      |
|---------------|-------------|--------------------------------------------|
| BME280 SDA    | 25 (SDA)   | Board defaults; Wire.begin()               |
| BME280 SCL    | 26 (SCL)   |                                            |
| HC-SR04 Trig  | 27          | Adjust to free GPIOs on your board         |
| HC-SR04 Echo  | 7           | Must be ≤ 3.3V                             |

nRF52840 has no 5V rail. Options:

1. Use a **3.3V-tolerant HC-SR04** / RCWL-1601 / similar.
2. Or a small boost to 5V for the module + divider on Echo.

## Power

- ESP32 DevKit: USB for bring-up. Deep sleep works, but board quiescent current is high — not a battery reference.
- nRF52840 + LiPo: intended battery path. Sample every 90s (configurable via `SAMPLE_INTERVAL_SEC`).
