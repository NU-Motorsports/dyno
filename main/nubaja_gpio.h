#ifndef NUBAJA_GPIO_H_
#define NUBAJA_GPIO_H_

#include "driver/gpio.h"
#include "driver/timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

extern xQueueHandle primary_rpm_queue;   // queue for engine rpm values
extern xQueueHandle secondary_rpm_queue; // queue for wheel speed values

extern double last_prim_rpm_time;
extern double last_sec_rpm_time;

void flasher_on();

void flasher_off();

void ebrake_set();

void ebrake_release();

void engine_off();

void engine_on();

// configure gpio pins for input and ISRs, and the flasher pin for output
void configure_gpio();

void rpm_log(QueueHandle_t queue, uint16_t *val);

#endif // NUBAJA_GPIO_H_
