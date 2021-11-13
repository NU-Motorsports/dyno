#include "nubaja_lsm6dsm.h"

LSM6DSM_t init_lsm6dsm(int port_num, int slave_address) {
  printf("init_lsm6dsm -- configuring an LSM6DSM_t IMU on port %d\n", port_num);
  LSM6DSM_t dev;
  dev.port_num = port_num;
  dev.slave_address = slave_address;

  uint8_t ODR_XL = 0x90;

  uint8_t FS_XL = 0x04;
  uint8_t LPF1_BW_SEL = 0b0;
  uint8_t BWO_XL = 0b0;
  uint8_t CTRL1_XL_CONFIG = (ODR_XL | FS_XL | LPF1_BW_SEL | BWO_XL);

  uint8_t LPF2_XL_EN = 0b0;
  uint8_t HPCF_XL = 0b00;
  uint8_t HP_REF_MODE = 0b0;
  uint8_t INPUT_COMPOSITE = 0b0;
  uint8_t HP_SLOPE_XL_EN = 0b0;
  uint8_t LOW_PASS_ON_6D = 0b0;
  uint8_t CTRL8_XL_CONFIG = (LPF2_XL_EN | HPCF_XL | HP_REF_MODE |
                             INPUT_COMPOSITE | HP_SLOPE_XL_EN | LOW_PASS_ON_6D);

  uint8_t ODR_G = 0x80;
  uint8_t FS_G = 0x04;
  uint8_t FS_125 = 0b0;
  uint8_t CTRL2_G_CONFIG = (ODR_G | FS_G | FS_125);

  i2c_write_byte(port_num, slave_address, CTRL1_XL, CTRL1_XL_CONFIG);
  i2c_write_byte(port_num, slave_address, CTRL8_XL, CTRL8_XL_CONFIG);
  i2c_write_byte(port_num, slave_address, CTRL2_G, CTRL2_G_CONFIG);

  return dev;
}

void imu_read_gyro_xl(LSM6DSM_t *dev, int16_t *gyro_x, int16_t *gyro_y,
                      int16_t *gyro_z, int16_t *xl_x, int16_t *xl_y,
                      int16_t *xl_z) {
  // The below function does not exist. The containing function is never used.
  /*i2c_read_2_bytes_6_lh(dev->port_num, dev->slave_address, OUTX_L_G,
                        (uint16_t *)gyro_x, (uint16_t *)gyro_y,
                        (uint16_t *)gyro_z, (uint16_t *)xl_x, (uint16_t *)xl_y,
                        (uint16_t *)xl_z);*/
}