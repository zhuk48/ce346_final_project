#pragma once
#include "app_timer.h"

// structure to keep track of time
typedef struct time_struct {
  uint8_t s; // seconds
  uint8_t m; // minutes
  uint8_t h; // hours
} time_struct;

void clock_init(uint8_t hour, uint8_t min, uint8_t sec);
static void clock_inc(void* _unused);
void clock_set(bool dir);
time_struct get_time();
