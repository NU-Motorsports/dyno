#ifndef NUBAJA_PWM_H_
#define NUBAJA_PWM_H_

#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

void pwm_init();

void set_throttle(float tp);

void set_brake_duty(float duty);

#endif
