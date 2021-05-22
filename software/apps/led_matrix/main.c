// LED Matrix app
//
// Display messages on the LED matrix

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf_delay.h"

#include "led_matrix.h"
#include "microbit_v2.h"

int main(void) {
  printf("Board started!\n");
  
  // initialize LED matrix driver
  led_matrix_init();

  // call other functions here
  char text1[7] = "ABCDEFG";
  //disp_text(text1);

  // loop forever
  while (1) {
    nrf_delay_ms(1000);
  }
}

