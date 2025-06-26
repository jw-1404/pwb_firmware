#include "common.h"

#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

/* unit: ms */
#define MEASUREMENT_PERIOD 150
#define MEASUREMENT_TIMEs 30

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) ||            \
  !DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

/* use DT_FOREACH_PROP_ELEM in combination with ADC_DT_SPEC_GET_BY_IDX, see adc_dt sample; io-channel must be used */
#define DT_SPEC_AND_COMMA(node_id, prop, idx)   \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* 0-9: voltage, 10-19: current */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
                       DT_SPEC_AND_COMMA)
};

/* hardcoded mapping from encoder ch to internal register addr (fixed in PCB design) */
const static uint16_t voltage_ch_map[] = {REG_PVV_CH0, REG_PVV_CH1, REG_PVV_CH2,
                                          REG_PVV_CH3, REG_PVV_CH4, REG_PVV_CH5,
                                          REG_PVV_CH6, REG_PVV_CH7, REG_PVV_CH8,
                                          REG_PVV_CH9};
const static uint16_t current_ch_map[] = {REG_PVI_CH0, REG_PVI_CH1, REG_PVI_CH2,
                                          REG_PVI_CH3, REG_PVI_CH4, REG_PVI_CH5,
                                          REG_PVI_CH6, REG_PVI_CH7, REG_PVI_CH8,
                                          REG_PVI_CH9};
// [todo] should be calibrated for each board
const static uint16_t current_zero_offset[] = { 1822, 1682, 1640, 1730, 1715,
                                                1707, 1812, 1717, 1670, 1662};

static int32_t voltage_cache[ARRAY_SIZE(voltage_ch_map)];
static int32_t current_cache[ARRAY_SIZE(current_ch_map)];
static int32_t cache_counter = 0;

// [todo] rtn err code?
inline static void read_voltage() {
  int err;
  uint16_t buf;
  struct adc_sequence sequence = {
      .buffer = &buf,
      .buffer_size = sizeof(buf),
  };

  for (size_t i = 0U; i < ARRAY_SIZE(voltage_ch_map); i++) {
    (void)adc_sequence_init_dt(&adc_channels[i], &sequence);

    err = adc_read_dt(&adc_channels[i], &sequence);
    if (err < 0) {
      printk("Could not read voltage value at ch_%d (%d)\n", i+1, err);
      return;
    }

    // [todo] value conversion

    // update corresponding regs
    if (cache_counter == (MEASUREMENT_TIMEs-1)) {
      uint16_t tmp_cache = voltage_cache[i] / MEASUREMENT_TIMEs;
      holding_reg[voltage_ch_map[i]]->value = 3300.0 / 4096 * tmp_cache * 5.6;
      voltage_cache[i] = 0;
      printk("voltage %d: %d\n", i + 1, holding_reg[voltage_ch_map[i]]->value);
    }
    else {
      voltage_cache[i] += buf;
    }
  }

  return;
}

inline static void read_current() {
  int err;
  uint16_t buf;
  struct adc_sequence sequence = {
      .buffer = &buf,
      .buffer_size = sizeof(buf),
  };

  size_t start_index = ARRAY_SIZE(voltage_ch_map);
  for (size_t i = start_index; i < ARRAY_SIZE(adc_channels); i++) {
    //    printk("ch_%d\n", i + 1);
    (void)adc_sequence_init_dt(&adc_channels[i], &sequence);
    err = adc_read_dt(&adc_channels[i], &sequence);
    if (err < 0) {
      printk("Could not read current value at ch_%d (%d)\n", i + 1, err);
      return;
    }

    // [todo] value conversion
    /* int32_t val_mv = (int32_t)buf; */
    /* err = adc_raw_to_millivolts_dt(&adc_channels[i], &val_mv); */
    /* /\* conversion to mV may not be supported, skip if not *\/ */
    /* if (err < 0) { */
    /*   printk(" (value in mV not available)\n"); */
    /* } else { */
    /*   printk(" = %" PRId32 " mV\n", val_mv); */
    /* } */

    // update corresponding regs
    if (cache_counter == (MEASUREMENT_TIMEs-1)) {
      uint16_t tmp_cache = current_cache[i - start_index] / MEASUREMENT_TIMEs;
      if (tmp_cache < current_zero_offset[i - start_index]){
        tmp_cache = 0;
      }
      else {
        tmp_cache -= current_zero_offset[i - start_index];
      }

      /* holding_reg[current_ch_map[i - start_index]]->value = tmp_cache*16.0 -
       * 25000; */
      holding_reg[current_ch_map[i - start_index]]->value = tmp_cache*16.0;
      current_cache[i-start_index] = 0;
      printk("current %d: %d\n", i + 1, holding_reg[current_ch_map[i-start_index]]->value);
    }
    else {
      current_cache[i-start_index] += buf;
    }
  }

  return;
}
/* work handler for measurement timer */
static void measure_work_handler(struct k_work *work) {
  read_current();
  read_voltage();

  if(cache_counter == (MEASUREMENT_TIMEs-1))
    cache_counter = 0;
  else
    cache_counter++;

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

  // init measurement interval
  holding_reg[REG_MEASURE_INTERVAL]->value = MEASUREMENT_PERIOD;

  for (int i = 0; i < ARRAY_SIZE(adc_channels); i++) {
    if (!adc_is_ready_dt(&adc_channels[i])) {
      printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
      return -ENOENT;
    }
    int err = adc_channel_setup_dt(&adc_channels[i]);
    if (err < 0) {
      printk("Could not setup channel #%d (%d)\n", i, err);
      return err;
    }
  }

  // start measurement
  k_timer_start(&measure_timer, K_NO_WAIT,
                K_MSEC(MEASUREMENT_PERIOD));

  return 0;
}

SYS_INIT(measure_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

