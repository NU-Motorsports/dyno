#ifndef NUBAJA_AS1115_H_
#define NUBAJA_AS1115_H_

#include "nubaja_i2c.h"

#define AS1115_SLAVE_ADDR 0x3

typedef struct AS1115 {
  int port_num;
  int slave_address;
} AS1115_t;

// create and configure the AS1115_t 7-segment display
AS1115_t init_as1115(int port_num, int slave_address);

/*
 * write to one of the 8 digit registers of the AS1115_t
 * the device is configured to use BCD encoding, meaning values of 0-9 and
 * -,E,H,L,P are possible
 */
void display_one_digit(AS1115_t *dev, uint8_t digit, uint8_t value);

// write 4 digits to an AS1115_t display
void display_4_digits(AS1115_t *dev, uint8_t digit_0, uint8_t digit_1,
                      uint8_t digit_2, uint8_t digit_3);

// disable an AS1115_t display
void display_disable(AS1115_t *dev);

#endif // NUBAJA_AS1115_H_
