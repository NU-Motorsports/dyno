#ifndef NUBAJA_LSM6DSM_H_
#define NUBAJA_LSM6DSM_H_

#include "nubaja_i2c.h"

#define IMU_SLAVE_ADDR 0x6a
#define OUTX_L_G       0x22
#define OUTX_H_G       0x23
#define OUTY_L_G       0x24
#define OUTY_H_G       0x25
#define OUTZ_L_G       0x26
#define OUTZ_H_G       0x27
#define OUTX_L_XL      0x28
#define OUTX_H_XL      0x29
#define OUTY_L_XL      0x2a
#define OUTY_H_XL      0x2b
#define OUTZ_L_XL      0x2c
#define OUTZ_H_XL      0x2d
#define CTRL1_XL       0x10
#define CTRL8_XL       0x17
#define CTRL2_G        0x11
#define IMU_GYRO_FS    2000 // full scale: +/- 2000 degrees / sec
#define IMU_GYRO_SCALE (IMU_GYRO_FS / 32767)
#define IMU_XL_FS      16 // full scale: +/- 16 g's
#define IMU_XL_SCALE   (IMU_XL_FS / 32767)

typedef struct LSM6DSM {
  int port_num;
  int slave_address;
} LSM6DSM_t;

// create and configure the LSM6DSM_t IMU
LSM6DSM_t init_lsm6dsm(int port_num, int slave_address);

void imu_read_gyro_xl(LSM6DSM_t *dev, int16_t *gyro_x, int16_t *gyro_y,
                      int16_t *gyro_z, int16_t *xl_x, int16_t *xl_y,
                      int16_t *xl_z);

#endif // NUBAJA_LSM6DSM_H_
