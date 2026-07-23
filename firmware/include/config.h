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
