#pragma once

#include <stdint.h>

void power_deep_sleep_seconds(uint32_t seconds);
uint8_t power_battery_percent();

#if defined(BOARD_NRF52840)
// True System OFF — only returns after an external wake (GPIO/NFC/reset).
void power_system_off();
#endif
