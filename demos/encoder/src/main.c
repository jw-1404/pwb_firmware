/*
 * Copyright (c) 2023 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

static const struct gpio_dt_spec mux_a0 = GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(adc_encoder), gpios, 0);
static const struct gpio_dt_spec mux_a1 =
    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(adc_encoder), gpios, 1);
static const struct gpio_dt_spec mux_a2 =
    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(adc_encoder), gpios, 2);

int main(void) {
  int err;

  if (!gpio_is_ready_dt(&mux_a0)) {
    printk("The load switch pin GPIO port is not ready.\n");
    return 0;
  }
  if (!gpio_is_ready_dt(&mux_a1)) {
    printk("The load switch pin GPIO port is not ready.\n");
    return 0;
  }
  if (!gpio_is_ready_dt(&mux_a2)) {
    printk("The load switch pin GPIO port is not ready.\n");
    return 0;
  }

  printk("Initializing pin with inactive level.\n");

  /* err = gpio_pin_configure_dt(&mux_a0, GPIO_OUTPUT_INACTIVE); */
  /* if (err != 0) { */
  /*   printk("Configuring GPIO pin failed: %d\n", err); */
  /*   return 0; */
  /* } */

  /* printk("Waiting 10 second.\n"); */

  /* k_sleep(K_MSEC(10000)); */

  /* printk("Setting pin to active level.\n"); */

  /* err = gpio_pin_set_dt(&mux_a0, 1); */
  /* err = gpio_pin_configure_dt(&mux_a1, GPIO_OUTPUT_ACTIVE); */
  err = gpio_pin_configure_dt(&mux_a2, GPIO_OUTPUT_ACTIVE);
  if (err != 0) {
    printk("Setting GPIO pin level failed: %d\n", err);
  }
  return 0;
}
