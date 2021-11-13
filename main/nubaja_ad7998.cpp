#include "nubaja_ad7998.h"

// register addresses
#define CONFIGURATION 0b01110010

// configuration register
#define ALERT_BUSY          0x0    // pin does not provide any interrupt signal
#define ALERT_EN            0x0    // pin does not provide any interrupt signal
#define FLTR                0b1000 // filtering disabled on SDA/SCL
#define ALERT_BUSY_POLARITY 0x0    // alert/busy output is active low

// modified since AD7998 registers are 12 bits (4 MSBs unused here)
#define AD7998_BITMASK 0b0000111111111111

// command mode
#define CMD_MODE                                                               \
  0b01110000 // sequence of channels specified in the config register

float counts_to_volts(uint16_t adc_counts) {
  float v = ((float)adc_counts / (float)4096) * (float)ADC_FS;
  return v;
}

void ad7998_config(int port_num, int slave_address, uint8_t ch_sel_h,
                   uint8_t ch_sel_l) {
  uint8_t addr_ptr = CONFIGURATION;
  uint8_t data_h = ch_sel_h;
  uint8_t data_l =
      (ch_sel_l | FLTR | ALERT_EN | ALERT_BUSY | ALERT_BUSY_POLARITY);
  i2c_write_2_byte(port_num, slave_address, addr_ptr, data_h, data_l);
  printf("ad7998_config -- configuring success\n");
}

void ad7998_read_0(int port_num, int slave_address, uint16_t *ch1,
                   uint16_t *ch3, uint16_t *ch5, uint16_t *ch7) {
  uint8_t addr_pointer = CMD_MODE; // cmd mode
  i2c_read_2_bytes_4(port_num, slave_address, addr_pointer, ch1, ch3, ch5, ch7);
  *ch1 = (*ch1 & AD7998_BITMASK);
  *ch3 = (*ch3 & AD7998_BITMASK);
  *ch5 = (*ch5 & AD7998_BITMASK);
  *ch7 = (*ch7 & AD7998_BITMASK);
}

void ad7998_read_1(int port_num, int slave_address, uint16_t *ch2,
                   uint16_t *ch3, uint16_t *ch4, uint16_t *ch6) {
  uint8_t addr_pointer = CMD_MODE; // cmd mode
  i2c_read_2_bytes_4(port_num, slave_address, addr_pointer, ch2, ch3, ch4, ch6);
  *ch2 = (*ch2 & AD7998_BITMASK);
  *ch3 = (*ch3 & AD7998_BITMASK);
  *ch4 = (*ch4 & AD7998_BITMASK);
  *ch6 = (*ch6 & AD7998_BITMASK);
}

void ad7998_read_3(int port_num, int slave_address, uint16_t *ch1,
                   uint16_t *ch2, uint16_t *ch3, uint16_t *ch4, uint16_t *ch5,
                   uint16_t *ch6, uint16_t *ch7, uint16_t *ch8) {
  uint8_t addr_pointer = CMD_MODE; // cmd mode

  i2c_read_2_bytes_8(port_num, slave_address, addr_pointer, ch1, ch2, ch3, ch4,
                     ch5, ch6, ch7, ch8);
  *ch1 = (*ch1 & AD7998_BITMASK);
  *ch2 = (*ch2 & AD7998_BITMASK);
  *ch3 = (*ch3 & AD7998_BITMASK);
  *ch4 = (*ch4 & AD7998_BITMASK);
  *ch5 = (*ch5 & AD7998_BITMASK);
  *ch6 = (*ch6 & AD7998_BITMASK);
  *ch7 = (*ch7 & AD7998_BITMASK);
  *ch8 = (*ch8 & AD7998_BITMASK);
}
