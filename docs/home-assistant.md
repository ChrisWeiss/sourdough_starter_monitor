# Home Assistant setup

1. Bring up the prototype ([bringup.md](bringup.md)) near a Bluetooth proxy / HA Bluetooth adapter.
2. Wait for **BTHome** discovery (Settings → Devices & services → devices). Name defaults to `sourdough`.
3. Confirm entities: temperature, humidity (BME280 only), **distance**, battery, pressure.
4. **Distance units:** firmware already advertises BTHome distance in **mm**. If HA shows inches (1 in steps), that is display conversion from your unit system — not firmware resolution.
   - Open the distance entity → cog → **Unit of measurement** → **mm**
   - Optionally set display precision to `0` (whole millimetres)
5. The MQTT republish automation (**Sourdough BTHome → MQTT**) should already be present; adjust entity IDs if HA renamed them. Reference YAML: [`ha/mqtt_republish.yaml`](../ha/mqtt_republish.yaml). Payload includes `distance_mm` and legacy `distance` (cm).
6. Subscribe to `sourdough/sourdough/state` on your broker to verify JSON.

## Bluetooth proxy tips

- ESPHome `bluetooth_proxy:` on an always-on ESP32 near the kitchen works well.
- Advertising window is ~1.5s every sample interval — proxies must be awake/scanning.

## Trust / hardening notes

- BTHome advertisements are unencrypted; treat the kitchen BLE as a trusted LAN segment (or use BTHome encryption keys if you enable them later).
- The MQTT republish uses `retain: true` so last state survives broker restarts — fine for dashboards; disable retain if you prefer ephemeral telemetry.

## Debugging

- Serial monitor at 115200: `./scripts/bringup_esp32.sh` or `./scripts/bringup_nrf52840.sh`.
- If HA never sees the device: check Flags in the advertisement (firmware sets `0x06`), UUID `FCD2`, and that passive scanning is enabled.
- If MQTT stays empty: confirm automation is on and entity IDs match Developer Tools → States.
