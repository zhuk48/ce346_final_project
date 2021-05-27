// LSM303AGR accelerometer and magnetometer

#pragma once

#include "nrf_twi_mngr.h"
#include "lsm303agr.h"


typedef struct {
  volatile uint8_t X_U;
  volatile uint8_t X_L;
  volatile uint8_t Y_U;
  volatile uint8_t Y_L;
  volatile uint8_t Z_U;
  volatile uint8_t Z_L;
} accel_raw;


// Function prototypes

// Initialize and configure the LSM303AGR accelerometer/magnetometer
//
// i2c - pointer to already initialized and enabled twim instance
// config - pointer to already initialized twi driver config
void pedometer_init(const nrf_twi_mngr_t* i2c, const nrf_drv_twi_config_t* config);


// returns number of steps and clears number of steps
int get_steps(void);

void clear_steps(void);

// schedules a read of the accelerometer
void schedule(void);

//get the buffer
lsm303agr_measurement_t * return_buf(void);

void raw_2_measurement(void);



