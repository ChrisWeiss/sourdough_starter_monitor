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

// VL53L1X default 7-bit address (many breakouts stay at 0x29)
#ifndef VL53_I2C_ADDR
#define VL53_I2C_ADDR 0x29
#endif

// Keep lid-to-dough clearance healthy (prefer ≥ ~30–40 mm with window/foam).
#ifndef VL53_MIN_RANGE_MM
#define VL53_MIN_RANGE_MM 20
#endif

#ifndef VL53_MAX_RANGE_MM
#define VL53_MAX_RANGE_MM 2000
#endif

// Timing budget for a single ranging (ms). Valid: 15,20,33,50,100,200,500.
// Longer = less noise; 200 ms is a good jar-rise tradeoff.
#ifndef VL53_TIMING_BUDGET_MS
#define VL53_TIMING_BUDGET_MS 200
#endif

// ROI width/height in SPADs (4–16). Narrower FOV reduces jar-wall bounce.
#ifndef VL53_ROI_XY
#define VL53_ROI_XY 8
#endif

// Median of N continuous ranges per reported sample.
#ifndef VL53_SAMPLE_COUNT
#define VL53_SAMPLE_COUNT 5
#endif

// Legacy alias used by older notes (microseconds).
#ifndef VL53_TIMING_BUDGET_US
#define VL53_TIMING_BUDGET_US (VL53_TIMING_BUDGET_MS * 1000UL)
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
