#include "power.h"
#include "config.h"

#include <Arduino.h>

#if defined(BOARD_ESP32)

#include <esp_sleep.h>

void power_deep_sleep_seconds(uint32_t seconds) {
  esp_sleep_enable_timer_wakeup(static_cast<uint64_t>(seconds) * 1000000ULL);
  Serial.flush();
  esp_deep_sleep_start();
}

uint8_t power_battery_percent() {
#if defined(PIN_VBAT)
  // Optional divider on ADC1 (e.g. GPIO36). Scale assumes 2:1 divider to ~3.3V full.
  const float mv = analogRead(PIN_VBAT) * (3300.0f / 4095.0f) * 2.0f;
  float pct = (mv - VBAT_EMPTY_MV) / (VBAT_FULL_MV - VBAT_EMPTY_MV) * 100.0f;
  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;
  return static_cast<uint8_t>(pct);
#else
  // DevKit often has no battery sense — report "full" for USB prototyping.
  return 100;
#endif
}

#elif defined(BOARD_NRF52840)

#include <nrf_soc.h>
#include <nrf_sdm.h>

void power_deep_sleep_seconds(uint32_t seconds) {
  // nRF52 System OFF cannot wake from an internal timer — only GPIO/NFC/reset.
  // For duty-cycled sampling we stay in System ON low-power idle and RTC-wake
  // via SoftDevice (waitForEvent). Enable DCDC when the SoftDevice allows it.
  (void)sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
  (void)sd_power_mode_set(NRF_POWER_MODE_LOWPWR);

  const uint32_t deadline = millis() + (seconds * 1000UL);
  while (static_cast<int32_t>(deadline - millis()) > 0) {
    waitForEvent();
  }
}

void power_system_off() {
  // True System OFF (~0.3–1 µA). Requires a GPIO/NFC/reset to wake — e.g. an
  // external RTC open-drain to PIN_WAKE, or a user button. Call only when that
  // wake source is wired; otherwise the board will not resume on its own.
  Serial.flush();
  sd_power_system_off();
}

uint8_t power_battery_percent() {
#if defined(PIN_VBAT)
  // Feather nRF52840: VBAT sense is typically 2× attenuated into ADC.
  float mv = analogRead(PIN_VBAT) * 2.0f * 3600.0f / 1024.0f;
  float pct = (mv - VBAT_EMPTY_MV) / (VBAT_FULL_MV - VBAT_EMPTY_MV) * 100.0f;
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
