#ifndef NUBAJA_PROJ_VARS_H_
#define NUBAJA_PROJ_VARS_H_

#include "driver/timer.h"

// timing
#define DAQ_TIMER_GROUP   TIMER_GROUP_0 // group of daq timer
constexpr timer_idx_t DAQ_TIMER_IDX = TIMER_0;           // index of daq timer
#define DAQ_TIMER_HZ      1             // frequency of the daq timer in Hz
#define DAQ_TIMER_DIVIDER 100

// ctrl
#define LAUNCH_THRESHOLD 50  // % of throttle needed for launch
#define BSIZE            100 // test length

// adc scales, offsets (physical quantity = scale*volts + offset)
#define TORQUE_SCALE  15.6
#define TORQUE_OFFSET 0

#define THERM_SCALE  44.5
#define THERM_OFFSET 14.3

#define BELT_TEMP_SCALE  0.359
#define BELT_TEMP_OFFSET -307.4

#define I_BRAKE_SCALE  1
#define I_BRAKE_OFFSET 0.05

#define LOAD_CELL_SCALE  30.3
#define LOAD_CELL_OFFSET -50

#define BREAK_IN_RPM 1800

// fault thresholds
#define MAX_I_BRAKE     2.4 // amps
#define MAX_BELT_TEMP   150 // deg C
#define MAX_CVT_AMBIENT 100 // deg C
#define MAX_BRAKE_TEMP  100 // deg C
#define I_BRAKE_MAX     3.6

// PIDs
#define KP                 0
#define KI                 0.5
#define KD                 0
#define BRAKE_WINDUP_GUARD 10
#define BRAKE_OUTPUT_MAX   100

// struct for consolidating various flags, key quantities, etc
typedef struct control {
  // flags
  int en_eng;
  int eng;
  int run;
  int num_profile;
  int test_chosen;
  int idx;
  int en_log;

  // quantities
  float i_brake_amps;
  float i_brake_duty;
  float brake_temp;
  float belt_temp;
} control_t;

// profiles
extern float i_sp_accel_launch[BSIZE];

extern float tps_sp_accel_launch[BSIZE];

extern float i_sp_accel[BSIZE];

extern float tps_sp_accel[BSIZE];

extern float i_sp_hill[BSIZE];

extern float tps_sp_hill[BSIZE];

extern float i_sp_test[BSIZE];

extern float tps_sp_test[BSIZE];

extern float i_sp_demo[BSIZE];

extern float tps_sp_demo[BSIZE];

#endif
