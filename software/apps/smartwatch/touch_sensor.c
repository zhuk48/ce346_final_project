#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "touch_sensor.h"

#include "app_timer.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "microbit_v2.h"
#include "nrfx_gpiote.h"

#include "virtual_timer.h"

APP_TIMER_DEF(touch_timer);

uint8_t sm_state;

uint32_t start0 = 0;
uint32_t start1 = 0;
uint32_t start2 = 0;
uint32_t finish0 = 0;
uint32_t finish1 = 0;
uint32_t finish2 = 0;

static void button_0(void) {
  finish0 = read_timer();
  nrfx_gpiote_in_event_disable(TOUCH_RING0);
  uint32_t diff = finish0 - start0;
  
  if (diff > 1000) {
    sm_state = 0;
  }
}

static void button_1(void) {
  finish1 = read_timer();
  nrfx_gpiote_in_event_disable(TOUCH_RING1);
  uint32_t diff = finish1 - start1;
  
  if (diff > 1000) {
    sm_state = 1;
  }
}

static void button_2(void) {
  finish2 = read_timer();
  nrfx_gpiote_in_event_disable(TOUCH_RING2);
  uint32_t diff = finish2 - start2;
  
  
  if (diff > 1000) {
    sm_state = 2;
  }
}

static void check_touch(void* _unused) {
  nrf_gpio_pin_dir_set(TOUCH_RING0, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_write(TOUCH_RING0, 0);
  nrf_gpio_pin_dir_set(TOUCH_RING0, NRF_GPIO_PIN_DIR_INPUT);	
  nrfx_gpiote_in_event_enable(TOUCH_RING0, true);
  start0 = read_timer();
  
  nrf_gpio_pin_dir_set(TOUCH_RING1, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_write(TOUCH_RING1, 0);
  nrf_gpio_pin_dir_set(TOUCH_RING1, NRF_GPIO_PIN_DIR_INPUT);	
  nrfx_gpiote_in_event_enable(TOUCH_RING1, true);
  start1 = read_timer();
  
  nrf_gpio_pin_dir_set(TOUCH_RING2, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_write(TOUCH_RING2, 0);
  nrf_gpio_pin_dir_set(TOUCH_RING2, NRF_GPIO_PIN_DIR_INPUT);	
  nrfx_gpiote_in_event_enable(TOUCH_RING2, true);
  start2 = read_timer();
}

void touch_init(void){
  // Initialize touch sensors.
  nrf_gpio_pin_dir_set(TOUCH_LOGO, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(TOUCH_RING0, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(TOUCH_RING1, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(TOUCH_RING2, NRF_GPIO_PIN_DIR_OUTPUT);
  
  nrfx_gpiote_init();
  nrfx_gpiote_in_config_t gpiote_config = {                                                   
    .sense = NRF_GPIOTE_POLARITY_LOTOHI,            
    .pull = NRF_GPIO_PIN_NOPULL,                    
    .is_watcher = false,                            
    .hi_accuracy = true,                         
    .skip_gpio_setup = false,                       
  };
  nrfx_gpiote_in_init(TOUCH_RING0, &gpiote_config, button_0);
  nrfx_gpiote_in_init(TOUCH_RING1, &gpiote_config, button_1);
  nrfx_gpiote_in_init(TOUCH_RING2, &gpiote_config, button_2);
  nrfx_gpiote_in_event_disable(TOUCH_RING0);
  nrfx_gpiote_in_event_disable(TOUCH_RING1);
  nrfx_gpiote_in_event_disable(TOUCH_RING2);
  
  app_timer_create(&touch_timer, APP_TIMER_MODE_REPEATED, check_touch);
  app_timer_start(touch_timer, 8192, NULL);
  sm_state = 0;
}

uint8_t get_state(void) {
  return sm_state;
}
