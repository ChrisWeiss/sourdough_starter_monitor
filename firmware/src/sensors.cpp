#include "sensors.h"
#include "config.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <math.h>

static Adafruit_BME280 bme;
static bool bme_ok = false;

static void ultrasonic_power(bool on) {
#if defined(PIN_US_PWR)
  digitalWrite(PIN_US_PWR, on ? HIGH : LOW);
  if (on) {
    delay(60); // HC-SR04 settle after VCC rise
  }
#else
  (void)on;
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

static long measure_distance_cm() {
  ultrasonic_power(true);

  digitalWrite(PIN_US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_US_TRIG, LOW);

  unsigned long duration = pulseIn(PIN_US_ECHO, HIGH, US_TIMEOUT_US);

  ultrasonic_power(false);

  if (duration == 0) {
    return -1;
  }
  // duration (us) / 58 ≈ cm
  return static_cast<long>(duration / 58UL);
}

bool sensors_begin() {
  pinMode(PIN_US_TRIG, OUTPUT);
  pinMode(PIN_US_ECHO, INPUT);
  digitalWrite(PIN_US_TRIG, LOW);

#if defined(PIN_US_PWR)
  pinMode(PIN_US_PWR, OUTPUT);
  digitalWrite(PIN_US_PWR, LOW);
#endif

#if defined(BOARD_ESP32)
  Wire.begin(PIN_BME_SDA, PIN_BME_SCL);
#else
  Wire.begin();
#endif
  bme_ok = bme.begin(BME280_I2C_ADDR, &Wire);
  if (!bme_ok) {
    // Common alternate address
    bme_ok = bme.begin(0x77, &Wire);
  }
  if (!bme_ok) {
    i2c_scan_log();
  }
  if (bme_ok) {
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1,
                    Adafruit_BME280::SAMPLING_X1,
                    Adafruit_BME280::SAMPLING_X1,
                    Adafruit_BME280::FILTER_OFF,
                    Adafruit_BME280::STANDBY_MS_1000);
  }
  return bme_ok;
}

SensorReading sensors_read() {
  SensorReading r{};
  r.climate_ok = false;
  r.distance_ok = false;
  r.temperature_c = NAN;
  r.humidity_pct = NAN;
  r.pressure_hpa = NAN;
  r.distance_cm = -1;

  if (bme_ok) {
    bme.takeForcedMeasurement();
    r.temperature_c = bme.readTemperature();
    r.humidity_pct = bme.readHumidity();
    r.pressure_hpa = bme.readPressure() / 100.0F;
    r.climate_ok = !isnan(r.temperature_c) && !isnan(r.humidity_pct);
  }

  long d = measure_distance_cm();
  if (d >= 0 && d < 400) {
    r.distance_cm = d;
    r.distance_ok = true;
  }

  return r;
}
