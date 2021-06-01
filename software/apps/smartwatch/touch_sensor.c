#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "touch_sensor.h"

#include "app_timer.h"
#include "nrf.h"
#include "nrf_delay.h"

#include "microbit_v2.h"
#include "virtual_timer.h"

APP_TIMER_DEF(touch_timer);

uint8_t sm_state;

static void button_0(void) {
  nrf_gpio_pin_dir_set(TOUCH_RING0, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_write(TOUCH_RING0, 0);
  nrf_gpio_pin_dir_set(TOUCH_RING0, NRF_GPIO_PIN_DIR_INPUT);
  
  uint32_t start = read_timer();
  bool input = nrf_gpio_pin_read(TOUCH_RING0);
  while (input != 1) {
    input = nrf_gpio_pin_read(TOUCH_RING0);
  }
  uint32_t finish = read_timer();
  uint32_t difference = finish - start;
  if (difference > 500) {
    sm_state = 0;
  }
}

static void button_1(void) {
  nrf_gpio_pin_dir_set(TOUCH_RING1, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_write(TOUCH_RING1, 0);
  nrf_gpio_pin_dir_set(TOUCH_RING1, NRF_GPIO_PIN_DIR_INPUT);
  
  uint32_t start = read_timer();
  bool input = nrf_gpio_pin_read(TOUCH_RING1);
  while (input != 1) {
    input = nrf_gpio_pin_read(TOUCH_RING1);
  }
  uint32_t finish = read_timer();
  uint32_t difference = finish - start;
  if (difference > 500) {
    sm_state = 1;
  }
}

static void button_2(void) {
  nrf_gpio_pin_dir_set(TOUCH_RING2, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_write(TOUCH_RING2, 0);
  nrf_gpio_pin_dir_set(TOUCH_RING2, NRF_GPIO_PIN_DIR_INPUT);
  
  uint32_t start = read_timer();
  bool input = nrf_gpio_pin_read(TOUCH_RING2);
  while (input != 1) {
    input = nrf_gpio_pin_read(TOUCH_RING2);
  }
  uint32_t finish = read_timer();
  uint32_t difference = finish - start;
  if (difference > 500) {
    sm_state = 2;
  }
}

static void check_touch(void* _unused) {
  button_0();

  button_1();
  button_2();
}

void touch_init(void){
  // Initialize touch sensors.
  nrf_gpio_pin_dir_set(TOUCH_LOGO, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(TOUCH_RING0, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(TOUCH_RING1, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(TOUCH_RING2, NRF_GPIO_PIN_DIR_OUTPUT);
  
  app_timer_create(&touch_timer, APP_TIMER_MODE_REPEATED, check_touch);
  app_timer_start(touch_timer, 16384, NULL);
  sm_state = 0;
  
  //NRF_GPIOTE->CONFIG[0] = 0x00012201;   //...0XX01XX100010XXXXXX01
                                        // configures GPIOTE on touch logo, port 1, pin 4, low to high mode
  //NVIC_EnableIRQ(GPIOTE_IRQn);
}

uint8_t get_state(void) {
  return sm_state;
}
