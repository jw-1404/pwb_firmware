#ifdef CONFIG_SHT3XD

#include "common.h"

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

#define SHT3XA_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(sensirion_sht3xd)
static const struct device *dev;

/* status polling handler */
void sht3xa_poll()
{
  struct sensor_value temp, hum;
  sensor_sample_fetch(dev);
  sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
  sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &hum);
  holding_reg[REG_TEMPERATURE_INT]->value = temp.val1 & BIT_MASK(16);
  holding_reg[REG_TEMPERATURE_FLT]->value = temp.val2 & BIT_MASK(16); // = flt/1000000
  holding_reg[REG_HUMIDITY_INT]->value = hum.val1 & BIT_MASK(16);
  holding_reg[REG_HUMIDITY_FLT]->value = hum.val2 & BIT_MASK(16); // = flt/1000000

  return;
}

POLL_HANDLER_DEFINE(sht3xa_poll, sht3xa_poll);

//
static int sht3xa_init(void) {
  dev = DEVICE_DT_GET(SHT3XA_NODE);

  if (!device_is_ready(dev)) {
    printk("\nError: Device \"%s\" is not ready; "
           "check the driver initialization logs for errors.\n",
           dev->name);
    return -ENOENT;
  }

  return 0;
}

SYS_INIT(sht3xa_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif
