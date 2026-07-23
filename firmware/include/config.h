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

// HC-SR04: speed of sound approx — microseconds to cm
#ifndef US_TIMEOUT_US
#define US_TIMEOUT_US 30000UL
#endif

#ifndef BME280_I2C_ADDR
#define BME280_I2C_ADDR 0x76
#endif

// LiPo calibration (mV at the cell, after undoing the sense divider)
#ifndef VBAT_EMPTY_MV
#define VBAT_EMPTY_MV 3300.0f
#endif
#ifndef VBAT_FULL_MV
#define VBAT_FULL_MV 4200.0f
#endif

// Optional high-side / low-side MOSFET gate for HC-SR04 VCC (active HIGH = powered).
// Leave undefined to keep the module always powered (USB prototype).
// #define PIN_US_PWR 17
