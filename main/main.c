#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#include "nubaja_gpio.h"
#include "nubaja_i2c.h"
#include "nubaja_as1115.h"
#include "nubaja_lsm6dsm.h"
#include "nubaja_sd.h"

#define DAQ_TIMER_GROUP       TIMER_GROUP_0  // group of daq timer
#define DAQ_TIMER_IDX         0              // index of daq timer
#define DAQ_TIMER_HZ          1000           // frequency of the daq timer in Hz

#define DISPLAY_REFRESH_RATE  4              // 4 Hz

// queue to time the daq task
xQueueHandle daq_timer_queue;
// queues to store data points
xQueueHandle logging_queue_1, logging_queue_2, current_dp_queue;

// interrupt for daq_task timer
void IRAM_ATTR daq_timer_isr(void *para)
{
  // retrieve the interrupt status and the counter value from the timer
  uint32_t intr_status = TIMERG0.int_st_timers.val;
  TIMERG0.hw_timer[DAQ_TIMER_IDX].update = 1;

  // clear the interrupt
  if (intr_status & BIT(DAQ_TIMER_IDX))
  {
    TIMERG0.int_clr_timers.t0 = 1;
  }

  // enable the alarm again, so it is triggered the next time
  TIMERG0.hw_timer[DAQ_TIMER_IDX].config.alarm_en = TIMER_ALARM_EN;

  // send the counter value to the queue to trigger the daq task
  xQueueOverwriteFromISR(daq_timer_queue, &intr_status, NULL);
}

static void daq_timer_init()
{
  // how quickly timer ticks, 80 MHz / divider
  int divider = 100;

  // select and initialize basic parameters of the timer
  timer_config_t config;
  config.divider = divider;
  config.counter_dir = TIMER_COUNT_UP;
  config.counter_en = TIMER_PAUSE;
  config.alarm_en = TIMER_ALARM_EN;
  config.intr_type = TIMER_INTR_LEVEL;
  config.auto_reload = TIMER_AUTORELOAD_EN;
  timer_init(DAQ_TIMER_GROUP, DAQ_TIMER_IDX, &config);

  // timer's counter will initially start from value below
  timer_set_counter_value(DAQ_TIMER_GROUP, DAQ_TIMER_IDX, 0x00000000ULL);

  // configure the alarm value and the interrupt on alarm
  timer_set_alarm_value(DAQ_TIMER_GROUP, DAQ_TIMER_IDX, TIMER_BASE_CLK / divider / DAQ_TIMER_HZ);
  timer_enable_intr(DAQ_TIMER_GROUP, DAQ_TIMER_IDX);
  timer_isr_register(DAQ_TIMER_GROUP, DAQ_TIMER_IDX, daq_timer_isr, NULL, ESP_INTR_FLAG_IRAM, NULL);

  timer_start(DAQ_TIMER_GROUP, DAQ_TIMER_IDX);
}

// task to run the main daq system based on a timer
static void daq_task(void *arg)
{
  // initial config

  // init imu
  i2c_master_config(PORT_0, FAST_MODE, I2C_MASTER_0_SDA_IO,I2C_MASTER_0_SCL_IO);
  LSM6DSM imu = init_lsm6dsm(PORT_0, IMU_SLAVE_ADDR);

  // init sd
  init_sd();
  xQueueHandle current_logging_queue = logging_queue_1;

  // TODO: be rid of debugging
  //int ticks = 0, last_ticks = 0;

  // button vars
  uint8_t buttons, logging_enabled, data_to_log;

  uint32_t intr_status;
  while (1)
  {
    // wait for timer alarm
    xQueueReceive(daq_timer_queue, &intr_status, portMAX_DELAY);

    // TODO: be rid of debugging
    //ticks = xTaskGetTickCount();
    //if (ticks - last_ticks != 1)
      //printf("%d, %d\n", ticks, ticks - last_ticks);
    //last_ticks = ticks;

    // get button flags
    buttons = xEventGroupGetBitsFromISR(button_eg);
    logging_enabled = buttons & ENABLE_LOGGING_BIT;
    data_to_log = buttons & DATA_TO_LOG_BIT;

    // flasher if logging
    if (logging_enabled)
      flasher_on();
    else
      flasher_off();

    // create data struct and populate with this cycle's data
    data_point dp =
    {
      .rpm = 0,    .mph = 0,    .temp = 0,
      .gyro_x = 0, .gyro_y = 0, .gyro_z = 0,
      .xl_x = 0,   .xl_y = 0,   .xl_z = 0
    };

    // imu
    imu_read_gyro_xl(&imu, &(dp.gyro_x), &(dp.gyro_y), &(dp.gyro_z),
                           &(dp.xl_x), &(dp.xl_y), &(dp.xl_z));

    // rpm, mph, temp
    xQueuePeek(rpm_queue, &(dp.rpm), 0);
    xQueuePeek(mph_queue, &(dp.mph), 0);

    // TODO: temp
    dp.temp = 0;

    // push struct to current dp queue for display
    xQueueOverwrite(current_dp_queue, &dp);

    // push struct to logging queue
    // if the queue is full, switch queues and send the full for writing to SD
    if ((logging_enabled && xQueueSend(current_logging_queue, &dp, 0) == errQUEUE_FULL)
        || (!logging_enabled && data_to_log))
    {
      printf("daq_task -- queue full, writing and switiching...\n");
      if (current_logging_queue == logging_queue_1)
      {
        current_logging_queue = logging_queue_2;
        xTaskCreatePinnedToCore(write_logging_queue_to_sd,
                "write_lq_1_sd", 2048, (void *) logging_queue_1,
                (configMAX_PRIORITIES-1), NULL, 1);
      }
      else
      {
        current_logging_queue = logging_queue_1;
        xTaskCreatePinnedToCore(write_logging_queue_to_sd,
                "write_lq_2_sd", 2048, (void *) logging_queue_2,
                (configMAX_PRIORITIES-1), NULL, 1);
      }

      // reset, though queue should be empty after writing
      // queue won't be empty if a writing task overlap occurred
      xQueueReset(current_logging_queue);

      // if writing data after logging disabled, clear DATA_TO_LOG
      // else add dp to new current buffer
      if (!logging_enabled)
        xEventGroupClearBits(button_eg, DATA_TO_LOG_BIT);
      else
        xQueueSend(current_logging_queue, &dp, 0);
    }
  }
  // per FreeRTOS, tasks MUST be deleted before breaking out of its implementing funciton
  vTaskDelete(NULL);
}

static void display_task(void *arg)
{
  // init display
  i2c_master_config(PORT_1, FAST_MODE_PLUS, I2C_MASTER_1_SDA_IO, I2C_MASTER_1_SCL_IO);
  AS1115 display = init_as1115(PORT_1, AS1115_SLAVE_ADDR);
  uint8_t display_data = 0, cycle_display = 0,
          ones, tens, hundreds, thousands;
  uint16_t disp_val = 0;

  while (1)
  {
    // sleep to lower refresh rate
    vTaskDelay(1000 / DISPLAY_REFRESH_RATE / portTICK_PERIOD_MS);

    // get button flag
    cycle_display = xEventGroupGetBitsFromISR(button_eg) & CYCLE_DISPLAY_BIT;

    if (cycle_display)
    {
      display_data = (display_data + 1) % 3;
      xEventGroupClearBits(button_eg, CYCLE_DISPLAY_BIT);
    }

    data_point dp;
    xQueuePeek(current_dp_queue, &dp, 0);
    switch (display_data)
    {
      case 0:
        disp_val = dp.rpm;
        break;
      case 1:
        disp_val = dp.mph;
        break;
      case 2:
        disp_val = dp.temp;
        break;
      default:
        printf("display_data -- invalid value\n");
    }
    ones = disp_val % 10;
    tens = (disp_val /= 10) % 10;
    hundreds = (disp_val /= 10) % 10;
    thousands = (disp_val /= 10) % 10;
    display_4_digits(&display, thousands, hundreds, tens, ones);
  }
  // per FreeRTOS, tasks MUST be deleted before breaking out of its implementing funciton
  vTaskDelete(NULL);
}


// initialize the daq timer and start the daq task
void app_main()
{
  daq_timer_queue = xQueueCreate(1, sizeof(uint32_t));

  logging_queue_1 = xQueueCreate(LOGGING_QUEUE_SIZE, sizeof(data_point));
  logging_queue_2 = xQueueCreate(LOGGING_QUEUE_SIZE, sizeof(data_point));

  // setup current data point queue
  current_dp_queue = xQueueCreate(1, sizeof(data_point));
  data_point dp =
  {
    .rpm = 0,    .mph = 0,    .temp = 0,
    .gyro_x = 0, .gyro_y = 0, .gyro_z = 0,
    .xl_x = 0,   .xl_y = 0,   .xl_z = 0
  };
  xQueueOverwrite(current_dp_queue, &dp);

  // rpm, mph, logging toggle, display cycle, and flasher are gpio
  configure_gpio();

  // start daq timer and tasks
  daq_timer_init();

  xTaskCreatePinnedToCore(daq_task, "daq_task", 2048,
                          NULL, (configMAX_PRIORITIES-1), NULL, 0);
  xTaskCreatePinnedToCore(display_task, "display_task", 2048,
                          NULL, (configMAX_PRIORITIES-2), NULL, 1);
}
