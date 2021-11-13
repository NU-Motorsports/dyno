#ifndef NUBAJA_SD_H_
#define NUBAJA_SD_H_

#define __STDC_FORMAT_MACROS

#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "esp_vfs_fat.h"
#include "freertos/semphr.h"
#include "sdmmc_cmd.h"
#include <cstdint>

#define SD_MISO 19
#define SD_MOSI 18
#define SD_CLK  14
#define SD_CS   15

#define LOGGING_QUEUE_SIZE 1000 // data logging queue size

typedef struct data_point {
  uint16_t prim_rpm, sec_rpm;
  uint16_t torque, temp3, belt_temp, temp2, temp1, load_cell, tps, i_brake;
  float i_sp, tps_sp;
} data_point_t;

void print_data_point(data_point_t *dp);

void write_logging_queue_to_sd(void *arg);

void init_sd();

#endif // NUBAJA_SD_H_
