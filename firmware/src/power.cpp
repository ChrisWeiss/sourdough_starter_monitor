#include "power.h"

#include <Arduino.h>

#if defined(BOARD_ESP32)

#include <esp_sleep.h>

void power_deep_sleep_seconds(uint32_t seconds) {
  esp_sleep_enable_timer_wakeup(static_cast<uint64_t>(seconds) * 1000000ULL);
  Serial.flush();
  esp_deep_sleep_start();
}

uint8_t power_battery_percent() {
  // DevKit often has no battery sense — report "full" for USB prototyping.
  // Override with a voltage divider on GPIO36 and map to % when available.
  return 100;
}

#elif defined(BOARD_NRF52840)

void power_deep_sleep_seconds(uint32_t seconds) {
  // Low-power idle while SoftDevice runs. For true System OFF + RTC wake,
  // see docs/power.md once your specific nRF52840 board pinout is fixed.
  const uint32_t deadline = millis() + (seconds * 1000UL);
  while (static_cast<int32_t>(deadline - millis()) > 0) {
    waitForEvent();
  }
}

uint8_t power_battery_percent() {
  // Feather nRF52840: VBAT on A6 / PIN_VBAT when available.
#if defined(PIN_VBAT)
  float mv = analogRead(PIN_VBAT) * 2.0f * 3600.0f / 1024.0f;
  // Crude LiPo map 3.3V–4.2V
  float pct = (mv - 3300.0f) / (4200.0f - 3300.0f) * 100.0f;
  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;
  return static_cast<uint8_t>(pct);
#else
  return 100;
#endif
}

#else
#error "Define BOARD_ESP32 or BOARD_NRF52840"
#endif
