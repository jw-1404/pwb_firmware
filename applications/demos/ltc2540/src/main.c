#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#define ADC_NODE DT_NODELABEL(adc_ltc2450)
static const struct device *const adc_dev = DEVICE_DT_GET(ADC_NODE);

int main(void)
{
  if (!device_is_ready(adc_dev)) {
    printk("ADC device %s is not ready\n", adc_dev->name);
    return 0;
  }

  int err;
	uint32_t count = 0;
	uint16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		.buffer_size = sizeof(buf),
	};

	while (1) {
		printk("ADC reading[%u]:\n", count++);
    err = adc_read(adc_dev, &sequence);
    if (err < 0) {
      printk("Could not read (%d)\n", err);
      continue;
    }

    printk("adc value: %x\n", buf);
		k_sleep(K_MSEC(1000));
	}
	return 0;
}
