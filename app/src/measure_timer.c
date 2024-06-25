#ifdef CONFIG_ADC_LTC2450

/*
 * LTC2450-1 has fixed conversion time.
 * In order not blocking the default measure polling (in system queue),
 * measurement reading is better implemented in separate timer and work
 * queue.
 *
 */

#include "common.h"

#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#define ADC_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(lltc_ltc2450)
#define ENCODER_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(gpio_encoder)

/* should be more frequent than other peripheral's status update */
#define MEASUREMENT_PERIOD 500

/* hardcoded mapping from encoder ch to internal register addr (fixed in PCB design) */
const static uint16_t measure_ch_map[] = {REG_PVV_CH1, REG_PVI_CH1, REG_PVI_CH0,
                                          REG_PVV_CH0, REG_PVI_CH2, REG_PVV_CH2,
                                          REG_PVV_CH3, REG_PVI_CH3};

/* set the current adc channel */
const static struct gpio_dt_spec encoder_gpio[] = {
    GPIO_DT_SPEC_GET_BY_IDX(ENCODER_NODE, gpios, 0),
    GPIO_DT_SPEC_GET_BY_IDX(ENCODER_NODE, gpios, 1),
    GPIO_DT_SPEC_GET_BY_IDX(ENCODER_NODE, gpios, 2)};

#define SET_ENCODER_ADDR(index, ch)                                            \
  gpio_pin_configure_dt(encoder_gpio + index, (ch >> index) & BIT_MASK(1)            \
                                            ? GPIO_OUTPUT_ACTIVE               \
                                            : GPIO_OUTPUT_INACTIVE)
inline static void choose_adc_ch(uint8_t ch) {
  SET_ENCODER_ADDR(0, ch);
  SET_ENCODER_ADDR(1, ch);
  SET_ENCODER_ADDR(2, ch);
  return;
}

/* read one channel's current measurement */
const static struct device *adc_dev = DEVICE_DT_GET(ADC_NODE);

// [todo] rtn err code?
inline static void read_adc_ch(int ch) {
  // select ch
  choose_adc_ch(ch);

  // read current conversion
  uint16_t buf;
  struct adc_sequence sequence = {
      .buffer = &buf,
      .buffer_size = sizeof(buf),
  };
  int err = adc_read(adc_dev, &sequence);
  if (err < 0) {
    printk("Could not read current conversion value (%d)\n", err);
    return;
  }

  // update corresponding regs
  holding_reg[measure_ch_map[ch]]->value = buf;

  return;
}

/* work handler for measurement timer */
static void measure_work_handler(struct k_work *work) {

  for (int ch = 0; ch < ARRAY_SIZE(measure_ch_map); ch++) {
    read_adc_ch(ch);
  }

  return;
}

K_WORK_DEFINE(measure_work, measure_work_handler);

static void measure_timer_callback(struct k_timer *timer) {
  /* immediately submit to system work queue */
  // current to system queue, better in separate work queue
  k_work_submit(&measure_work);
}

static K_TIMER_DEFINE(measure_timer, measure_timer_callback, NULL);

/* holding register handler */
static void measure_holding_register_handler(uint8_t addr) {
  if (addr == REG_MEASURE_INTERVAL) {
    k_timer_start(&measure_timer, K_NO_WAIT, K_MSEC(holding_reg[addr]->value));
  }
  return;
}

HOLDING_REG_HANDLER_DEFINE(measure_holding_reg,
                           measure_holding_register_handler);

/* start the timer */
static int measure_init(void) {

  // check encoder
  for (int i = 0; i < ARRAY_SIZE(encoder_gpio); i++) {
    if (!gpio_is_ready_dt(encoder_gpio + i)) {
      printk("The encoder GPIO port is not ready.\n");
      return -ENOENT;
    }
  }

  // check adc
  if (!device_is_ready(adc_dev)) {
    printk("ADC device %s is not ready\n", adc_dev->name);
    return -ENOENT;
  }

  // init measurement interval
  holding_reg[REG_MEASURE_INTERVAL]->value = MEASUREMENT_PERIOD;

  // start measurement
  k_timer_start(&measure_timer, K_NO_WAIT,
                K_MSEC(holding_reg[REG_MEASURE_INTERVAL]->value));

  return 0;
}

SYS_INIT(measure_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif
