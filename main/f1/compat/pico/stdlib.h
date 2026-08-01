#pragma once
// Minimal Pico SDK shim for the AKA build (F1 core only needs types + time_us_32).
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "esp_timer.h"
static inline uint32_t time_us_32(void) { return (uint32_t)esp_timer_get_time(); }
