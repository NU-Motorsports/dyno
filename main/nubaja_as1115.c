#include "nubaja_as1115.h"

#define DIGIT_0 0x1
#define DIGIT_1 0x2
#define DIGIT_2 0x3
#define DIGIT_3 0x4

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

void display_one_digit(AS1115_t *dev, uint8_t digit, uint8_t value) {
  i2c_write_byte(dev->port_num, dev->port_num, digit, value);
}

void display_4_digits(AS1115_t *dev, uint8_t digit_0, uint8_t digit_1,
                      uint8_t digit_2, uint8_t digit_3) {
  i2c_write_4_bytes(dev->port_num, dev->slave_address, DIGIT_0, digit_0,
                    digit_1, digit_2, digit_3);
}

void display_disable(AS1115_t *dev) {
  // sets shutdown register for normal operation
  i2c_write_byte(dev->port_num, dev->slave_address, 0xc, 0x80);
}
