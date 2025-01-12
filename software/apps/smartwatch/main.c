// I2C sensors app
//
// Read from I2C accelerometer/magnetometer on the Microbit

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "nrf_delay.h"
#include "nrf_twi_mngr.h"

#include "microbit_v2.h"
#include "pedometer.h"
#include "clock.h"
#include "disp.h"
#include "touch_sensor.h"
#include "countdown.h"

// Global variables
NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);

#define BUFFER_SIZE 100

//lsm303agr_measurement_t sample[BUFFER_SIZE] = {0};

int main(void) {
  printf("Board started!\n");

  // Initialize I2C peripheral and driver
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = I2C_SCL;
  i2c_config.sda = I2C_SDA;
  i2c_config.interrupt_priority = 5;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);

  // Initialize the LSM303AGR accelerometer/magnetometer sensor
  //lsm303agr_init(&twi_mngr_instance);
  virtual_timer_init();
  app_timer_init();

  disp_init();
  clock_init(12, 00, 00);
  touch_init();
  countdown_init();
  pedometer_init(&twi_mngr_instance, &i2c_config);
    
  //collect_data();
  
  
  
  
  // Loop forever
  while (1) {
    nrf_delay_ms(1000);
    //printf("%d\n", get_steps());
  }
}

