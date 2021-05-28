#pragma once

#include "nrf.h"

// Type for the function pointer to call when the timer expires
typedef void (*virtual_timer_callback_t)(void);

// Read the current value of the hardware timer counter
// Returns the counter value
uint32_t read_timer(void);

// Initialize the timer peripheral
void virtual_timer_init(void);

// Start a one-shot timer that calls <cb> <microseconds> in the future
// Returns a unique timer_id
void virtual_timer_start(uint32_t microseconds);
