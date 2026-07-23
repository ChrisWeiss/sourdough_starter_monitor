# Home Assistant — BTHome + MQTT republish

The jar sensor advertises **BTHome v2** (UUID `0xFCD2`). Home Assistant discovers it via Bluetooth proxies. Use the automation below to mirror readings onto your existing MQTT broker in sourd.io-style JSON.

## Prerequisites

1. [Bluetooth](https://www.home-assistant.io/integrations/bluetooth/) integration with coverage near the jar (built-in adapter or ESPHome Bluetooth proxy).
2. [MQTT](https://www.home-assistant.io/integrations/mqtt/) integration pointed at your local broker.
3. Device appears under Settings → Devices as a BTHome device (name defaults to `sourdough`).

## Entity IDs

After discovery, adjust these entity IDs to match your install (Developer Tools → States):

| Reading   | Typical entity                          |
|-----------|-----------------------------------------|
| Temp      | `sensor.sourdough_temperature`          |
| Humidity  | `sensor.sourdough_humidity`             |
| Distance  | `sensor.sourdough_distance`             |
| Battery   | `sensor.sourdough_battery`              |
| Pressure  | `sensor.sourdough_pressure` (optional)  |

Distance is advertised in **mm**; convert to cm in the payload to match sourd.io.

## MQTT topic

- Topic: `sourdough/sourdough/state`
- Payload example:

```json
{"humidity":55.20,"temp":24.50,"distance":12,"battery":97,"pressure":1013.25}
```

Replace the middle path segment with a stable device id if you run more than one jar.

## Automation

The live Home Assistant automation is created via the config API (alias **Sourdough BTHome → MQTT**, id `sourdough_bthome_mqtt_republish`). A checked-in reference copy lives in [`mqtt_republish.yaml`](mqtt_republish.yaml) for package/YAML installs.

After the BTHome device appears, confirm a retained message on `sourdough/sourdough/state` whenever temperature/humidity/distance/battery updates.
