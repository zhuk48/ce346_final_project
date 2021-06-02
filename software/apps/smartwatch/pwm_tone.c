#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_pwm.h"

#include "microbit_v2.h"

// PWM configuration
static const nrfx_pwm_t PWM_INST = NRFX_PWM_INSTANCE(0);

// Holds duty cycle values to trigger PWM toggle
nrf_pwm_values_common_t sequence_data[1] = {0};

// Sequence structure for configuring DMA
nrf_pwm_sequence_t pwm_sequence = {
  .values.p_common = sequence_data,
  .length = 1,
  .repeats = 0,
  .end_delay = 0,
};


static void pwm_init(void) {
  // Initialize the PWM
  // SPEAKER_OUT is the output pin, mark the others as NRFX_PWM_PIN_NOT_USED
  // Set the clock to 500 kHz, count mode to Up, and load mode to Common
  // The Countertop value doesn't matter for now. We'll set it in play_tone()
  nrfx_pwm_config_t PWM_CONFIG;
  PWM_CONFIG.output_pins[0] = SPEAKER_OUT;
  PWM_CONFIG.output_pins[1] = NRFX_PWM_PIN_NOT_USED;
  PWM_CONFIG.output_pins[2] = NRFX_PWM_PIN_NOT_USED;
  PWM_CONFIG.output_pins[3] = NRFX_PWM_PIN_NOT_USED;
  PWM_CONFIG.base_clock = NRF_PWM_CLK_500kHz;
  PWM_CONFIG.count_mode = NRF_PWM_MODE_UP;
  PWM_CONFIG.load_mode = NRF_PWM_LOAD_COMMON;
  PWM_CONFIG.step_mode = NRF_PWM_STEP_AUTO;
  PWM_CONFIG.top_value = 0;
  
  nrfx_pwm_init(&PWM_INST, &PWM_CONFIG, NULL);
}

static void play_tone(uint16_t frequency) {
  // Stop the PWM (and wait until its finished)
  nrfx_pwm_stop(&PWM_INST, true);

  // Set a countertop value based on desired tone frequency
  // You can access it as NRF_PWM0->COUNTERTOP
  NRF_PWM0->COUNTERTOP = 500000 / frequency; //base clock/freq

  // Modify the sequence data to be a 25% duty cycle
  sequence_data[0] = (500000 / frequency) * 0.25;

  // Start playback of the samples and loop indefinitely
  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}


void pwm_play(void) {
  printf("Board started!\n");

  // initialize PWM
  pwm_init();

  // G3, C4, D4, Eb4
  play_tone(196);
  nrf_delay_ms(1000);
  nrfx_pwm_stop(&PWM_INST, true);
}

