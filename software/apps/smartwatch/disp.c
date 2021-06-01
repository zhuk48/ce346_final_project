#include "nrf_gpio.h"
#include "disp.h"
#include "microbit_v2.h"

#include "clock.h"
#include "touch_sensor.h"
#include "pedometer.h"
#include "countdown.h"

uint8_t curr_state = 0; // keeps track of current state

bool led_state[5][5] = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
  };
uint8_t col = 0;
// state[0][i] would be column 1 

  APP_TIMER_DEF(LEDtimer);
  APP_TIMER_DEF(clock_timer);
  APP_TIMER_DEF(ped_timer);
  APP_TIMER_DEF(cd_timer);
  APP_TIMER_DEF(state_machine_timer);

void disp_init(void){
  // initialize row pins
  nrf_gpio_pin_dir_set(LED_ROW1, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_ROW2, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_ROW3, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_ROW4, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_ROW5, NRF_GPIO_PIN_DIR_OUTPUT);

  // initialize col pins
  nrf_gpio_pin_dir_set(LED_COL1, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_COL2, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_COL3, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_COL4, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_COL5, NRF_GPIO_PIN_DIR_OUTPUT);

  // set default values for pins
  nrf_gpio_pin_clear(LED_ROW1);
  nrf_gpio_pin_clear(LED_ROW2);
  nrf_gpio_pin_clear(LED_ROW3);
  nrf_gpio_pin_clear(LED_ROW4);
  nrf_gpio_pin_clear(LED_ROW5);
  nrf_gpio_pin_clear(LED_COL1);
  nrf_gpio_pin_clear(LED_COL2);
  nrf_gpio_pin_clear(LED_COL3);
  nrf_gpio_pin_clear(LED_COL4);
  nrf_gpio_pin_clear(LED_COL5);
  
  // init buttons
  nrf_gpio_pin_dir_set(BTN_A, NRF_GPIO_PIN_DIR_INPUT); // button A
  nrf_gpio_pin_dir_set(BTN_B, NRF_GPIO_PIN_DIR_INPUT); // button B
  
  app_timer_create(&LEDtimer, APP_TIMER_MODE_REPEATED, disp_show);
  app_timer_create(&clock_timer, APP_TIMER_MODE_REPEATED, disp_time);
  app_timer_create(&state_machine_timer, APP_TIMER_MODE_REPEATED, check_state);
  app_timer_create(&ped_timer, APP_TIMER_MODE_REPEATED, disp_steps);
  app_timer_create(&cd_timer, APP_TIMER_MODE_REPEATED, disp_cd);
  app_timer_start(LEDtimer, 65, NULL);
  app_timer_start(state_machine_timer, 8198, NULL);
}

static void check_state(void* _unused) {
  led_state[curr_state][0] = 1;
  curr_state = get_state();
  
  switch(curr_state) {
    case(0):
      app_timer_stop(ped_timer);
      app_timer_stop(cd_timer);
      app_timer_start(clock_timer, 16384, NULL);
      if (nrf_gpio_pin_read(BTN_A) == 0) {
        clock_set(false);
      }
      if (nrf_gpio_pin_read(BTN_B) == 0) {
        clock_set(true);
      }
      break;
      
    case(1):
      app_timer_stop(clock_timer);
      app_timer_stop(cd_timer);
      app_timer_start(ped_timer, 16384, NULL);
      if (nrf_gpio_pin_read(BTN_A) == 0 || nrf_gpio_pin_read(BTN_B) == 0) {
        clear_steps();
      }
      break;
      
    case(2):
      app_timer_stop(clock_timer);
      app_timer_stop(ped_timer);
      app_timer_start(cd_timer, 16384, NULL);
      if (nrf_gpio_pin_read(BTN_A) == 0) {
        countdown_set();
      }
      if (nrf_gpio_pin_read(BTN_B) == 0) {
        countdown_start_stop();
      }
      break;
  }
    
}

static void disp_show(void* _unused) {
  // this function cycles through the rows and sets each row correctly
  switch(col) {
    case(0):
      nrf_gpio_pin_set(LED_COL5);
      nrf_gpio_pin_clear(LED_COL1);
      break;
    
    case(1):
      nrf_gpio_pin_set(LED_COL1);
      nrf_gpio_pin_clear(LED_COL2);
      break;
      
    case(2):
      nrf_gpio_pin_set(LED_COL2);
      nrf_gpio_pin_clear(LED_COL3);
      break;
      
    case(3):
      nrf_gpio_pin_set(LED_COL3);
      nrf_gpio_pin_clear(LED_COL4);
      break;
      
    case(4):
      nrf_gpio_pin_set(LED_COL4);
      nrf_gpio_pin_clear(LED_COL5);
      break;
  }
  
  nrf_gpio_pin_write(LED_ROW1, led_state[col][0]);
  nrf_gpio_pin_write(LED_ROW2, led_state[col][1]);
  nrf_gpio_pin_write(LED_ROW3, led_state[col][2]);
  nrf_gpio_pin_write(LED_ROW4, led_state[col][3]);
  nrf_gpio_pin_write(LED_ROW5, led_state[col][4]);
  
  // increments to next col
  if (col == 4) {
    col = 0;
  } else {
    col = col + 1;
  }
}

static void disp_time(void* _unused) {
  time_struct t = get_time();
  uint8_t h_tens = t.h / 10;
  uint8_t h_ones = t.h % 10;
  uint8_t m_tens = t.m / 10;
  uint8_t m_ones = t.m % 10;
  num_to_led(h_tens, 0);
  num_to_led(h_ones, 1);
  num_to_led(m_tens, 3);
  num_to_led(m_ones, 4);
  led_state[2][2] = !(t.s % 2);
  led_state[2][0] = 0;
  led_state[2][1] = 0;
  led_state[2][4] = !(t.s % 2);
  led_state[2][3] = 0;
}

static void disp_steps(void* _unused){
  int steps = get_steps();
  uint8_t steps_tth = steps / 10000;
  uint8_t steps_th = steps / 1000 % 10;
  uint8_t steps_h = steps / 100 % 10;
  uint8_t steps_t = steps / 10 % 10;
  uint8_t steps_o = steps % 10;
  num_to_led(steps_tth, 0);
  num_to_led(steps_th, 1);
  num_to_led(steps_h, 2);
  num_to_led(steps_t, 3);
  num_to_led(steps_o, 4);
}

static void disp_cd(void* _unused) {
  uint32_t rem = get_time_remain();
  uint8_t rem_m_tens = (rem / 60) / 10;
  uint8_t rem_m_ones = (rem / 60) % 10;
  uint8_t rem_s_tens = (rem % 60) / 10;
  uint8_t rem_s_ones = (rem % 60) % 10;
  num_to_led(rem_m_tens, 0);
  num_to_led(rem_m_ones, 1);
  num_to_led(0, 2);
  num_to_led(rem_s_tens, 3);
  num_to_led(rem_s_ones, 4);
}
 
static void num_to_led(uint8_t num, uint8_t col_index) {
  for (int j = 0; j < 5; j++) {
   led_state[col_index][4-j] = num % 2;
   num = num /2;
  }
}


