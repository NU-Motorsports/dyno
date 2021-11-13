#ifndef NUBAJA_I2C_H_
#define NUBAJA_I2C_H_

#include "driver/i2c.h"

#define I2C_MASTER_0_SDA_IO       23        // gpio number for I2C master data
#define I2C_MASTER_0_SCL_IO       22        // gpio number for I2C master clock
#define PORT_0                    I2C_NUM_0 // I2C port number for master dev
#define I2C_MASTER_TX_BUF_DISABLE 0         // I2C master do not need buffer
#define I2C_MASTER_RX_BUF_DISABLE 0         // I2C master do not need buffer
#define NORMAL_MODE               100000    // I2C master clock frequency
#define FAST_MODE                 400000    // I2C master clock frequency
#define FAST_MODE_PLUS            1000000   // I2C master clock frequency
#define WRITE_BIT                 I2C_MASTER_WRITE // I2C master write
#define READ_BIT                  I2C_MASTER_READ  // I2C master read
#define ACK_CHECK_EN              0x1 // I2C master will check ack from slave
#define ACK_CHECK_DIS             0x0 // I2C master will not check ack from slave
#define ACK                       0x0 // I2C ack value
#define NACK                      0x1 // I2C nack value
#define DATA_LENGTH               1   // bytes
#define I2C_TASK_LENGTH           1   // ms

// return values
#define I2C_SUCCESS      0
#define I2C_WRITE_FAILED 1
#define I2C_READ_FAILED  2

// configure one I2C module for operation as an I2C master with internal pullups
// disabled
void i2c_master_config(int port_num, int clk, int sda, int scl);

// write a single byte of data to a register using I2C protocol
int i2c_write_byte(int port_num, uint8_t slave_address, uint8_t reg,
                   uint8_t data);

// write two byte of data to a register using I2C protocol
int i2c_write_2_byte(int port_num, uint8_t slave_address, uint8_t reg,
                     uint8_t data_h, uint8_t data_l);

// write four bytes of data to four consecutive registers using I2C protocol
int i2c_write_4_bytes(int port_num, uint8_t slave_address, uint8_t reg,
                      uint8_t data_0, uint8_t data_1, uint8_t data_2,
                      uint8_t data_3);

// read one byte from the register of an I2C device
int i2c_read_byte(int port_num, uint8_t slave_address, int reg, uint8_t *data);

// read two consecutive bytes from the register of an I2C device
int i2c_read_2_bytes(int port_num, uint8_t slave_address, int reg,
                     uint16_t *data);

// read four consecutive pairs of 2 bytes from an I2C device
int i2c_read_2_bytes_4(int port_num, uint8_t slave_address, int reg,
                       uint16_t *data_0, uint16_t *data_1, uint16_t *data_2,
                       uint16_t *data_3);

// read eight consecutive pairs of 2 bytes from an I2C device
int i2c_read_2_bytes_8(int port_num, uint8_t slave_address, int reg,
                       uint16_t *data_0, uint16_t *data_1, uint16_t *data_2,
                       uint16_t *data_3, uint16_t *data_4, uint16_t *data_5,
                       uint16_t *data_6, uint16_t *data_7);

#endif // NUBAJA_I2C_H_
