// LSM303AGR driver for Microbit_v2
//
// Initializes sensor and communicates over I2C
// Capable of reading temperature, acceleration, and magnetic field strength

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "pedometer.h"
#include "nrf_delay.h"
#include "app_timer.h"
#include "lsm303agr.h"

#define BUFFER_SIZE 100

accel_raw buffer1[BUFFER_SIZE] = {{1, 1, 1, 1, 1, 1}};
lsm303agr_measurement_t samples[BUFFER_SIZE] = {0};
uint8_t i = 0;

int steps; // number of steps

//APP_TIMER_DEF(my_timer_1);
//APP_TIMER_DEF(my_timer_2);

// Pointer to an initialized I2C instance to use for transactions
static const nrf_twi_mngr_t* i2c_manager = NULL;
static const nrf_drv_twi_config_t* i2c_config = NULL;

void callback(ret_code_t unused0, void* unused1) {
  printf("4\n");
}

//Helper function to schedule a read from the accelerometer into buffer
void schedule(void) {//uint8_t i2c_addr) {
  uint8_t i2c_addr = LSM303AGR_ACC_ADDRESS;

  uint8_t X_L_reg = LSM303AGR_ACC_OUT_X_L;
  uint8_t X_U_reg = LSM303AGR_ACC_OUT_X_H;
  uint8_t Y_L_reg = LSM303AGR_ACC_OUT_Y_L;
  uint8_t Y_U_reg = LSM303AGR_ACC_OUT_Y_H;
  uint8_t Z_L_reg = LSM303AGR_ACC_OUT_Z_L;
  uint8_t Z_U_reg = LSM303AGR_ACC_OUT_Z_H;

  volatile uint8_t X_U;
  volatile uint8_t X_L;
  volatile uint8_t Y_U;
  volatile uint8_t Y_L;
  volatile uint8_t Z_U;
  volatile uint8_t Z_L;

  //read accelerometer as 1 transaction
  nrf_twi_mngr_transfer_t const read_transfer[] = { 
    NRF_TWI_MNGR_WRITE(i2c_addr, &X_L_reg, 1, NRF_TWI_MNGR_NO_STOP),
    NRF_TWI_MNGR_READ(i2c_addr, &X_L, 1, 0), 
    NRF_TWI_MNGR_WRITE(i2c_addr, &X_U_reg, 1, NRF_TWI_MNGR_NO_STOP),
    NRF_TWI_MNGR_READ(i2c_addr, &X_U, 1, 0),
    NRF_TWI_MNGR_WRITE(i2c_addr, &Y_L_reg, 1, NRF_TWI_MNGR_NO_STOP),
    NRF_TWI_MNGR_READ(i2c_addr, &Y_L, 1, 0),
    NRF_TWI_MNGR_WRITE(i2c_addr, &Y_U_reg, 1, NRF_TWI_MNGR_NO_STOP),
    NRF_TWI_MNGR_READ(i2c_addr, &Y_U, 1, 0),
    NRF_TWI_MNGR_WRITE(i2c_addr, &Z_L_reg, 1, NRF_TWI_MNGR_NO_STOP),
    NRF_TWI_MNGR_READ(i2c_addr, &Z_L, 1, 0),
    NRF_TWI_MNGR_WRITE(i2c_addr, &Z_U_reg, 1, NRF_TWI_MNGR_NO_STOP),
    NRF_TWI_MNGR_READ(i2c_addr, &Z_U, 1, 0),
  };
  printf("1\n");
  
  nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 1, NULL);
  /* nrf_twi_mngr_transaction_t transaction = { */
  /* 				      .callback = callback, */
  /* 				      .p_user_data = NULL, */
  /* 				      .p_transfers = read_transfer, */
  /* 				      .number_of_transfers = 12, */
  /* 				      .p_required_twi_cfg = NULL //i2c_config, */
  /* }; */

  printf("2\n");
  //nrf_twi_mngr_schedule(i2c_manager, &transaction);

  printf("3\n");
  accel_raw raw = {X_U, X_L, Y_U, Y_L, Z_U, Z_L};
  printf("%d%d, %d%d, %d%d\n", X_U, X_L, Y_U, Y_L, Z_U, Z_L);
  buffer1[i] = raw;
  
  if (i < BUFFER_SIZE - 1) {
    i++;
  }
  else {
    i = 0;
  }
}

void raw_2_measurement(void) {
  for(int j = 0; j < BUFFER_SIZE; j++) {
    // combine values into one int16_t
    int16_t X = ((uint16_t)buffer1[j].X_U << 8) + buffer1[j].X_L;
    int16_t Y = ((uint16_t)buffer1[j].Y_U << 8) + buffer1[j].Y_L;
    int16_t Z = ((uint16_t)buffer1[j].Z_U << 8) + buffer1[j].Z_L;
    printf("%d, %d, %d\n", X, Y, Z);
    // shift right to account for left aligned
    X = X >> 6;
    Y = Y >> 6;
    Z = Z >> 6;
    
    // multiplying by scaling factor 
    float Xf = (float)X * 3.9;
    float Yf = (float)Y * 3.9;
    float Zf = (float)Z * 3.9;
    
    // convert mg (milli-g) to g
    Xf = Xf / 1000.0;
    Yf = Yf / 1000.0;
    Zf = Zf / 1000.0;
    
    // save result to buffer
    lsm303agr_measurement_t measurement = {Xf, Yf, Zf};
    samples[j] = measurement;
  }
}

void count_steps(void) {
}

lsm303agr_measurement_t* return_buf(void) {
  return samples;
}

void clear_steps(void) {
  steps = 0;
}

int get_steps(void) {
  // something lol
  return 0;
}

void pedometer_init(const nrf_twi_mngr_t* i2c, const nrf_drv_twi_config_t* config) {
  i2c_manager = i2c;
  i2c_config = config;
  lsm303agr_init(i2c);
}
