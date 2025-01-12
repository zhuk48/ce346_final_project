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
#include "lsm303agr.h"

// Global variables
NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);

int main(void) {
  printf("Board started!\n");

  // Initialize I2C peripheral and driver
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = I2C_SCL;
  i2c_config.sda = I2C_SDA;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);

  // Initialize the LSM303AGR accelerometer/magnetometer sensor
  lsm303agr_init(&twi_mngr_instance);

  // Loop forever
  while (1) {
    // Print temperature:
    volatile float t = lsm303agr_read_temperature();
    printf("Temperature: %f\n", t);
    
    // Print accelrometer reading
    lsm303agr_measurement_t acc = lsm303agr_read_accelerometer();
    printf("Accelerometer reading: (%f, %f, %f)\n", acc.x_axis, acc.y_axis, acc.z_axis);
    
    // Print magnetometer reading
    lsm303agr_measurement_t mag = lsm303agr_read_magnetometer();
    printf("Magnetometer reading: (%f, %f, %f)\n", mag.x_axis, mag.y_axis, mag.z_axis);
    
    // Print tilt angle
    float angle = get_tilt_angle();
    printf("Tilt angle: %f\n", angle);
    
    printf("\n");

    nrf_delay_ms(1000);
  }
}

