#include "gpio.h"
#include <stdio.h>

typedef struct{
  // Step 3:
  // Add register definitions here
  uint32_t unused1[321];
  uint32_t OUT;
  uint32_t OUTSET;
  uint32_t OUTCLR;
  uint32_t IN;
  uint32_t DIR;
  uint32_t DIRSET;
  uint32_t DIRCLR;
  uint32_t LATCH;
  uint32_t DETECTMODE;
  uint32_t unused2[118];
  uint32_t PIN_CNF[31];
} gpio_reg_t;

  volatile gpio_reg_t* GPIO0 = (gpio_reg_t*)(0x50000000);
  volatile gpio_reg_t* GPIO1 = (gpio_reg_t*)(0x50000300);

// Inputs: 
//  gpio_num - gpio number 0-31 OR (32 + gpio number)
//  dir - gpio direction (INPUT, OUTPUT)
void gpio_config(uint8_t gpio_num, gpio_direction_t dir) {
  // Implement me
  // Hint: Use proper PIN_CNF instead of DIR
  if (gpio_num >= 32){
  	gpio_num = gpio_num - 32;
  	if (dir == GPIO_OUTPUT) {
  		GPIO1->PIN_CNF[gpio_num] = 3; // ...00011b
  	} else {
  		GPIO1->PIN_CNF[gpio_num] = 0;
  	}
  } else {
  	if (dir == GPIO_OUTPUT) {
  		GPIO0->PIN_CNF[gpio_num] = 3;
  	} else {
  		GPIO0->PIN_CNF[gpio_num] = 0;
  	}
  }
}

// Inputs: 
//  gpio_num - gpio number 0-31 OR (32 + gpio number)
void gpio_set(uint8_t gpio_num) {
  // Implement me
  if (gpio_num >=32) {
  	gpio_num = gpio_num - 32;
  	GPIO1->OUTSET = 1 << gpio_num;
  } else {
  	GPIO0->OUTSET = 1 << gpio_num;
  }
}

// Inputs: 
//  gpio_num - gpio number 0-31 OR (32 + gpio number)
void gpio_clear(uint8_t gpio_num) {
  // Implement me
  if (gpio_num >=32) {
  	gpio_num = gpio_num - 32;
  	GPIO1->OUTCLR = 1 << gpio_num;
  } else {
  	GPIO0->OUTCLR = 1 << gpio_num;
  }
}

// Inputs: 
//  gpio_num - gpio number 0-31 OR (32 + gpio number)
// Output:
//  bool - pin state (true == high)
bool gpio_read(uint8_t gpio_num) {
	uint32_t data_in;
	if (gpio_num >= 32) {
		gpio_num = gpio_num - 32;
		data_in = GPIO1->IN;
	} else {
		data_in = GPIO0->IN;
	}
	data_in = data_in & (1 << gpio_num);
	data_in = data_in >> gpio_num;
	if (data_in == 1) {
		return true;
	} else {
		return false;
	}
}
