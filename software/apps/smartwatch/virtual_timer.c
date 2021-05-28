// Virtual timer implementation

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "nrf.h"

#include "virtual_timer.h"


// This is the interrupt handler that fires on a compare event
void TIMER4_IRQHandler(void) {
  // This should always be the first line of the interrupt handler!
  // It clears the event so that it doesn't happen again
  NRF_TIMER4->EVENTS_COMPARE[0] = 0;

  // Place your interrupt handler code here
  printf("Timer Fired!\n");
}

// Read the current value of the timer counter
uint32_t read_timer(void) {
  NRF_TIMER4->TASKS_CAPTURE[1] = 1;
  volatile uint32_t time = NRF_TIMER4->CC[1];
  // Should return the value of the internal counter for TIMER4
  return time;
}

// Initialize TIMER4 as a free running timer
// 1) Set to be a 32 bit timer
// 2) Set to count at 1MHz
// 3) Enable the timer peripheral interrupt (look carefully at the INTENSET register!)
//    Skip this for the first part of the lab.
// 4) Clear the timer
// 5) Start the timer
void virtual_timer_init(void) {
  NRF_TIMER4->BITMODE = 3;      // 32 bit timer
  NRF_TIMER4->PRESCALER = 4;    // 16/ 2^4 = 1
  NRF_TIMER4->TASKS_START = 1;  // starts timer
  NRF_TIMER4->MODE = 0;         // timer mode
  
  NRF_TIMER4->INTENSET = 1 << 16; // enables interrupts on CC[0]
  NVIC_EnableIRQ(TIMER4_IRQn);
}

// Start a timer. This function is called for both one-shot and repeated timers
// To start a timer:
// 1) Create a linked list node (This requires `malloc()`. Don't forget to free later)
// 2) Setup the linked list node with the correct information
//      - You will need to modify the `node_t` struct in "virtual_timer_linked_list.h"!
// 3) Place the node in the linked list
// 4) Setup the compare register so that the timer fires at the right time
// 5) Return a timer ID
//
// Your implementation will also have to take special precautions to make sure that
//  - You do not miss any timers
//  - You do not cause consistency issues in the linked list (hint: you may need the `__disable_irq()` and `__enable_irq()` functions).
//
// Follow the lab manual and start with simple cases first, building complexity and
// testing it over time.
static void timer_start(uint32_t microseconds) {
  volatile uint32_t curr_time = read_timer();
  int set_time = curr_time + microseconds; 
  NRF_TIMER4->CC[0] = set_time;
  // Return a unique timer ID. (hint: What is guaranteed unique about the timer you have created?)
}
