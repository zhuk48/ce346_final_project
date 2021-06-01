#include <stdbool.h>
#include <stdio.h>
#include "countdown.h"

#include "app_timer.h"

bool timer_running = false;
uint32_t time_left = 0; // in seconds
APP_TIMER_DEF(one_sec);

static void decrement(void* _unused) {
  if (time_left <= 0) {
    // do something when timer expries
    timer_running = false;
    app_timer_stop(one_sec);
    // play tone
  } else {
    time_left--;
  }
}

void countdown_init(void) {
  app_timer_create(&one_sec, APP_TIMER_MODE_REPEATED, decrement);
}

void countdown_set(void) {
  if (!timer_running) {
    if (time_left >= 1800) {
      time_left = 0;
    } else {
      time_left = time_left / 60 * 60;
      time_left = time_left + 60;
    }
  }
}

void countdown_start_stop(void) {
  timer_running = !timer_running;
  
  if (!timer_running) {
    app_timer_start(one_sec, 32768, NULL);
  } else {
    app_timer_stop(one_sec);
  }
}

uint8_t get_time_remain(void) {
  return time_left;
}

bool get_countdown_running(void) {
  return timer_running;
}
