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
void i2c_master_config(int port_num, int clk, int sda, int scl) {
  printf("i2c_master_config -- configuring port %d\n", port_num);
  i2c_config_t conf;
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = sda;
  conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
  conf.scl_io_num = scl;
  conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
  conf.clk_flags = 0;
  conf.master.clk_speed = clk;
  i2c_param_config(port_num, &conf);
  i2c_driver_install(port_num, conf.mode, I2C_MASTER_RX_BUF_DISABLE,
                     I2C_MASTER_TX_BUF_DISABLE, 0);
  printf("i2c_master_config -- configuring success\n");
}

// write a single byte of data to a register using I2C protocol
int i2c_write_byte(int port_num, uint8_t slave_address, uint8_t reg,
                   uint8_t data) {
  int ret;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_address << 1) | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg, ACK);
  i2c_master_write_byte(cmd, data, ACK);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(port_num, cmd, I2C_TASK_LENGTH / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);

  if (ret != ESP_OK) {
    printf(
        "i2c_write_byte -- failure on port: %d, slave: %d, reg: %d, data: %d\n",
        port_num, slave_address, reg, data);
    return I2C_WRITE_FAILED;
  } else
    return I2C_SUCCESS;
}

// write two byte of data to a register using I2C protocol
int i2c_write_2_byte(int port_num, uint8_t slave_address, uint8_t reg,
                     uint8_t data_h, uint8_t data_l) {
  int ret;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_address << 1) | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg, ACK);
  i2c_master_write_byte(cmd, data_h, ACK);
  i2c_master_write_byte(cmd, data_l, NACK);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(port_num, cmd, I2C_TASK_LENGTH / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);

  if (ret != ESP_OK) {
    printf("i2c_write_2_byte -- failure on port: %d, slave: %d, reg: %d, data: "
           "%d\n",
           port_num, slave_address, reg, data_h);
    return I2C_WRITE_FAILED;
  } else
    return I2C_SUCCESS;
}

// write four bytes of data to four consecutive registers using I2C protocol
int i2c_write_4_bytes(int port_num, uint8_t slave_address, uint8_t reg,
                      uint8_t data_0, uint8_t data_1, uint8_t data_2,
                      uint8_t data_3) {

  int ret;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_address << 1) | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg, ACK);
  i2c_master_write_byte(cmd, data_0, ACK);
  i2c_master_write_byte(cmd, data_1, ACK);
  i2c_master_write_byte(cmd, data_2, ACK);
  i2c_master_write_byte(cmd, data_3, NACK);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(port_num, cmd, I2C_TASK_LENGTH / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);

  if (ret != ESP_OK) {
    printf("i2c_write_4_bytes -- failure on port: %d, slave: %d, reg: %d\n",
           port_num, slave_address, reg);
    return I2C_WRITE_FAILED;
  } else
    return I2C_SUCCESS;
}

// read one byte from the register of an I2C device
int i2c_read_byte(int port_num, uint8_t slave_address, int reg, uint8_t *data) {
  int ret;

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_address << 1) | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg, ACK);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_address << 1) | READ_BIT, ACK_CHECK_EN);
  i2c_master_read_byte(cmd, data, NACK);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(port_num, cmd, I2C_TASK_LENGTH / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);

  if (ret != ESP_OK) {
    printf("i2c_read_byte -- failure on port: %d, slave: %d, reg: %d\n",
           port_num, slave_address, reg);
    return I2C_READ_FAILED;
  } else
    return I2C_SUCCESS;
}

// read two consecutive bytes from the register of an I2C device
int i2c_read_2_bytes(int port_num, uint8_t slave_address, int reg,
                     uint16_t *data) {
  int ret;
  uint8_t data_h, data_l;

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_address << 1) | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg, ACK);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_address << 1) | READ_BIT, ACK_CHECK_EN);
  i2c_master_read_byte(cmd, &data_h, ACK);
  i2c_master_read_byte(cmd, &data_l, NACK);
  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(port_num, cmd, I2C_TASK_LENGTH / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);

  *data = (data_h << 8 | data_l);

  if (ret != ESP_OK) {
    printf("i2c_read_2_bytes -- failure on port: %d, slave: %d, reg: %d\n",
           port_num, slave_address, reg);
    return I2C_READ_FAILED;
  } else
    return I2C_SUCCESS;
}

// read four consecutive pairs of 2 bytes from an I2C device
int i2c_read_2_bytes_4(int port_num, uint8_t slave_address, int reg,
                       uint16_t *data_0, uint16_t *data_1, uint16_t *data_2,
                       uint16_t *data_3) {
  int ret;
  uint8_t data_0_h, data_0_l, data_1_h, data_1_l, data_2_h, data_2_l, data_3_h,
      data_3_l;

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_address << 1) | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg, ACK);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_address << 1) | READ_BIT, ACK_CHECK_EN);

  i2c_master_read_byte(cmd, &data_0_h, ACK);
  i2c_master_read_byte(cmd, &data_0_l, ACK);

  i2c_master_read_byte(cmd, &data_1_h, ACK);
  i2c_master_read_byte(cmd, &data_1_l, ACK);

  i2c_master_read_byte(cmd, &data_2_h, ACK);
  i2c_master_read_byte(cmd, &data_2_l, ACK);

  i2c_master_read_byte(cmd, &data_3_h, ACK);
  i2c_master_read_byte(cmd, &data_3_l, NACK);

  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(port_num, cmd, I2C_TASK_LENGTH / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);

  *data_0 = (data_0_h << 8 | data_0_l);
  *data_1 = (data_1_h << 8 | data_1_l);
  *data_2 = (data_2_h << 8 | data_2_l);
  *data_3 = (data_3_h << 8 | data_3_l);

  if (ret != ESP_OK) {
    printf("i2c_read_2_bytes_4 -- failure on port: %d, slave: %d, reg: %d\n",
           port_num, slave_address, reg);
    return I2C_READ_FAILED;
  } else
    return I2C_SUCCESS;
}

// read eight consecutive pairs of 2 bytes from an I2C device
int i2c_read_2_bytes_8(int port_num, uint8_t slave_address, int reg,
                       uint16_t *data_0, uint16_t *data_1, uint16_t *data_2,
                       uint16_t *data_3, uint16_t *data_4, uint16_t *data_5,
                       uint16_t *data_6, uint16_t *data_7) {
  int ret;
  uint8_t data_0_h, data_0_l, data_1_h, data_1_l, data_2_h, data_2_l, data_3_h,
      data_3_l;
  uint8_t data_4_h, data_4_l, data_5_h, data_5_l, data_6_h, data_6_l, data_7_h,
      data_7_l;

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_address << 1) | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg, ACK);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (slave_address << 1) | READ_BIT, ACK_CHECK_EN);

  i2c_master_read_byte(cmd, &data_0_h, ACK);
  i2c_master_read_byte(cmd, &data_0_l, ACK);

  i2c_master_read_byte(cmd, &data_1_h, ACK);
  i2c_master_read_byte(cmd, &data_1_l, ACK);

  i2c_master_read_byte(cmd, &data_2_h, ACK);
  i2c_master_read_byte(cmd, &data_2_l, ACK);

  i2c_master_read_byte(cmd, &data_3_h, ACK);
  i2c_master_read_byte(cmd, &data_3_l, ACK);

  i2c_master_read_byte(cmd, &data_4_h, ACK);
  i2c_master_read_byte(cmd, &data_4_l, ACK);

  i2c_master_read_byte(cmd, &data_5_h, ACK);
  i2c_master_read_byte(cmd, &data_5_l, ACK);

  i2c_master_read_byte(cmd, &data_6_h, ACK);
  i2c_master_read_byte(cmd, &data_6_l, ACK);

  i2c_master_read_byte(cmd, &data_7_h, ACK);
  i2c_master_read_byte(cmd, &data_7_l, NACK);

  i2c_master_stop(cmd);
  ret = i2c_master_cmd_begin(port_num, cmd, I2C_TASK_LENGTH / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);

  *data_0 = (data_0_h << 8 | data_0_l);
  *data_1 = (data_1_h << 8 | data_1_l);
  *data_2 = (data_2_h << 8 | data_2_l);
  *data_3 = (data_3_h << 8 | data_3_l);
  *data_4 = (data_4_h << 8 | data_4_l);
  *data_5 = (data_5_h << 8 | data_5_l);
  *data_6 = (data_6_h << 8 | data_6_l);
  *data_7 = (data_7_h << 8 | data_7_l);

  if (ret != ESP_OK) {
    printf("i2c_read_2_bytes_8 -- failure on port: %d, slave: %d, reg: %d\n",
           port_num, slave_address, reg);
    return I2C_READ_FAILED;
  } else {
    return I2C_SUCCESS;
  }
}

#endif // NUBAJA_I2C_H_
