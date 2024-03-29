#ifndef NUBAJA_AS1115_H_
#define NUBAJA_AS1115_H_

#include "nubaja_i2c.h"

#define AS1115_SLAVE_ADDR 0x3
#define DIGIT_0           0x1
#define DIGIT_1           0x2
#define DIGIT_2           0x3
#define DIGIT_3           0x4

typedef struct AS1115 {
  int port_num;
  int slave_address;
} AS1115_t;

// create and configure the AS1115_t 7-segment display
AS1115_t init_as1115(int port_num, int slave_address) {
  printf("init_as1115 -- configuring an AS1115_t display driver on port %d\n",
         port_num);
  AS1115_t dev;
  dev.port_num = port_num;
  dev.slave_address = slave_address;

  // enable self addressing setting the slave-addr to 0x03
  i2c_write_byte(port_num, 0x0, 0x2d, 0x1);
  // sets shutdown register for normal operation
  i2c_write_byte(port_num, 0x0, 0xc, 0x81);
  // sets features as desired with hex-code font
  i2c_write_byte(port_num, 0x0, 0xe, 0x04);
  // sets shutdown register for normal operation
  i2c_write_byte(port_num, slave_address, 0xc, 0x81);
  // sets features as desired with hex-code font
  i2c_write_byte(port_num, slave_address, 0xe, 0x04);
  // decode mode enabled for all digits
  i2c_write_byte(port_num, slave_address, 0x9, 0xff);
  // global intensity set to 4/16
  i2c_write_byte(port_num, slave_address, 0xa, 0xf);
  // scan limit set to only display 4 digits
  i2c_write_byte(port_num, slave_address, 0xb, 0x3);

  return dev;
}

/*
 * write to one of the 8 digit registers of the AS1115_t
 * the device is configured to use BCD encoding, meaning values of 0-9 and
 * -,E,H,L,P are possible
 */
void display_one_digit(AS1115_t *dev, uint8_t digit, uint8_t value) {
  i2c_write_byte(dev->port_num, dev->port_num, digit, value);
}

// write 4 digits to an AS1115_t display
void display_4_digits(AS1115_t *dev, uint8_t digit_0, uint8_t digit_1,
                      uint8_t digit_2, uint8_t digit_3) {
  i2c_write_4_bytes(dev->port_num, dev->slave_address, DIGIT_0, digit_0,
                    digit_1, digit_2, digit_3);
}

// disable an AS1115_t display
void display_disable(AS1115_t *dev) {
  // sets shutdown register for normal operation
  i2c_write_byte(dev->port_num, dev->slave_address, 0xc, 0x80);
}

#endif // NUBAJA_AS1115_H_
