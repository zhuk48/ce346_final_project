// LSM303AGR driver for Microbit_v2
//
// Initializes sensor and communicates over I2C
// Capable of reading temperature, acceleration, and magnetic field strength

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "lsm303agr.h"
#include "nrf_delay.h"

// Pointer to an initialized I2C instance to use for transactions
static const nrf_twi_mngr_t* i2c_manager = NULL;

// Helper function to perform a 1-byte I2C read of a given register
//
// i2c_addr - address of the device to read from
// reg_addr - address of the register within the device to read
//
// returns 8-bit read value
static uint8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr) {
  uint8_t rx_buf = 0;
  nrf_twi_mngr_transfer_t const read_transfer[] = {
    NRF_TWI_MNGR_WRITE(i2c_addr, &reg_addr, 1, NRF_TWI_MNGR_NO_STOP),
    NRF_TWI_MNGR_READ(i2c_addr, &rx_buf, 1, 0)
  };
  nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 2, NULL);

  return rx_buf;
}

// Helper function to perform a 1-byte I2C write of a given register
//
// i2c_addr - address of the device to write to
// reg_addr - address of the register within the device to write
static void i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data) {
  //Note: there should only be a single two-byte transfer to be performed
  uint8_t data_in[] = {reg_addr, data};
  nrf_twi_mngr_transfer_t const write_transfer[] = {
    NRF_TWI_MNGR_WRITE(i2c_addr, data_in, 2, 0),
  };
  nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
}

// Initialize and configure the LSM303AGR accelerometer/magnetometer
//
// i2c - pointer to already initialized and enabled twim instance
void lsm303agr_init(const nrf_twi_mngr_t* i2c) {
  i2c_manager = i2c;

  // ---Initialize Accelerometer---

  // Reboot acclerometer
  i2c_reg_write(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_CTRL_REG5, 0x80);
  nrf_delay_ms(100); // needs delay to wait for reboot

  // Enable Block Data Update
  // Only updates sensor data when both halves of the data has been read
  i2c_reg_write(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_CTRL_REG4, 0x80);

  // Configure accelerometer at 100Hz, normal mode (10-bit)
  // Enable x, y and z axes
  i2c_reg_write(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_CTRL_REG1, 0x57);

  // Read WHO AM I register
  // Always returns the same value if working
  uint8_t result = i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_WHO_AM_I_REG);
  // check the result of the Accelerometer WHO AM I register
  printf("%i\n", result);
  // ---Initialize Magnetometer---

  // Reboot magnetometer
  i2c_reg_write(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_CFG_REG_A, 0x40);
  nrf_delay_ms(100); // needs delay to wait for reboot

  // Enable Block Data Update
  // Only updates sensor data when both halves of the data has been read
  i2c_reg_write(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_CFG_REG_C, 0x10);

  // Configure magnetometer at 100Hz, continuous mode
  i2c_reg_write(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_CFG_REG_A, 0x0C);

  // Read WHO AM I register
  result = i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_WHO_AM_I_REG);
  // check the result of the Magnetometer WHO AM I register
  printf("%i\n", result);

  // ---Initialize Temperature---

  // Enable temperature sensor
  i2c_reg_write(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_TEMP_CFG_REG, 0xC0);
}

// Read the internal temperature sensor
//
// Return measurement as floating point value in degrees C
float lsm303agr_read_temperature(void) {
  volatile int8_t temp_LH = i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_TEMP_L);
  volatile uint8_t temp_UH = i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_TEMP_H);
  uint16_t temp_BH = (temp_UH << 8) + temp_LH;
  float temp = temp_BH * 1.0 / 256.0 + 25.0;

  return temp;
}

lsm303agr_measurement_t lsm303agr_read_accelerometer(void) {
  volatile uint8_t X_L = i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_X_L);
  volatile uint8_t X_U = i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_X_H);
  volatile uint8_t Y_L = i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_Y_L);
  volatile uint8_t Y_U = i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_Y_H);
  volatile uint8_t Z_L = i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_Z_L);
  volatile uint8_t Z_U = i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_Z_H);
  
  // combining lower and upper half of 10 bit value
  int16_t X = ((uint16_t)X_U << 8) + X_L;
  int16_t Y = ((uint16_t)Y_U << 8) + Y_L;
  int16_t Z = ((uint16_t)Z_U << 8) + Z_L;
  
  // shift right to account for left aligned
  X = X >> 6;
  Y = Y >> 6;
  Z = Z >> 6;
  
  // multiplying by scaling factor 
  float Xf = (float)X * 3.9;
  float Yf = (float)Y * 3.9;
  float Zf = (float)Z * 3.9;

  // convert mg (milli-g) to g
  Xf = Xf / 1000;
  Yf = Yf / 1000;
  Zf = Zf / 1000;

  lsm303agr_measurement_t measurement = {Xf,Yf,Zf};
  return measurement;
}

lsm303agr_measurement_t lsm303agr_read_magnetometer(void) {
  volatile uint8_t X_L = i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_OUT_X_L_REG);
  volatile uint8_t X_U = i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_OUT_X_H_REG);
  volatile uint8_t Y_L = i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_OUT_Y_L_REG);
  volatile uint8_t Y_U = i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_OUT_Y_H_REG);
  volatile uint8_t Z_L = i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_OUT_Z_L_REG);
  volatile uint8_t Z_U = i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_OUT_Z_H_REG);
  
  // combining lower and upper half of 10 bit value
  int16_t X = ((uint16_t)X_U << 8) + X_L;
  int16_t Y = ((uint16_t)Y_U << 8) + Y_L;
  int16_t Z = ((uint16_t)Z_U << 8) + Z_L;
  
  // multiplying by scaling factor
  float Xf = (float)X * 1.5;
  float Yf = (float)Y * 1.5;
  float Zf = (float)Z * 1.5;
  
  // converting from mgauss (milli-gauss) to uT (micro-Tesla)
  Xf = Xf / 10;
  Yf = Yf / 10;
  Zf = Zf / 10;

  lsm303agr_measurement_t measurement = {Xf,Yf,Zf};
  return measurement;
}

float get_tilt_angle(void) {
  lsm303agr_measurement_t a = lsm303agr_read_accelerometer();
  float angle;
  float pi = atan(1)*4;
  angle = sqrt((a.x_axis * a.x_axis) + (a.y_axis * a.y_axis)) / a.z_axis;
  angle = atan(angle);
  angle = angle * 180 / pi;
  return angle;
}
