#include "nrf_gpio.h"
#include "disp.h"
#include "microbit_v2.h"
#include "clock.h"

enum modes {CLOCK, STEP_COUNTER, STOPWATCH}; // defines modes of watch
enum modes curr_state; // keeps track of current state

bool state[5][5] = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
  };
uint8_t col = 0;
// state[0][i] would be column 1 

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
  
  APP_TIMER_DEF(LEDtimer);
  APP_TIMER_DEF(clock_timer);
  app_timer_create(&LEDtimer, APP_TIMER_MODE_REPEATED, disp_show);
  app_timer_create(&clock_timer, APP_TIMER_MODE_REPEATED, disp_time);
  app_timer_start(LEDtimer, 100, NULL);
  app_timer_start(clock_timer, 16384, NULL);
  
  //curr_state = CLOCK;
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
  
  nrf_gpio_pin_write(LED_ROW1, state[col][0]);
  nrf_gpio_pin_write(LED_ROW2, state[col][1]);
  nrf_gpio_pin_write(LED_ROW3, state[col][2]);
  nrf_gpio_pin_write(LED_ROW4, state[col][3]);
  nrf_gpio_pin_write(LED_ROW5, state[col][4]);
  
  // increments to next col
  if (col == 4) {
    col = 0;
  } else {
    col = col + 1;
  }
}

static void disp_time(void) {
  time_struct t = get_time();
  
  uint8_t h_tens = t.h / 10;
  uint8_t h_ones = t.h % 10;
  uint8_t m_tens = t.m / 10;
  uint8_t m_ones = t.m % 10;
  time_to_led(h_tens, 0);
  time_to_led(h_ones, 1);
  time_to_led(m_tens, 3);
  time_to_led(m_ones, 4);
  state[2][2] = !(t.s % 2);
  state[2][4] = !(t.s % 2);
}
 
static void time_to_led(uint8_t num, uint8_t col_index) {
  for (int j = 0; j < 5; j++) {
   state[col_index][4-j] = num % 2;
   num = num /2;
  }
}


