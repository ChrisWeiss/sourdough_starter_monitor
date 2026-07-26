#include <Arduino.h>

#include "bthome.h"
#include "config.h"
#include "power.h"
#include "sensors.h"

static uint8_t packet_id = 0;

void setup() {
  Serial.begin(115200);
  // USB CDC on nRF: wait briefly so boot diagnostics are visible.
  uint32_t wait_until = millis() + 3000;
  while (!Serial && static_cast<int32_t>(wait_until - millis()) > 0) {
    delay(10);
  }
  delay(50);
  Serial.println();
  Serial.println(F("sourdough BTHome monitor"));

  if (!sensors_begin()) {
    Serial.println(F("WARN: climate sensor not found — temp/pressure omitted"));
  }

  if (!bthome_advertise_begin(DEVICE_NAME)) {
    Serial.println(F("ERROR: BLE init failed"));
  }
}

void loop() {
  SensorReading r = sensors_read();
  uint8_t batt = power_battery_percent();

  Serial.printf("temp=%.2fC hum=", r.temperature_c);
  if (r.humidity_ok) {
    Serial.printf("%.2f%%", r.humidity_pct);
  } else {
    Serial.print(F("n/a"));
  }
  Serial.printf(" press=%.2fhPa dist=%ldmm batt=%u%%\n",
                r.pressure_hpa, r.distance_mm, batt);

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
