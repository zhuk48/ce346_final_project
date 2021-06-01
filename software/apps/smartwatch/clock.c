#include <stdbool.h>
#include <stdio.h>

#include "clock.h"

// global variable to keep track of current time
time_struct t = {0,0,0};

// initializes the clock
void clock_init(uint8_t hour, uint8_t min, uint8_t sec) {
  
  t.h = hour;
  t.m = min;
  t.s = sec;
  
  APP_TIMER_DEF(one_sec);
  app_timer_create(&one_sec, APP_TIMER_MODE_REPEATED, clock_inc);
  app_timer_start(one_sec, 32768, NULL);
}

void clock_set(bool dir) {
  if (dir == true) {
    if (t.m == 59) {
      t.m = 0;
    } else {
      t.m++;
    }
  } else if (dir == false) {
    if (t.h == 12) {
      t.h = 1;
    } else {
      t.h++;
    }
  }
}

// function to be called every second to "increment" clock
// this should be called once every second from main
static void clock_inc(void* _unused) {
  t.s++;
  if (t.s > 59) {
    t.s = 0;
    t.m++;
    if (t.m > 59) {
      t.m = 0;
      t.h++;
      if (t.h > 11) {
        t.h = 1;
      }
    }
  }
}

// returns the time in the form of a time_t struct
time_struct get_time(void) {
  return t;
}
