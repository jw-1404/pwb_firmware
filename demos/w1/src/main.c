/*
 * Copyright (c) 2022 Thomas Stranger
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/types.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor/w1_sensor.h>
#include <zephyr/kernel.h>

/*
 * Get a device structure from a devicetree node with compatible
 * "maxim,ds18s20". (If there are multiple, just pick one.)
 */
static const struct device *get_ds18s20_device(void)
{
	const struct device *const dev = DEVICE_DT_GET_ANY(maxim_ds18s20);

	if (dev == NULL) {
		/* No such node, or the node does not have status "okay". */
		printk("\nError: no device found.\n");
		return NULL;
	}

	if (!device_is_ready(dev)) {
		printk("\nError: Device \"%s\" is not ready; "
		       "check the driver initialization logs for errors.\n",
		       dev->name);
		return NULL;
	}

	printk("Found device \"%s\", getting sensor data\n", dev->name);
	return dev;
}

int main(void)
{
	const struct device *dev = get_ds18s20_device();

	if (dev == NULL) {
		return 0;
	}

  // get uuid
  struct sensor_value rom_id;
  sensor_attr_get(dev, SENSOR_CHAN_MAX, SENSOR_ATTR_W1_ROM, &rom_id);
  printk("UUID (ROM id): %08x %08x\n", rom_id.val2, rom_id.val1);

  // poll temperature
  int32_t couter=0;
	while (1) {
		struct sensor_value temp;

		sensor_sample_fetch(dev);
		sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);

		printk("%d Temp: %d.%06d\n", ++couter, temp.val1, temp.val2);
    k_sleep(K_MSEC(1000));
	}
	return 0;
}
