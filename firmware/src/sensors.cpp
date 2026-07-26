#include "sensors.h"
#include "config.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_VL53L1X.h>
#include <math.h>

static Adafruit_BME280 bme;
static Adafruit_BMP280 bmp;
static Adafruit_VL53L1X tof = Adafruit_VL53L1X();
static bool climate_ok = false;
static bool humidity_ok = false;
static bool using_bmp = false;
static bool tof_ok = false;

static void tof_xshut(bool enable_sensor) {
#if defined(PIN_TOF_XSHUT)
  // XSHUT is active-low shutdown on VL53L1X
  digitalWrite(PIN_TOF_XSHUT, enable_sensor ? HIGH : LOW);
  if (enable_sensor) {
    delay(2);
  }
#else
  (void)enable_sensor;
#endif
}

static void i2c_scan_log() {
  Serial.println(F("I2C scan:"));
  uint8_t found = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  - 0x%02X\n", addr);
      found++;
    }
  }
  if (found == 0) {
    Serial.println(F("  (no devices)"));
  }
}

static void i2c_bus_begin() {
#if defined(BOARD_ESP32)
  Wire.begin(PIN_BME_SDA, PIN_BME_SCL);
#elif defined(PIN_BME_SDA) && defined(PIN_BME_SCL)
  // Adafruit nRF52 Wire uses Arduino pin indices (see board variant map).
  Wire.setPins(PIN_BME_SDA, PIN_BME_SCL);
  Wire.begin();
#else
  Wire.begin();
#endif
#ifndef I2C_CLOCK_HZ
#define I2C_CLOCK_HZ 100000
#endif
  Wire.setClock(I2C_CLOCK_HZ);

#if defined(PIN_BME_SDA) && defined(PIN_BME_SCL)
  Serial.printf("I2C pins: SDA=%u SCL=%u @ %u Hz\n",
                (unsigned)PIN_BME_SDA, (unsigned)PIN_BME_SCL,
                (unsigned)I2C_CLOCK_HZ);
#if defined(BOARD_NRF52840)
  extern const uint32_t g_ADigitalPinMap[];
  Serial.printf("I2C GPIO: SDA=P%u.%02u SCL=P%u.%02u\n",
                (unsigned)(g_ADigitalPinMap[PIN_BME_SDA] >= 32),
                (unsigned)(g_ADigitalPinMap[PIN_BME_SDA] & 31),
                (unsigned)(g_ADigitalPinMap[PIN_BME_SCL] >= 32),
                (unsigned)(g_ADigitalPinMap[PIN_BME_SCL] & 31));
#endif
#endif
}

static long measure_distance_mm() {
  if (!tof_ok) {
    return -1;
  }

  tof_xshut(true);
  if (!tof.startRanging()) {
    return -1;
  }

  int16_t samples[VL53_SAMPLE_COUNT];
  uint8_t n = 0;
  const uint32_t per_sample_ms = static_cast<uint32_t>(VL53_TIMING_BUDGET_MS) + 100UL;

  while (n < VL53_SAMPLE_COUNT) {
    const uint32_t deadline = millis() + per_sample_ms;
    while (!tof.dataReady()) {
      if (static_cast<int32_t>(deadline - millis()) <= 0) {
        break;
      }
      delay(1);
    }
    if (!tof.dataReady()) {
      break;
    }

    uint8_t range_status = 0xFF;
    (void)tof.VL53L1X_GetRangeStatus(&range_status);
    const int16_t mm = tof.distance();
    tof.clearInterrupt();

    // ULD: 0 = range valid.
    if (range_status != 0 || mm < 0) {
      continue;
    }
    if (mm < VL53_MIN_RANGE_MM || mm > VL53_MAX_RANGE_MM) {
      continue;
    }
    samples[n++] = mm;
  }

  (void)tof.stopRanging();
  tof_xshut(false);

  if (n == 0) {
    return -1;
  }

  // Insertion sort then median (odd count preferred; even → average of middle).
  for (uint8_t i = 1; i < n; i++) {
    const int16_t key = samples[i];
    int8_t j = static_cast<int8_t>(i) - 1;
    while (j >= 0 && samples[j] > key) {
      samples[j + 1] = samples[j];
      j--;
    }
    samples[j + 1] = key;
  }
  if ((n & 1U) != 0) {
    return samples[n / 2];
  }
  return (static_cast<long>(samples[n / 2 - 1]) + samples[n / 2]) / 2;
}

static bool climate_begin_bme(uint8_t addr) {
  if (!bme.begin(addr, &Wire)) {
    return false;
  }
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1,
                  Adafruit_BME280::SAMPLING_X1,
                  Adafruit_BME280::SAMPLING_X1,
                  Adafruit_BME280::FILTER_OFF,
                  Adafruit_BME280::STANDBY_MS_1000);
  using_bmp = false;
  humidity_ok = true;
  Serial.printf("BME280 OK @0x%02X (chip=0x%02X)\n", addr,
                (unsigned)bme.sensorID());
  return true;
}

static bool climate_begin_bmp(uint8_t addr) {
  if (!bmp.begin(addr)) {
    return false;
  }
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,
                  Adafruit_BMP280::SAMPLING_X1,
                  Adafruit_BMP280::SAMPLING_X1,
                  Adafruit_BMP280::FILTER_OFF,
                  Adafruit_BMP280::STANDBY_MS_1000);
  using_bmp = true;
  humidity_ok = false;
  Serial.printf("BMP280 OK @0x%02X (temp+pressure; no humidity)\n", addr);
  return true;
}

bool sensors_begin() {
#if defined(PIN_TOF_XSHUT)
  pinMode(PIN_TOF_XSHUT, OUTPUT);
  digitalWrite(PIN_TOF_XSHUT, HIGH);
#endif

  i2c_bus_begin();
  i2c_scan_log();

  climate_ok = false;
  humidity_ok = false;
  using_bmp = false;

  const uint8_t addrs[] = {BME280_I2C_ADDR, 0x77};
  for (uint8_t i = 0; i < 2 && !climate_ok; i++) {
    if (climate_begin_bme(addrs[i])) {
      climate_ok = true;
    }
  }
  for (uint8_t i = 0; i < 2 && !climate_ok; i++) {
    if (climate_begin_bmp(addrs[i])) {
      climate_ok = true;
    }
  }
  if (!climate_ok) {
    Serial.println(F("Climate sensor not found (tried BME280 then BMP280)"));
  }

  tof_ok = false;
  if (tof.begin(VL53_I2C_ADDR, &Wire)) {
    (void)tof.VL53L1X_SetDistanceMode(1); // short: better near-field / ambient
    (void)tof.VL53L1X_SetROI(VL53_ROI_XY, VL53_ROI_XY);
    const uint16_t budget_ms = static_cast<uint16_t>(VL53_TIMING_BUDGET_MS);
    if (!tof.setTimingBudget(budget_ms)) {
      Serial.printf("VL53L1X setTimingBudget(%u) failed\n", budget_ms);
    }
    // Inter-measurement must be >= timing budget while ranging.
    (void)tof.VL53L1X_SetInterMeasurementInMs(budget_ms);
    // Do not leave continuous ranging on between samples (nRF light-sleep loop).
    tof_ok = true;
    Serial.printf("VL53L1X OK (short, ROI=%ux%u, budget=%ums, median=%u)\n",
                  (unsigned)VL53_ROI_XY, (unsigned)VL53_ROI_XY,
                  (unsigned)budget_ms, (unsigned)VL53_SAMPLE_COUNT);
  } else {
    Serial.printf("VL53L1X begin failed status=%u\n", (unsigned)tof.vl_status);
  }

  if (!climate_ok || !tof_ok) {
    Serial.println(F("WARN: sensor init issue — rescanning I2C"));
    i2c_scan_log();
  }

  return climate_ok;
}

SensorReading sensors_read() {
  SensorReading r{};
  r.climate_ok = false;
  r.humidity_ok = false;
  r.distance_ok = false;
  r.temperature_c = NAN;
  r.humidity_pct = NAN;
  r.pressure_hpa = NAN;
  r.distance_mm = -1;

  if (climate_ok) {
    if (using_bmp) {
      // Forced mode: one measurement per read.
      (void)bmp.takeForcedMeasurement();
      r.temperature_c = bmp.readTemperature();
      r.pressure_hpa = bmp.readPressure() / 100.0F;
      r.humidity_pct = NAN;
      r.humidity_ok = false;
    } else {
      bme.takeForcedMeasurement();
      r.temperature_c = bme.readTemperature();
      r.humidity_pct = bme.readHumidity();
      r.pressure_hpa = bme.readPressure() / 100.0F;
      r.humidity_ok = !isnan(r.humidity_pct);
    }
    r.climate_ok = !isnan(r.temperature_c);
  }

  long mm = measure_distance_mm();
  if (mm >= VL53_MIN_RANGE_MM) {
    r.distance_mm = mm;
    r.distance_ok = true;
  }

  return r;
}
