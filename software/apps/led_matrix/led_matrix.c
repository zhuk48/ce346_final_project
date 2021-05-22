// LED Matrix Driver
// Displays characters on the LED matrix

#include <stdbool.h>
#include <stdio.h>

#include "nrf_gpio.h"
#include "app_timer.h"

#include "led_matrix.h"
#include "font.h"
#include "microbit_v2.h"

// global 2D array to keep track of state of each LED
bool state[5][5] = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
  };
// int 0 to 4 to keep track of which row is currently being displayed
int row = 0;
// following global variables needed to cycle through letters
int ascii_arr[2048];
int text_length = 0; // length of text being displayed
int text_index = 0; // index of which letter is being displayed
APP_TIMER_DEF(texttimer);

void led_matrix_init(void) {
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
  
  // initialize timer(s) (Part 3 and onwards)
  APP_TIMER_DEF(LEDtimer);
  app_timer_init();
  app_timer_create(&LEDtimer, APP_TIMER_MODE_REPEATED, led_matrix_disp);
  app_timer_create(&texttimer, APP_TIMER_MODE_REPEATED, cycle_text);
  app_timer_start(LEDtimer, 65, NULL);

}

// helper function to convert character from font.c to LED matrix
void font_to_led(int ascii) {
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      int ascii_row = font[ascii][i];
      ascii_row = ascii_row & (1 << j);
      ascii_row = ascii_row >> j;
      state[i][j] = ascii_row;
    }
  }
}

static void led_matrix_disp(void* _unused) {
  // this function cycles through the rows and sets each row correctly
  switch(row) {
    case(0):
      nrf_gpio_pin_clear(LED_ROW5);
      nrf_gpio_pin_set(LED_ROW1);
      break;
    
    case(1):
      nrf_gpio_pin_clear(LED_ROW1);
      nrf_gpio_pin_set(LED_ROW2);
      break;
      
    case(2):
      nrf_gpio_pin_clear(LED_ROW2);
      nrf_gpio_pin_set(LED_ROW3);
      break;
      
    case(3):
      nrf_gpio_pin_clear(LED_ROW3);
      nrf_gpio_pin_set(LED_ROW4);
      break;
      
    case(4):
      nrf_gpio_pin_clear(LED_ROW4);
      nrf_gpio_pin_set(LED_ROW5);
      break;
  }
  
  nrf_gpio_pin_write(LED_COL1, !state[row][0]);
  nrf_gpio_pin_write(LED_COL2, !state[row][1]);
  nrf_gpio_pin_write(LED_COL3, !state[row][2]);
  nrf_gpio_pin_write(LED_COL4, !state[row][3]);
  nrf_gpio_pin_write(LED_COL5, !state[row][4]);
  
  // increments to next row
  if (row == 4) {
    row = 0;
  } else {
    row = row + 1;
  }
}

void disp_text(const char *text) {
  text_length = strlen(text);
  for (size_t i = 0; i < strlen(text); i++) {
     //ascii_arr[i] = (int)text[i];
     //printf("%c\n", text[i]);
  }
  app_timer_start(texttimer, 32768/2, NULL);
}

static void cycle_text(void* _unused) {
  if (text_index <= text_length) {
    font_to_led(ascii_arr[text_index]);
    text_index = text_index + 1;
  } else {
    app_timer_stop(texttimer);
  }
}
