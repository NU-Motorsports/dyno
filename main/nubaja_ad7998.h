#ifndef NUBAJA_AD7998_H_
#define NUBAJA_AD7998_H_

#include "nubaja_i2c.h"
#include "nubaja_sd.h"

// run in fast mode plus
// use cmd mode

// pn ad7998-1 with AS @ GND. this is default address.
#define ADC_SLAVE_ADDR 0x23
#define ADC_FS         3.3

#define CHANNEL_SELECTION_0                                                    \
  0b0000010101010000 // torque, belt temp, brake current, load cell
#define CHANNEL_SELECTION_1 0b0000001011100000 // temps only
#define CHANNEL_SELECTION_2 0x0                // idk
#define CHANNEL_SELECTION_3 0b0000111111110000 // all channels

#define CH1 0b00010000
#define CH2 0b00100000
#define CH3 0b01000000
#define CH4 0b10000000
#define CH5 0b0001
#define CH6 0b0010
#define CH7 0b0100
#define CH8 0b1000

// cycle timer register
#define CYCLE_TIME 0b00000100 // 0.5ms conversion interval
#define SAMPLE_DELAY_TRIAL                                                     \
  0b11000000 // bit trial and sample interval delaying mechanism implemented

/*
** CHANNEL MAPPING - MAPS ADC CHANNELS TO SIGNAL/NET NAMES
Channel 1 - torque transducer
Channel 2 - temp 3 (tbd)
Channel 3 - belt temp
Channel 4 - temp 2 (tbd)
Channel 5 - brake current
Channel 6 - temp1 (tbd)
Channel 7 - load cell
Channel 8 - throttle position
*/

// each new read function requires another call of ad7998_init with the
// corresponding channel selection

float counts_to_volts(uint16_t adc_counts);

void ad7998_config(int port_num, int slave_address, uint8_t ch_sel_h,
                   uint8_t ch_sel_l);

// read function for channel selection 0
void ad7998_read_0(int port_num, int slave_address, uint16_t *ch1,
                   uint16_t *ch3, uint16_t *ch5, uint16_t *ch7);

// read function for channel selection 1
void ad7998_read_1(int port_num, int slave_address, uint16_t *ch2,
                   uint16_t *ch3, uint16_t *ch4, uint16_t *ch6);

// read function for channel selection 3
void ad7998_read_3(int port_num, int slave_address, uint16_t *ch1,
                   uint16_t *ch2, uint16_t *ch3, uint16_t *ch4, uint16_t *ch5,
                   uint16_t *ch6, uint16_t *ch7, uint16_t *ch8);

#endif
