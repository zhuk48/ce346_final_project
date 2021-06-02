// Virtual timer implementation

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "nrf.h"

#include "virtual_timer.h"
#include "virtual_timer_linked_list.h"

#define CLOCK_MAX_VALUE 4294967000

// This is the interrupt handler that fires on a compare event
void TIMER4_IRQHandler(void) {
  // This should always be the first line of the interrupt handler!
  // It clears the event so that it doesn't happen again
  NRF_TIMER4->EVENTS_COMPARE[0] = 0;

  // Place your interrupt handler code here
  //printf("timer fired\n");
  
  __disable_irq();
  node_t* my_timer_node = list_remove_first();
  __enable_irq();
  
  my_timer_node->my_callback_variable();
  virtual_timer_start(1000, my_timer_node->my_callback_variable);
  
  free(my_timer_node);
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
  
  NRF_TIMER4->INTENSET = 1 << 16; // enables interrupts on CC[2]
  NVIC_EnableIRQ(TIMER4_IRQn);
  
  NVIC_SetPriority(TIMER4_IRQn, 4);
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
static uint32_t timer_start(uint32_t microseconds, virtual_timer_callback_t cb, bool repeated) {
  volatile uint32_t curr_time = read_timer();
  
  int set_time = 0;
  
  if ((curr_time + microseconds) < CLOCK_MAX_VALUE) {
  	set_time = curr_time + microseconds; //sets timer firing time
  }
  else {
  	set_time = curr_time + microseconds - CLOCK_MAX_VALUE; //overflow
  }
  	 
  NRF_TIMER4->CC[0] = set_time;

  node_t* timer_pointer = malloc(sizeof(node_t));
  timer_pointer->timer_value = set_time;
  timer_pointer->microseconds = microseconds;
  timer_pointer->repeated = repeated;
  timer_pointer->my_callback_variable = cb;
  
  __disable_irq();
  list_insert_sorted(timer_pointer);
  __enable_irq();
  
  // Return a unique timer ID. (hint: What is guaranteed unique about the timer you have created?)
  return (uint32_t)timer_pointer;
}

// You do not need to modify this function
// Instead, implement timer_start
uint32_t virtual_timer_start(uint32_t microseconds, virtual_timer_callback_t cb) {
  return timer_start(microseconds, cb, false);
}

// You do not need to modify this function
// Instead, implement timer_start
uint32_t virtual_timer_start_repeated(uint32_t microseconds, virtual_timer_callback_t cb) {
  return timer_start(microseconds, cb, true);
}

// Remove a timer by ID.
// Make sure you don't cause linked list consistency issues!
// Do not forget to free removed timers.
void virtual_timer_cancel(uint32_t timer_id) {
}

