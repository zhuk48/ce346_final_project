	// Breadboard example app
//
// Read from multiple analog sensors and control an RGB LED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
// I hate dealing with strings in C
#include <stdlib.h>
#include <string.h>

#include "app_timer.h"
#include "nrf_delay.h"
#include "nrfx_saadc.h"

#include "microbit_v2.h"

// Digital outputs
// Breakout pins 13, 14, and 15
// These are GPIO pin numbers that can be used in nrf_gpio_* calls
#define LED_RED   EDGE_P15
#define LED_GREEN EDGE_P14
#define LED_BLUE  EDGE_P13

// Digital inputs
// Breakout pin 16
// These are GPIO pin numbers that can be used in nrf_gpio_* calls
#define SWITCH_IN EDGE_P16

// Analog inputs
// Breakout pins 1 and 2
// These are GPIO pin numbers that can be used in ADC configurations
#define ANALOG_TEMP_IN  NRF_SAADC_INPUT_AIN1
#define ANALOG_LIGHT_IN NRF_SAADC_INPUT_AIN2

// ADC channel configurations
// These are ADC channel numbers that can be used in ADC calls
#define ADC_TEMP_CHANNEL  0
#define ADC_LIGHT_CHANNEL 1

// Global variables
APP_TIMER_DEF(sample_timer);


// Function prototypes
static void gpio_init(void);
static void adc_init(void);
static float adc_sample_blocking(uint8_t channel);

static void sample_timer_callback(void* _unused) {
  // Do things periodically here
  
  // Reading digital input
  volatile bool switchin = nrf_gpio_pin_read(SWITCH_IN);
  //printf("%d\n", switchin);
  
  // Testing functionality of ADC
  //volatile float volts_in = adc_sample_blocking(ADC_LIGHT_CHANNEL);
  //printf("%f\n", volts_in);
  
  // Printing output of light sensor
  int volts_to_lux(char*, size_t);
  char* b = (char*) malloc(12);
  int brightness = volts_to_lux(b, 12);
  printf("%s\n", b);
  free(b);
  
  // Printing output of temp sensor (if I had one)
  float volts_to_temp(void);
  float temp = volts_to_temp();
  printf("%f\n", temp);
  
  // Controlling LED with sensors
  // RED led = light sensor
  if (brightness < 1) {
    nrf_gpio_pin_clear(LED_RED);
  } else {
    nrf_gpio_pin_set(LED_RED);
  }
  
  // GREEN led = temp sensor
  //if (temp > 25) {
  //  nrf_gpio_pin_clear(LED_GREEN);
  //} else {
  //  nrf_gpio_pin_set(LED_GREEN);
  //}
  
  // BLUE led = switch
  if (switchin == 1) {
    nrf_gpio_pin_clear(LED_BLUE);
  } else {
    nrf_gpio_pin_set(LED_BLUE);
  }
  
}

static void saadc_event_callback(nrfx_saadc_evt_t const* _unused) {
  // don't care about saadc events
  // ignore this function
}

static void gpio_init(void) {
  // Initialize output pins
  nrf_gpio_cfg_output(LED_RED);
  nrf_gpio_cfg_output(LED_BLUE);
  nrf_gpio_cfg_output(LED_GREEN);

  // Set LEDs off initially
  nrf_gpio_pin_set(LED_BLUE);
  nrf_gpio_pin_set(LED_RED);
  nrf_gpio_pin_set(LED_GREEN);

  // Initialize input pin
  nrf_gpio_cfg_input(SWITCH_IN, NRF_GPIO_PIN_NOPULL);
}

static void adc_init(void) {
  // Initialize the SAADC
  nrfx_saadc_config_t saadc_config = {
    .resolution = NRF_SAADC_RESOLUTION_12BIT,
    .oversample = NRF_SAADC_OVERSAMPLE_DISABLED,
    .interrupt_priority = 4,
    .low_power_mode = false,
  };
  ret_code_t error_code = nrfx_saadc_init(&saadc_config, saadc_event_callback);
  APP_ERROR_CHECK(error_code);

  // Initialize temperature sensor channel
  nrf_saadc_channel_config_t temp_channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(ANALOG_TEMP_IN);
  error_code = nrfx_saadc_channel_init(ADC_TEMP_CHANNEL, &temp_channel_config);
  APP_ERROR_CHECK(error_code);

  // Initialize light sensor channel
  nrf_saadc_channel_config_t light_channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(ANALOG_LIGHT_IN);
  error_code = nrfx_saadc_channel_init(ADC_LIGHT_CHANNEL, &light_channel_config);
  APP_ERROR_CHECK(error_code);
}

static float adc_sample_blocking(uint8_t channel) {
  // read ADC counts (0-4095)
  // this function blocks until the sample is ready
  int16_t adc_counts = 0;
  ret_code_t error_code = nrfx_saadc_sample_convert(channel, &adc_counts);
  APP_ERROR_CHECK(error_code);

  // convert ADC counts to volts
  // 12-bit ADC with range from 0 to 3.6 Volts
  float volts;
  float interval = 3.6 / 4095; // (2^resoltuion - 1)/ v_ref
  volts = interval * adc_counts;
  
  // this is to account for funky negative numbers?
  //if (volts < 0) {
  //	volts = 0;
  //}

  // return voltage measurement
  return volts;
}  

// Helper functions
int volts_to_lux(char *b, size_t lof) {
  // very dark is the value I read when I place my hand over the light sensor
  // dark is the value that is read when the sensor is placed away from direct light
  // bright is the value I read when the sensor is placed under my LED desk lamp
  // very bright is hte value I read when I shine my phone flashlight directly at the sensor

  float volts = adc_sample_blocking(ADC_LIGHT_CHANNEL);
  int brightness;
  if (volts < 0.29) {
    strcpy(b, "very dark");
    brightness = 0;
  } else if (volts < 1.0) {
    strcpy(b, "dark");
    brightness = 1;
  } else if (volts < 3.0) {
    strcpy(b, "bright");
    brightness = 2;
  } else {
    strcpy(b, "very dark");
    brightness = 3;
  }
  return brightness;
}

float volts_to_temp(void) {
  float volts = adc_sample_blocking(ADC_TEMP_CHANNEL);
  float temp;
  float r = 10000.0 * (3.3 / volts - 1.0);
  float r_inf = 10000.0 * exp(-3950.0/298.15);
  temp = 3950.0 / (log(r/r_inf)/log(exp(1)));
  return temp;
}

int main(void) {
  printf("Board started!\n");
  
  // initialize GPIO
  gpio_init();

  // initialize ADC
  adc_init();

  // initialize app timers
  app_timer_init();
  app_timer_create(&sample_timer, APP_TIMER_MODE_REPEATED, sample_timer_callback);

  // start timer
  // change the rate to whatever you want
  app_timer_start(sample_timer, 32768, NULL);

  // loop forever
  while (1) {
    // Don't put any code in here. Instead put periodic code in `sample_timer_callback()`
    nrf_delay_ms(100);
  }
}

