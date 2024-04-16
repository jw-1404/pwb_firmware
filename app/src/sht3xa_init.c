#ifdef CONFIG_SHT3XD

#include "common.h"
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

static int sht3xa_init(void) {
  const struct device *const dev = DEVICE_DT_GET(SHT3XA_NODE);

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
