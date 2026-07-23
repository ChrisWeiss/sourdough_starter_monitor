# Home Assistant setup

1. Place the powered prototype near a Bluetooth proxy / HA Bluetooth adapter.
2. Wait for **BTHome** discovery (Settings → Devices & services → devices).
3. Confirm entities: temperature, humidity, distance (mm), battery, pressure.
4. Install [`ha/mqtt_republish.yaml`](../ha/mqtt_republish.yaml) and fix entity IDs.
5. Subscribe to `sourdough/sourdough/state` on your broker to verify JSON.

## Bluetooth proxy tips

- ESPHome `bluetooth_proxy:` on an always-on ESP32 near the kitchen works well.
- Passivevertising window is ~1.5s every sample interval — proxies must be awake/scanning.

## Debugging

- Serial monitor at 115200 on the jar MCU prints readings each wake.
- If HA never sees the device: check Flags in the advertisement (firmware sets `0x06`), UUID `FCD2`, and that passive scanning is enabled.
