# Home Assistant setup

1. Bring up the prototype ([bringup.md](bringup.md)) near a Bluetooth proxy / HA Bluetooth adapter.
2. Wait for **BTHome** discovery (Settings → Devices & services → devices). Name defaults to `sourdough`.
3. Confirm entities: temperature, humidity, distance (mm), battery, pressure.
4. The MQTT republish automation (**Sourdough BTHome → MQTT**) should already be present; adjust entity IDs if HA renamed them. Reference YAML: [`ha/mqtt_republish.yaml`](../ha/mqtt_republish.yaml).
5. Subscribe to `sourdough/sourdough/state` on your broker to verify JSON.

## Bluetooth proxy tips

- ESPHome `bluetooth_proxy:` on an always-on ESP32 near the kitchen works well.
- Advertising window is ~1.5s every sample interval — proxies must be awake/scanning.

## Debugging

- Serial monitor at 115200 on the jar MCU prints readings each wake (`./scripts/bringup_esp32.sh`).
- If HA never sees the device: check Flags in the advertisement (firmware sets `0x06`), UUID `FCD2`, and that passive scanning is enabled.
- If MQTT stays empty: confirm automation is on and entity IDs match Developer Tools → States.
