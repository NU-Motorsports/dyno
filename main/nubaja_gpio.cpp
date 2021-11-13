#include "nubaja_gpio.h"

#define GPIO_DECLARE(NAME, NUMBER)                                             \
  constexpr gpio_num_t NAME##_GPIO = GPIO_NUM_##NUMBER;                        \
  constexpr uint64_t NAME##_GPIO_SEL = GPIO_SEL_##NUMBER

/// engine rpm measurement
GPIO_DECLARE(PRIMARY, 26);
/// CVT secondary rpm measurement
GPIO_DECLARE(SECONDARY, 27);
/// e-brake release solenoid
///***DIFFERENT FROM PCB RIGHT NOW - NEEDS REWORK**
GPIO_DECLARE(SOLENOID, 16);
/// kill switch relay control
GPIO_DECLARE(KILL, 33);
/// flashing indicator relay control
GPIO_DECLARE(FLASHER, 32);
#define GPIO_INPUT_PIN_SEL (PRIMARY_GPIO_SEL | SECONDARY_GPIO_SEL)
#define GPIO_OUTPUT_PIN_SEL                                                    \
  (SOLENOID_GPIO_SEL | FLASHER_GPIO_SEL | KILL_GPIO_SEL)

#define RPM_TIMER_GROUP TIMER_GROUP_1          // group of speed timer
constexpr timer_idx_t RPM_TIMER_IDX = TIMER_0; // index of speed timer
#define RPM_TIMER_DIVIDER 100                  // speed timer prescale divider

// cut off wacky high errors. max engine rpm 3800
#define MAX_PRIMARY_RPM 4200
// cut off wacky high errors. max sec rpm 3800 / 0.9 = ~4200
#define MAX_SECONDARY_RPM 4500

xQueueHandle primary_rpm_queue;   // queue for engine rpm values
xQueueHandle secondary_rpm_queue; // queue for wheel speed values

double last_prim_rpm_time = 0;
double last_sec_rpm_time = 0;

void flasher_on() { gpio_set_level(FLASHER_GPIO, 1); }

void flasher_off() { gpio_set_level(FLASHER_GPIO, 0); }

void ebrake_set() { gpio_set_level(SOLENOID_GPIO, 0); }

void ebrake_release() { gpio_set_level(SOLENOID_GPIO, 1); }

void engine_off() { gpio_set_level(KILL_GPIO, 1); }

void engine_on() { gpio_set_level(KILL_GPIO, 0); }

static void secondary_isr_handler(void *arg) {
  double time;
  timer_get_counter_time_sec(RPM_TIMER_GROUP, RPM_TIMER_IDX, &time);
  uint16_t rpm = 60.0 / (time - last_sec_rpm_time);

  if (rpm <= MAX_SECONDARY_RPM) {
    xQueueOverwriteFromISR(secondary_rpm_queue, &rpm, NULL);
  }

  last_sec_rpm_time = time;
}

static void primary_isr_handler(void *arg) {
  double time;
  timer_get_counter_time_sec(RPM_TIMER_GROUP, RPM_TIMER_IDX, &time);
  uint16_t rpm = 60.0 / (time - last_prim_rpm_time);

  if (rpm <= MAX_PRIMARY_RPM) {
    xQueueOverwriteFromISR(primary_rpm_queue, &rpm, NULL);
  }

  last_prim_rpm_time = time;
}

static void speed_timer_init() {
  // select and initialize basic parameters of the timer
  timer_config_t config;
  config.divider = RPM_TIMER_DIVIDER;
  config.counter_dir = TIMER_COUNT_UP;
  config.counter_en = TIMER_PAUSE;
  config.alarm_en = TIMER_ALARM_DIS;
  config.intr_type = TIMER_INTR_LEVEL;
  config.auto_reload = TIMER_AUTORELOAD_DIS;
  timer_init(RPM_TIMER_GROUP, RPM_TIMER_IDX, &config);

  // timer's counter will initially start from value below
  timer_set_counter_value(RPM_TIMER_GROUP, RPM_TIMER_IDX, 0x00000000ULL);

  timer_start(RPM_TIMER_GROUP, RPM_TIMER_IDX);
}

void configure_gpio() {
  // setup timer and queues for speeds
  speed_timer_init();
  primary_rpm_queue = xQueueCreate(1, sizeof(uint16_t));
  secondary_rpm_queue = xQueueCreate(1, sizeof(uint16_t));

  // config rising-edge interrupt GPIO pins (primray, secondary RPM)
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_POSEDGE; // interrupt of rising edge
  io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL; // bit mask of the pins
  io_conf.mode = GPIO_MODE_INPUT;            // set as input mode
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  gpio_config(&io_conf);

  // congifure GPIO outputs
  io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL; // bit mask of the pins
  io_conf.intr_type = GPIO_INTR_DISABLE;  // interrupt of rising edge
  io_conf.mode = GPIO_MODE_OUTPUT;            // set as input mode
  gpio_config(&io_conf);

  // ISRs
  gpio_install_isr_service(0); // install gpio isr service
  gpio_isr_handler_add(PRIMARY_GPIO, primary_isr_handler, NULL);
  gpio_isr_handler_add(SECONDARY_GPIO, secondary_isr_handler, NULL);

  // GPIOs
  gpio_set_direction(FLASHER_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_direction(KILL_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_direction(SOLENOID_GPIO, GPIO_MODE_OUTPUT);

  flasher_off();
  engine_off();
  ebrake_set();
}

void rpm_log(QueueHandle_t queue, uint16_t *val) {
  // uint16_t rst = 0;
  xQueuePeek(queue, val, 0);
  // xQueueOverwrite(queue, &rst);  // overwrite queue to 0 to avoid repetitive
  // values
}
