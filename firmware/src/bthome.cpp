#include "bthome.h"
#include "config.h"

#include <Arduino.h>
#include <math.h>
#include <string.h>

// BTHome v2 object IDs (ascending order required)
static constexpr uint8_t OID_PACKET_ID = 0x00;
static constexpr uint8_t OID_BATTERY = 0x01;
static constexpr uint8_t OID_TEMP = 0x02;
static constexpr uint8_t OID_HUMIDITY = 0x03;
static constexpr uint8_t OID_PRESSURE = 0x04;
static constexpr uint8_t OID_DISTANCE_MM = 0x40;

// Device info: unencrypted, regular interval, BTHome v2 → 0x40
static constexpr uint8_t BTHOME_DEVICE_INFO = 0x40;

static void write_le16(uint8_t *p, int16_t v) {
  p[0] = static_cast<uint8_t>(v & 0xFF);
  p[1] = static_cast<uint8_t>((v >> 8) & 0xFF);
}

static void write_le16u(uint8_t *p, uint16_t v) {
  p[0] = static_cast<uint8_t>(v & 0xFF);
  p[1] = static_cast<uint8_t>((v >> 8) & 0xFF);
}

static void write_le24u(uint8_t *p, uint32_t v) {
  p[0] = static_cast<uint8_t>(v & 0xFF);
  p[1] = static_cast<uint8_t>((v >> 8) & 0xFF);
  p[2] = static_cast<uint8_t>((v >> 16) & 0xFF);
}

size_t bthome_build_payload(uint8_t *out, size_t out_len, const SensorReading &r,
                            uint8_t battery_pct, uint8_t packet_id) {
  // Worst case: info + packet + battery + temp + hum + pressure + distance
  // 1+2+2+3+3+4+3 = 18 bytes
  if (out_len < 20) {
    return 0;
  }

  size_t i = 0;
  out[i++] = BTHOME_DEVICE_INFO;

  out[i++] = OID_PACKET_ID;
  out[i++] = packet_id;

  out[i++] = OID_BATTERY;
  out[i++] = battery_pct > 100 ? 100 : battery_pct;

  if (r.climate_ok) {
    int16_t t = static_cast<int16_t>(lroundf(r.temperature_c * 100.0f));
    out[i++] = OID_TEMP;
    write_le16(&out[i], t);
    i += 2;

    uint16_t h = static_cast<uint16_t>(lroundf(r.humidity_pct * 100.0f));
    out[i++] = OID_HUMIDITY;
    write_le16u(&out[i], h);
    i += 2;

    if (!isnan(r.pressure_hpa)) {
      uint32_t p = static_cast<uint32_t>(lroundf(r.pressure_hpa * 100.0f));
      out[i++] = OID_PRESSURE;
      write_le24u(&out[i], p);
      i += 3;
    }
  }

  if (r.distance_ok) {
    uint16_t mm = static_cast<uint16_t>(r.distance_cm * 10);
    out[i++] = OID_DISTANCE_MM;
    write_le16u(&out[i], mm);
    i += 2;
  }

  return i;
}

#if defined(BOARD_ESP32)

#include <NimBLEDevice.h>

static NimBLEAdvertising *adv = nullptr;

bool bthome_advertise_begin(const char *device_name) {
  NimBLEDevice::init(device_name);
  NimBLEDevice::setPower(ESP_PWR_LVL_P3);
  adv = NimBLEDevice::getAdvertising();
  return adv != nullptr;
}

bool bthome_advertise(const uint8_t *payload, size_t payload_len) {
  if (!adv || payload_len == 0 || payload_len > 27) {
    return false;
  }

  NimBLEAdvertisementData ad;
  ad.setFlags(0x06);
  ad.setName(DEVICE_NAME);
  // Service data UUID 0xFCD2 (little-endian bytes in API)
  ad.setServiceData(NimBLEUUID((uint16_t)0xFCD2),
                    std::string(reinterpret_cast<const char *>(payload), payload_len));

  adv->setAdvertisementData(ad);
  adv->setScanResponse(false);
  adv->start();
  return true;
}

void bthome_advertise_stop() {
  if (adv) {
    adv->stop();
  }
}

#elif defined(BOARD_NRF52840)

#include <bluefruit.h>

bool bthome_advertise_begin(const char *device_name) {
  Bluefruit.begin();
  Bluefruit.setTxPower(4);
  Bluefruit.setName(device_name);
  Bluefruit.Advertising.clearData();
  return true;
}

bool bthome_advertise(const uint8_t *payload, size_t payload_len) {
  if (payload_len == 0 || payload_len > 27) {
    return false;
  }

  Bluefruit.Advertising.stop();
  Bluefruit.Advertising.clearData();
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addName();

  // Service data: 16-bit UUID FCD2 + payload
  uint8_t svc[2 + 27];
  svc[0] = 0xD2;
  svc[1] = 0xFC;
  memcpy(&svc[2], payload, payload_len);
  Bluefruit.Advertising.addData(BLE_GAP_AD_TYPE_SERVICE_DATA, svc, 2 + payload_len);

  Bluefruit.Advertising.setInterval(160, 160); // 100 ms
  Bluefruit.Advertising.setFastTimeout(1);
  Bluefruit.Advertising.start(0);
  return true;
}

void bthome_advertise_stop() {
  Bluefruit.Advertising.stop();
}

#else
#error "Define BOARD_ESP32 or BOARD_NRF52840"
#endif
