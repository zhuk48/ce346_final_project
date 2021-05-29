#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_timer.h"

#include "microbit_v2.h"
#include "virtual_timer.h"

APP_TIMER_DEF(touch_timer);

static void touch_init(void){
  // Initialize touch sensors.
  nrf_gpio_pin_dir_set(TOUCH_LOGO, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(TOUCH_RING0, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(TOUCH_RING1, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(TOUCH_RING2, NRF_GPIO_PIN_DIR_OUTPUT);
}

static void timers_init(void){
  //app_timer_init();
  //app_timer_create(&touch_timer, APP_TIMER_MODE_REPEATED, check_touch);

}
