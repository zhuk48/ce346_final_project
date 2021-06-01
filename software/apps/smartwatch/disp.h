#pragma once

enum modes{CLOCK, PEDOMETER, STOPWATCH}; // defines modes of watch

void disp_init(void);
static void disp_show(void* _unused);
static void disp_time(void* _unused);
static void disp_steps(void* _unused);
static void disp_cd(void* _unused);
static void num_to_led(uint8_t num, uint8_t col_index);
static void check_state(void* _unused);
