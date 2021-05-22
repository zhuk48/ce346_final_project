#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Initialize the LED matrix display
void led_matrix_init(void);

// You may need to add more functions here
void font_to_led(int ascii);
static void led_matrix_disp(void* _unused);
void disp_text(const char *text);
static void cycle_text(void* _unused);
