#ifdef CONFIG_DS18S20

#include "common.h"

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor/w1_sensor.h>
#include <zephyr/sys/util.h>

#define DS18S20_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(maxim_ds18s20)
static const struct device *dev;

//
static int ds18s20_init(void) {

  dev = DEVICE_DT_GET(DS18S20_NODE);

  if (!device_is_ready(dev)) {
    printk("\nError: Device \"%s\" is not ready; "
           "check the driver initialization logs for errors.\n",
           dev->name);
    return -ENOENT;
  }

  struct sensor_value rom_id;
  sensor_attr_get(dev, SENSOR_CHAN_MAX, SENSOR_ATTR_W1_ROM, &rom_id);

  holding_reg[REG_BOARD_UUID + 3]->value = rom_id.val1 & BIT_MASK(16);
  holding_reg[REG_BOARD_UUID + 2]->value = (rom_id.val1 >> 16) & BIT_MASK(16);
  holding_reg[REG_BOARD_UUID + 1]->value = rom_id.val2 & BIT_MASK(16);
  holding_reg[REG_BOARD_UUID]->value = (rom_id.val2 >> 16) & BIT_MASK(16);

  printk("UUID (ROM id): %08x %08x\n", rom_id.val2, rom_id.val1);

  return 0;
}

SYS_INIT(ds18s20_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif
