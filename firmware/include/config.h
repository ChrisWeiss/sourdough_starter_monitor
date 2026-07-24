#pragma once

#ifndef SAMPLE_INTERVAL_SEC
#define SAMPLE_INTERVAL_SEC 90
#endif

#ifndef ADVERTISE_DURATION_MS
#define ADVERTISE_DURATION_MS 1500
#endif

#ifndef DEVICE_NAME
#define DEVICE_NAME "sourdough"
#endif

#ifndef BME280_I2C_ADDR
#define BME280_I2C_ADDR 0x76
#endif

// VL53L3CX default 7-bit address (many breakouts stay at 0x29)
#ifndef VL53_I2C_ADDR
#define VL53_I2C_ADDR 0x29
#endif

// Datasheet minimum ranging distance is 10 mm. Keep lid-to-dough above this
// (recommend ≥ 30 mm mechanical clearance including cover glass / window).
#ifndef VL53_MIN_RANGE_MM
#define VL53_MIN_RANGE_MM 10
#endif

#ifndef VL53_MAX_RANGE_MM
#define VL53_MAX_RANGE_MM 2000
#endif

// Timing budget for a single ranging (microseconds). 50–100 ms is typical.
#ifndef VL53_TIMING_BUDGET_US
#define VL53_TIMING_BUDGET_US 50000UL
#endif

// LiPo calibration (mV at the cell, after undoing the sense divider)
#ifndef VBAT_EMPTY_MV
#define VBAT_EMPTY_MV 3300.0f
#endif
#ifndef VBAT_FULL_MV
#define VBAT_FULL_MV 4200.0f
#endif

// Optional XSHUT (active LOW shutdown). Leave undefined if breakout pulls XSHUT high.
// #define PIN_TOF_XSHUT 17
