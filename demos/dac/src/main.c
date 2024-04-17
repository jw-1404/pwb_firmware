/*
 * Copyright (c) 2020 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/dac.h>

#define DAC1_NODE DT_NODELABEL(volt_ch0)
#define DAC2_NODE DT_NODELABEL(volt_ch1)
#define DAC3_NODE DT_NODELABEL(volt_ch2)
#define DAC4_NODE DT_NODELABEL(volt_ch3)

static const struct device *const dac1_dev = DEVICE_DT_GET(DAC1_NODE);
static const struct device *const dac2_dev = DEVICE_DT_GET(DAC2_NODE);
static const struct device *const dac3_dev = DEVICE_DT_GET(DAC3_NODE);
static const struct device *const dac4_dev = DEVICE_DT_GET(DAC4_NODE);

int main(void)
{
	if (!device_is_ready(dac1_dev)) {
		printk("DAC device %s is not ready\n", dac1_dev->name);
		return 0;
	}
  if (!device_is_ready(dac2_dev)) {
    printk("DAC device %s is not ready\n", dac2_dev->name);
    return 0;
  }
  if (!device_is_ready(dac3_dev)) {
    printk("DAC device %s is not ready\n", dac3_dev->name);
    return 0;
  }
  if (!device_is_ready(dac4_dev)) {
    printk("DAC device %s is not ready\n", dac4_dev->name);
    return 0;
  }

  /* Generating sawtooth signal at DAC */
  int ret;
	while (1) {
		const int dac_values = 0xFFFF;
    int step = dac_values / 4;

    for (int i = 0; i < 5; i++) {
      printk("setting new value: %d\n", i);
      
			ret = dac_write_value(dac1_dev,0 , i*step);
      ret = dac_write_value(dac2_dev, 0, i * step);
      ret = dac_write_value(dac3_dev, 0, i * step);
      ret = dac_write_value(dac4_dev, 0, i * step);

      // dac settling time is 1 us, sleep 1 s for measuring
			k_sleep(K_MSEC(5000));
		}
	}

  /* ret = dac_write_value(dac_dev,0 , 0xFFFF); */

  /* ret = dac_write_value(dac1_dev, 0, 0x7F0F); */
  /* if (ret != 0) { */
  /*   printk("dac_write_value() failed with code %d\n", ret); */
  /*   return 0; */
  /* } */
  /* ret = dac_write_value(dac2_dev, 0, 0x7F0F); */
  /* if (ret != 0) { */
  /*   printk("dac_write_value() failed with code %d\n", ret); */
  /*   return 0; */
  /* } */
  /* ret = dac_write_value(dac3_dev, 0, 0x7F0F); */
  /* if (ret != 0) { */
  /*   printk("dac_write_value() failed with code %d\n", ret); */
  /*   return 0; */
  /* } */
  /* ret = dac_write_value(dac4_dev, 0, 0x7F0F); */
  /* if (ret != 0) { */
  /*   printk("dac_write_value() failed with code %d\n", ret); */
  /*   return 0; */
  /* } */
  return 0;
}
