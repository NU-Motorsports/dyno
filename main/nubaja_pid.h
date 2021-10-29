#ifndef NUBAJA_PID_H_
#define NUBAJA_PID_H_

typedef struct pid_controller {
  // measurements and input / output variables
  float error;
  float old_error;
  float output;

  // tuning parameters
  float kp;
  float ki;
  float kd;

  float P;
  float I;
  float D;

  // bounds
  float windupGuard;
  float outputMax;
} pid_ctrl_t;

void pid_update(pid_ctrl_t *pid, float sp, float pv);

void init_pid(pid_ctrl_t *pid, float kp, float ki, float kd, float windupGuard,
              float outputMax);

void reset_pid(pid_ctrl_t *pid);

float fetch_sp(int t, float *profile);

#endif
