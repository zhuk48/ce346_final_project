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

float force[BUFFER_SIZE] = {0};
lsm303agr_measurement_t samples[BUFFER_SIZE] = {0};
float run_avg = 0;
float n = 0;
//nrf_twi_mngr_transfer_t const transfer[] = {0};

int i = 0;
int start_calc = 0;

int steps; // number of steps

APP_TIMER_DEF(read_accel);
APP_TIMER_DEF(count_steps);

// Pointer to an initialized I2C instance to use for transactions
static const nrf_twi_mngr_t* i2c_manager = NULL;
static const nrf_drv_twi_config_t* i2c_config = NULL;

float last_5(int start) {
  float result = force[start];
  int j = start - 1;
  int end = 0;
  if (start < 5) {
    end = BUFFER_SIZE + (start - 5);
  }
  else {
    end = start - 5;
  }
  
  while (j > end) {
    result = (result + force[j]);
    if (j == 0) {
      j = BUFFER_SIZE;
    }
    j--;
  }
  return result/5.0;
}

//calculate steps taken
void calc_steps(void) {
  for(int j = start_calc; j < i; j++) {
    if ((last_5(j) > 1.75) && (last_5(j-1) <= 1.75)) {
      steps++;
    }
  } 
}

//calculate force from measurements and counts steps
void measurement_2_force(void* _unused) {
  for(int j = start_calc; j < i; j++) {
    float Xf = samples[j].x_axis;
    float Yf = samples[j].y_axis;
    float Zf = samples[j].z_axis;
    
    force[j] = sqrt((Xf*Xf) + (Yf*Yf) + (Zf*Zf));
  }

  calc_steps();
  start_calc = i;
}

//read in from accelerometer
void read_in(void* _unused) {
  samples[i] = lsm303agr_read_accelerometer();

  if (i < BUFFER_SIZE-1) {
    i++;
  }
  else {
    i = 0;
  }
}


void clear_steps(void) {
  steps = 0;
}

int get_steps(void) {
  // something lol
  return steps;
}

void collect_data(void) {
  //Pause timers
  app_timer_stop(read_accel);
  app_timer_stop(count_steps);
  
  printf("starting... \n");

  //collect a full buffer of data
  for (int j = 0; j < BUFFER_SIZE; j++) {
    samples[j] = lsm303agr_read_accelerometer();
    nrf_delay_ms(25);  
  }
  
  printf("finished collecting data.\n");

  //convert to force and count steps
  i = BUFFER_SIZE;
  start_calc = 1;

  measurement_2_force(NULL);
  //calc_steps();

  i = 0;
  start_calc = 0;
  
  //print buffer
  for (int j = 0; j < BUFFER_SIZE; j++) {
    printf("%f %f %f\n", samples[j].x_axis, samples[j].y_axis, samples[j].z_axis);
  }
  printf("%d steps\n", steps);
  
  //restart timers
  app_timer_start(read_accel, 32768/40, NULL);
  app_timer_start(count_steps, 32768/4, NULL); 
}


void pedometer_init(const nrf_twi_mngr_t* i2c, const nrf_drv_twi_config_t* config) {
  i2c_manager = i2c;
  i2c_config = config;
  lsm303agr_init(i2c);

  app_timer_create(&read_accel, APP_TIMER_MODE_REPEATED, read_in);
  app_timer_create(&count_steps, APP_TIMER_MODE_REPEATED, measurement_2_force);
  app_timer_start(read_accel, 32768/40, NULL);
  app_timer_start(count_steps, 32768/4, NULL);  
}
