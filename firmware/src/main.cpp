#include <Arduino.h>

#include "bthome.h"
#include "config.h"
#include "power.h"
#include "sensors.h"

static uint8_t packet_id = 0;

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println(F("sourdough BTHome monitor"));

  if (!sensors_begin()) {
    Serial.println(F("WARN: BME280 not found — climate fields will be omitted"));
  }

  if (!bthome_advertise_begin(DEVICE_NAME)) {
    Serial.println(F("ERROR: BLE init failed"));
  }
}

void loop() {
  SensorReading r = sensors_read();
  uint8_t batt = power_battery_percent();

  Serial.printf("temp=%.2fC hum=%.2f%% press=%.2fhPa dist=%ldmm batt=%u%%\n",
                r.temperature_c, r.humidity_pct, r.pressure_hpa, r.distance_mm, batt);

  uint8_t payload[24];
  size_t n = bthome_build_payload(payload, sizeof(payload), r, batt, packet_id++);
  if (n == 0) {
    Serial.println(F("ERROR: BTHome payload build failed"));
  } else if (!bthome_advertise(payload, n)) {
    Serial.println(F("ERROR: advertise failed"));
  } else {
    Serial.printf("Advertising %u bytes for %u ms\n", (unsigned)n, ADVERTISE_DURATION_MS);
    delay(ADVERTISE_DURATION_MS);
    bthome_advertise_stop();
  }

#if defined(BOARD_ESP32)
  // Full deep sleep — setup() runs again on wake.
  power_deep_sleep_seconds(SAMPLE_INTERVAL_SEC);
#else
  // nRF: light sleep between samples (see power.cpp notes for system-off).
  power_deep_sleep_seconds(SAMPLE_INTERVAL_SEC);
#endif
}
