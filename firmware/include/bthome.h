#pragma once

#include "sensors.h"
#include <stddef.h>
#include <stdint.h>

// Pack BTHome v2 service-data payload (UUID handled by advertiser).
// Returns number of bytes written into out[], or 0 on failure.
size_t bthome_build_payload(uint8_t *out, size_t out_len, const SensorReading &r,
                            uint8_t battery_pct, uint8_t packet_id);

bool bthome_advertise_begin(const char *device_name);
bool bthome_advertise(const uint8_t *payload, size_t payload_len);
void bthome_advertise_stop();
