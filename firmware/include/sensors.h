#pragma once

#include <stdint.h>

struct SensorReading {
  float temperature_c;
  float humidity_pct;
  float pressure_hpa;
  long distance_mm;
  bool climate_ok;   // temperature available (BME280 or BMP280)
  bool humidity_ok;  // humidity available (BME280 only)
  bool distance_ok;
};

bool sensors_begin();
SensorReading sensors_read();
