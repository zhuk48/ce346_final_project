// GPIO app
//
// Uses MMIO to control GPIO pins

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"

#include "microbit_v2.h"
#include "gpio.h"

int main(void) {
  printf("Board started!\n");

  // Step 2:
  // Control LED with raw MMIO
  // Microphone LED is P0.20 and active high
  // Add code here
  //*(uint32_t*)(0x50000514) = 1 << 20;
  //*(uint32_t*)(0x50000504) = 1 << 20;
  // loop forever
  
  // DEBUGGIN CODE:
  // volatile gpio_reg_t* GPIO0 = (gpio_reg_t*)(0x50000000);
  // volatile gpio_reg_t* GPIO1 = (gpio_reg_t*)(0x50000300);
  // printf("%p\n", &(GPIO0->PIN_IN));  
  gpio_clear(20);
  bool LED_status = false;

  printf("Looping\n");
  while (1) {

    // Step 4:
    // Control LED with buttons
    // Button A is P0.14 and active low
    // Button B is P0.23 and active low
    // Add code here
    gpio_config(20, GPIO_OUTPUT);
    gpio_config(14, GPIO_INPUT); // button A
    gpio_config(23, GPIO_INPUT); // button B
    
    bool on_press = gpio_read(14);
    printf("%d\n", on_press);
    bool off_press = gpio_read(23);
    
    if (LED_status == false && on_press == false) {
    	gpio_set(20);
    	LED_status = true;
    }
    
    if (LED_status == true && off_press == false) {
    	gpio_clear(20);
    	LED_status = false;
    }
	
    nrf_delay_ms(100);
  }
}

