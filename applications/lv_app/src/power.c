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

static const struct gpio_dt_spec pwr1 = GPIO_DT_SPEC_GET(DT_ALIAS(pwr1), gpios);
static const struct gpio_dt_spec pwr2 = GPIO_DT_SPEC_GET(DT_ALIAS(pwr2), gpios);
static const struct gpio_dt_spec pwr3 = GPIO_DT_SPEC_GET(DT_ALIAS(pwr3), gpios);

static void power_coil_register_handler(uint8_t addr) {
  switch (addr) {
  case COIL_ONOFF_CH0:
    gpio_pin_configure_dt(&pwr1, coil_reg[COIL_ONOFF_CH0]->value
                                     ? GPIO_OUTPUT_ACTIVE
                                     : GPIO_OUTPUT_INACTIVE);
    break;
  case COIL_ONOFF_CH1:
    gpio_pin_configure_dt(&pwr2, coil_reg[COIL_ONOFF_CH1]->value
                                     ? GPIO_OUTPUT_ACTIVE
                                     : GPIO_OUTPUT_INACTIVE);
    break;
  case COIL_ONOFF_CH2:
    gpio_pin_configure_dt(&pwr3, coil_reg[COIL_ONOFF_CH2]->value
                                      ? GPIO_OUTPUT_ACTIVE
                                      : GPIO_OUTPUT_INACTIVE);
    break;
  default:
    break;
  }

  return;
}

COIL_REG_HANDLER_DEFINE(power_coil_reg, power_coil_register_handler);

static int power_init(void) {

  if (!gpio_is_ready_dt(&pwr1) || !gpio_is_ready_dt(&pwr2) ||
      !gpio_is_ready_dt(&pwr3)) {
    printk("The valve GPIO port is not ready.\n");
    return -ENOENT;
  }

  return 0;
}

SYS_INIT(power_init, POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY);
