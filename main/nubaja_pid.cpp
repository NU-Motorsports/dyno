#include "nubaja_pid.h"

void pid_update(pid_ctrl_t *pid, float sp, float pv) {
  pid->old_error = pid->error;
  pid->error = sp - pv;

  pid->P = pid->error;
  pid->I += pid->error;
  pid->D = pid->error - pid->old_error;

  if (pid->I > pid->windupGuard) {
    pid->I = pid->windupGuard;
  }

  pid->output = (pid->kp * pid->P) + (pid->ki * pid->I) + (pid->kd * pid->D);

  if (pid->output > pid->outputMax) {
    pid->output = pid->outputMax;
  }
}

void init_pid(pid_ctrl_t *pid, float kp, float ki, float kd, float windupGuard,
              float outputMax) {
  // measurements and input / output variables
  pid->error = 0;
  pid->old_error = 0;
  pid->output = 0;

  // tuning parameters
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;

  pid->P = 0;
  pid->I = 0;
  pid->D = 0;

  // bounds
  pid->windupGuard = windupGuard;
  pid->outputMax = outputMax;
}

void reset_pid(pid_ctrl_t *pid) {
  pid->error = 0;
  pid->old_error = 0;
  pid->output = 0;
  pid->P = 0;
  pid->I = 0;
  pid->D = 0;
}

float fetch_sp(int t, float *profile) {
  float sp;
  sp = profile[t];
  return sp;
}