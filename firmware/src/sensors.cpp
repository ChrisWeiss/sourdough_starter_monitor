#include "sensors.h"
#include "config.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <PWFusion_VL53L3C.h>
#include <math.h>

static Adafruit_BME280 bme;
static VL53L3C tof;
static bool bme_ok = false;
static bool tof_ok = false;

static void tof_xshut(bool enable_sensor) {
#if defined(PIN_TOF_XSHUT)
  // XSHUT is active-low shutdown on VL53L3CX
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

static long measure_distance_mm() {
  if (!tof_ok) {
    return -1;
  }

  tof_xshut(true);
  tof.startMeasurement();

  const uint32_t deadline = millis() + 500;
  while (!tof.dataIsReady()) {
    if (static_cast<int32_t>(deadline - millis()) <= 0) {
      tof.stopMeasurement();
      return -1;
    }
    delay(1);
  }

  MeasurmentResult result{};
  if (tof.getMeasurmentData(&result) != 0 || result.numObjs == 0) {
    tof.stopMeasurement();
    return -1;
  }
  tof.stopMeasurement();

  // Prefer the nearest valid target (starter surface under the lid).
  int16_t best = -1;
  for (uint8_t i = 0; i < result.numObjs; i++) {
    const int16_t mm = result.rangeData[i].Range;
    if (mm < VL53_MIN_RANGE_MM || mm > VL53_MAX_RANGE_MM) {
      continue;
    }
    if (best < 0 || mm < best) {
      best = mm;
    }
  }
  return best;
}

bool sensors_begin() {
#if defined(PIN_TOF_XSHUT)
  pinMode(PIN_TOF_XSHUT, OUTPUT);
  digitalWrite(PIN_TOF_XSHUT, HIGH);
#endif

#if defined(BOARD_ESP32)
  Wire.begin(PIN_BME_SDA, PIN_BME_SCL);
#else
  Wire.begin();
#endif
  Wire.setClock(400000);

  bme_ok = bme.begin(BME280_I2C_ADDR, &Wire);
  if (!bme_ok) {
    bme_ok = bme.begin(0x77, &Wire);
  }
  if (bme_ok) {
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1,
                    Adafruit_BME280::SAMPLING_X1,
                    Adafruit_BME280::SAMPLING_X1,
                    Adafruit_BME280::FILTER_OFF,
                    Adafruit_BME280::STANDBY_MS_1000);
  }

  tof.begin(Wire);
  // Short mode favors near-field linearity for jar-lid geometry.
  if (tof.setDistanceMode(DIST_SHORT) == 0 &&
      tof.setTimingBudget(VL53_TIMING_BUDGET_US) == 0) {
    tof_ok = true;
  } else {
    tof_ok = false;
  }

  if (!bme_ok || !tof_ok) {
    Serial.println(F("WARN: sensor init issue"));
    if (!bme_ok) {
      Serial.println(F("  BME280 not found"));
    }
    if (!tof_ok) {
      Serial.println(F("  VL53L3CX not ready"));
    }
    i2c_scan_log();
  }

  return bme_ok; // climate is required for a useful node; ToF may recover later
}

SensorReading sensors_read() {
  SensorReading r{};
  r.climate_ok = false;
  r.distance_ok = false;
  r.temperature_c = NAN;
  r.humidity_pct = NAN;
  r.pressure_hpa = NAN;
  r.distance_mm = -1;

  if (bme_ok) {
    bme.takeForcedMeasurement();
    r.temperature_c = bme.readTemperature();
    r.humidity_pct = bme.readHumidity();
    r.pressure_hpa = bme.readPressure() / 100.0F;
    r.climate_ok = !isnan(r.temperature_c) && !isnan(r.humidity_pct);
  }

  long mm = measure_distance_mm();
  if (mm >= VL53_MIN_RANGE_MM) {
    r.distance_mm = mm;
    r.distance_ok = true;
  }

  return r;
}
