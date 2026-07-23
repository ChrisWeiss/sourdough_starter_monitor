# ESP32 bench bring-up

Goal: USB-powered Lolin/WeMos ESP32-WROOM advertising BTHome so Home Assistant can discover `sourdough`.

## Wiring checklist

Follow [wiring.md](wiring.md). Before powering USB:

1. BME280 VCC → 3V3, GND → GND, SDA → GPIO21, SCL → GPIO22
2. HC-SR04 VCC → **5V**, GND → GND, Trig → GPIO25
3. Echo → **voltage divider** → GPIO26 (never feed 5V Echo into the ESP32)
4. Confirm no short between 5V and 3V3

## Flash & serial

```bash
./scripts/bringup_esp32.sh
# or with an explicit port:
./scripts/bringup_esp32.sh --port /dev/cu.usbserial-XXXX
```

Healthy boot looks like:

```
sourdough BTHome monitor
temp=24.50C hum=55.20% press=1013.25hPa dist=12cm batt=100%
Advertising 18 bytes for 1500 ms
```

If BME280 is missing, firmware prints `WARN: BME280 not found` and an I2C scan of addresses seen on the bus. Try `-D BME280_I2C_ADDR=0x77` in `platformio.ini` if your module uses the alternate address.

If distance stays `-1`, re-check Echo divider polarity and that Trig/Echo pins match `PIN_US_*`.

## After serial looks good

1. Keep the board advertising within Bluetooth proxy range.
2. In Home Assistant → Settings → Devices, wait for BTHome device **sourdough**.
3. Confirm the MQTT republish automation (see [home-assistant.md](home-assistant.md)).
