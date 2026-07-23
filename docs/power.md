# Power notes

## ESP32 DevKit (prototype)

`esp_deep_sleep_start()` after each advertise window. Sleep current on Lolin/WeMos DevKits is typically **milliamps**, not microamps, because of USB-UART and LDO. Use USB power for bring-up only.

## nRF52840 (production)

Firmware currently waits with `delay()` between samples so the SoftDevice stays happy on any Adafruit board without custom RTC wiring. For multi-month battery life:

1. Switch to RTC wake + `sd_power_system_off()` (or System ON idle with RTC).
2. Power down HC-SR04 VCC via a MOSFET between samples.
3. Keep BME280 in forced mode (already configured).

Battery % uses `PIN_VBAT` on Feather when defined; otherwise reports 100.
