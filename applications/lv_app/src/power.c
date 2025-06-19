#include "common.h"
#include "modbus_ifc.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>

/* #if !DT_NODE_EXISTS(DT_NODELABEL(powers)) */
/* #error "compiler: dts node with lablel powers not defined" */
/* #else */
/* #define VALVE_NODE DT_NODELABEL(powers) */
/* #endif */

/* try: DT_FOREACH_PROP_ELEM in combination with GPIO_DT_SPEC_GET_BY_IDX; propery name customizable */
static const struct gpio_dt_spec pwr0 = GPIO_DT_SPEC_GET(DT_ALIAS(pwr2), gpios); // [bug]: ch1 and ch3 swaped
static const struct gpio_dt_spec pwr1 = GPIO_DT_SPEC_GET(DT_ALIAS(pwr1), gpios);
static const struct gpio_dt_spec pwr2 = GPIO_DT_SPEC_GET(DT_ALIAS(pwr0), gpios);
static const struct gpio_dt_spec pwr3 = GPIO_DT_SPEC_GET(DT_ALIAS(pwr3), gpios);
static const struct gpio_dt_spec pwr4 = GPIO_DT_SPEC_GET(DT_ALIAS(pwr4), gpios);
static const struct gpio_dt_spec pwr5 = GPIO_DT_SPEC_GET(DT_ALIAS(pwr5), gpios);
static const struct gpio_dt_spec pwr6 = GPIO_DT_SPEC_GET(DT_ALIAS(pwr6), gpios);
static const struct gpio_dt_spec pwr7 = GPIO_DT_SPEC_GET(DT_ALIAS(pwr7), gpios);
static const struct gpio_dt_spec pwr8 = GPIO_DT_SPEC_GET(DT_ALIAS(pwr8), gpios);
static const struct gpio_dt_spec pwr9 = GPIO_DT_SPEC_GET(DT_ALIAS(pwr9), gpios);

static void power_coil_register_handler(uint8_t addr) {
  switch (addr) {
    case COIL_ONOFF_CH0:
      /* gpio_pin_configure_dt(&pwr0, coil_reg[COIL_ONOFF_CH0]->value */
      /*                       ? GPIO_OUTPUT_ACTIVE */
      /*                       : GPIO_OUTPUT_INACTIVE); */
      coil_reg[COIL_ONOFF_CH0]->value = true;
      break;
    case COIL_ONOFF_CH1:
      gpio_pin_configure_dt(&pwr1, coil_reg[COIL_ONOFF_CH1]->value
                                       ? GPIO_OUTPUT_ACTIVE
                                       : GPIO_OUTPUT_INACTIVE);
      break;
    // CHno effect on CH0, it's always on
    case COIL_ONOFF_CH2:
      gpio_pin_configure_dt(&pwr2, coil_reg[COIL_ONOFF_CH2]->value
                                       ? GPIO_OUTPUT_ACTIVE
                                       : GPIO_OUTPUT_INACTIVE);
      break;
    case COIL_ONOFF_CH3:
      gpio_pin_configure_dt(&pwr3, coil_reg[COIL_ONOFF_CH3]->value
                                       ? GPIO_OUTPUT_ACTIVE
                                       : GPIO_OUTPUT_INACTIVE);
      break;
    case COIL_ONOFF_CH4:
      gpio_pin_configure_dt(&pwr4, coil_reg[COIL_ONOFF_CH4]->value
                                       ? GPIO_OUTPUT_ACTIVE
                                       : GPIO_OUTPUT_INACTIVE);
      break;
    case COIL_ONOFF_CH5:
      gpio_pin_configure_dt(&pwr5, coil_reg[COIL_ONOFF_CH5]->value
                                       ? GPIO_OUTPUT_ACTIVE
                                       : GPIO_OUTPUT_INACTIVE);
      break;
    case COIL_ONOFF_CH6:
      gpio_pin_configure_dt(&pwr6, coil_reg[COIL_ONOFF_CH6]->value
                                       ? GPIO_OUTPUT_ACTIVE
                                       : GPIO_OUTPUT_INACTIVE);
      break;
    case COIL_ONOFF_CH7:
      gpio_pin_configure_dt(&pwr7, coil_reg[COIL_ONOFF_CH7]->value
                                       ? GPIO_OUTPUT_ACTIVE
                                       : GPIO_OUTPUT_INACTIVE);
      break;
    case COIL_ONOFF_CH8:
      gpio_pin_configure_dt(&pwr8, coil_reg[COIL_ONOFF_CH8]->value
                                       ? GPIO_OUTPUT_ACTIVE
                                       : GPIO_OUTPUT_INACTIVE);
      break;
    case COIL_ONOFF_CH9:
      gpio_pin_configure_dt(&pwr9, coil_reg[COIL_ONOFF_CH9]->value
                                       ? GPIO_OUTPUT_ACTIVE
                                       : GPIO_OUTPUT_INACTIVE);
      break;
  }

  return;
}

COIL_REG_HANDLER_DEFINE(power_coil_reg, power_coil_register_handler);

static int power_init(void) {

  if (!gpio_is_ready_dt(&pwr1) ||
      !gpio_is_ready_dt(&pwr2) ||
      !gpio_is_ready_dt(&pwr3) ||
      !gpio_is_ready_dt(&pwr4) ||
      !gpio_is_ready_dt(&pwr5) ||
      !gpio_is_ready_dt(&pwr6) ||
      !gpio_is_ready_dt(&pwr7) ||
      !gpio_is_ready_dt(&pwr8) ||
      !gpio_is_ready_dt(&pwr9) ||
      !gpio_is_ready_dt(&pwr0)) {
    printk("The valve GPIO port is not ready.\n");
    return -ENOENT;
  }

	gpio_pin_configure_dt(&pwr0, GPIO_OUTPUT_ACTIVE); coil_reg[COIL_ONOFF_CH0]->value = true;
	gpio_pin_configure_dt(&pwr1, GPIO_OUTPUT_ACTIVE); coil_reg[COIL_ONOFF_CH1]->value = true;
	gpio_pin_configure_dt(&pwr2, GPIO_OUTPUT_ACTIVE); coil_reg[COIL_ONOFF_CH2]->value = true;
	gpio_pin_configure_dt(&pwr3, GPIO_OUTPUT_ACTIVE); coil_reg[COIL_ONOFF_CH3]->value = true;
	gpio_pin_configure_dt(&pwr4, GPIO_OUTPUT_ACTIVE); coil_reg[COIL_ONOFF_CH4]->value = true;
	gpio_pin_configure_dt(&pwr5, GPIO_OUTPUT_ACTIVE); coil_reg[COIL_ONOFF_CH5]->value = true;
	gpio_pin_configure_dt(&pwr6, GPIO_OUTPUT_ACTIVE); coil_reg[COIL_ONOFF_CH6]->value = true;
	gpio_pin_configure_dt(&pwr7, GPIO_OUTPUT_ACTIVE); coil_reg[COIL_ONOFF_CH7]->value = true;
	gpio_pin_configure_dt(&pwr8, GPIO_OUTPUT_ACTIVE); coil_reg[COIL_ONOFF_CH8]->value = true;
	gpio_pin_configure_dt(&pwr9, GPIO_OUTPUT_ACTIVE); coil_reg[COIL_ONOFF_CH9]->value = true;

  return 0;
}

SYS_INIT(power_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
