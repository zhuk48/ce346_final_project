#include <stdbool.h>
#include <time.h>

#include "clock.h"

// global variable to keep track of current time
time_struct t = {0,0,0};

// initializes the clock
void clock_init(uint8_t hour, uint8_t min, uint8_t sec) {
  t.h = hour;
  t.m = min;
  t.s = sec;
}

// function to be called every second to "increment" clock
// this should be called once every second from main
void clock_inc(void) {
  t.s = t.s++;
  if (t.s > 59) {
    t.s = 0;
    t.m++;
    if (t.m > 59) {
      t.m = 0;
      t.h++;
      if (t.h > 11) {
        t.h = 0;
      }
    }
  }
}

// returns the time in the form of a time_t struct
time_struct get_time(void) {
  return t;
}
