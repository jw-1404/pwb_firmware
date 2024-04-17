#ifdef CONFIG_DAC_AD5541

#include "common.h"

#include <zephyr/device.h>
#include <zephyr/drivers/dac.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#define DAC_NUM 4
#define DAC_LABEL_PREFIX volt_ch

static const struct device *dac_dev[DAC_NUM];

/* coil register handler */
#define SWITCH_ONOFF_VOLTAGE(id)                                               \
  if (coil_reg[COIL_TRIPPED_CH##id]->value)                                    \
    return;                                                                    \
  if (coil_reg[COIL_ONOFF_CH##id]->value)                                      \
    dac_write_value(dac_dev[id], 0, holding_reg[REG_SVV_CH##id]->value);       \
  else                                                                         \
    dac_write_value(dac_dev[id], 0, 0)

static void dac_coil_register_handler(uint8_t addr) {
  switch (addr) {
  case COIL_ONOFF_CH0:
    SWITCH_ONOFF_VOLTAGE(0);
    break;
  case COIL_ONOFF_CH1:
    SWITCH_ONOFF_VOLTAGE(1);
    break;
  case COIL_ONOFF_CH2:
    SWITCH_ONOFF_VOLTAGE(2);
    break;
  case COIL_ONOFF_CH3:
    SWITCH_ONOFF_VOLTAGE(3);
    break;
  }

  return;
}

COIL_REG_HANDLER_DEFINE(dac_coil_reg, dac_coil_register_handler);

/* holding register handler */
#define UPDATE_VOLTAGE(id)                                                     \
  if (coil_reg[COIL_TRIPPED_CH##id]->value ||                                  \
      !coil_reg[COIL_ONOFF_CH##id]->value)                                     \
    return;                                                                    \
  dac_write_value(dac_dev[id], 0, holding_reg[REG_SVV_CH##id]->value)

static void dac_holding_register_handler(uint8_t addr) {
  switch (addr) {
  case REG_SVV_CH0:
    UPDATE_VOLTAGE(0);
    break;
  case REG_SVV_CH1:
    UPDATE_VOLTAGE(1);
    break;
  case REG_SVV_CH2:
    UPDATE_VOLTAGE(2);
    break;
  case REG_SVV_CH3:
    UPDATE_VOLTAGE(3);
    break;
  }

  return;
}

HOLDING_REG_HANDLER_DEFINE(dac_holding_reg, dac_holding_register_handler);

/* init func */
#define CHECK_DAC_READINESS(index, prefix)                                     \
  dac_dev[index] = DEVICE_DT_GET(DT_NODELABEL(prefix##index));                 \
  if (!device_is_ready(dac_dev[index])) {                                      \
    printk("\nError: Device \"%s\" is not ready; "                             \
           "check the driver initialization logs for errors.\n",               \
           dac_dev[index]->name);                                              \
    return -ENOENT;                                                            \
  }

static int dac_init(void) {
  LISTIFY(DAC_NUM, CHECK_DAC_READINESS, (), DAC_LABEL_PREFIX);
  return 0;
}

SYS_INIT(dac_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif
