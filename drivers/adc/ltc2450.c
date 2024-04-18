/* LLTC LTC2450-1 ADC
 *
 * Not a full implementatio of zephyr's adc interface.
 * Just a normal SPI device implementing adc's read api.
 * More subtle details of adc API is not needed, thus not
 * implemented.
 *
 * NOTE:
 * The implementation includes a k_sleep while waiting for
 * conversion finished. Thus, this driver blocks the invoking thread
 * while reading latest conversion value.
 *
 * Copyright (c) Yong Zhou
 *
 */

#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>

LOG_MODULE_REGISTER(adc_ltc2450, CONFIG_ADC_LOG_LEVEL);

#define DT_DRV_COMPAT lltc_ltc2450

struct ltc2450_config {
  struct spi_dt_spec bus;
};

static int ltc2450_read_latest_conversion(const struct device *dev,
                                          const struct adc_sequence *sequence) {
  const struct ltc2450_config *config = dev->config;

  uint16_t *value_buf;
  uint8_t buffer_rx[2];
  const struct spi_buf rx_buf[2] = {{.buf = buffer_rx, .len = 1},
                                    {.buf = buffer_rx, .len = 2}};
  const struct spi_buf_set cmd_start_cnv = {
      .buffers = rx_buf,
      .count = 1,
  };
  const struct spi_buf_set cmd_read_cnv = {
      .buffers = rx_buf+1,
      .count = 1,
  };

  // start new conversion
  int err = spi_read_dt(&config->bus, &cmd_start_cnv);
  if (err != 0) {
    LOG_ERR("LTC start conversion failed (err %d)", err);
    return err;
  }

  // wait for conversion completion
  k_msleep(CONFIG_ADC_LTC2450_CONVERSION_MS);

  // read back the latest conversion data
  err = spi_read_dt(&config->bus, &cmd_read_cnv);
  if (err != 0) {
    LOG_ERR("LTC read back conversion data failed (err %d)", err);
    return err;
  }

  value_buf = (uint16_t *)sequence->buffer;
  value_buf[0] = sys_get_be16(buffer_rx);
  
  return 0;
}

static int ltc2450_init(const struct device *dev) {
  const struct ltc2450_config *config = dev->config;

  if (!spi_is_ready_dt(&config->bus)) {
    LOG_ERR("SPI device not ready");
    return -ENODEV;
  }

  return 0;
}

static const struct adc_driver_api ltc2450_api = {
    .read = ltc2450_read_latest_conversion,
};

#define LTC2450_DEFINE(index)                                                  \
  static const struct ltc2450_config ltc2450_cfg_##index = {                   \
      .bus = SPI_DT_SPEC_INST_GET(                                             \
          index, SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_WORD_SET(8), 1),  \
  };                                                                           \
  DEVICE_DT_INST_DEFINE(index, &ltc2450_init, NULL, NULL,                      \
                        &ltc2450_cfg_##index, POST_KERNEL,                     \
                        CONFIG_ADC_LTC2450_INIT_PRIORITY, &ltc2450_api);

DT_INST_FOREACH_STATUS_OKAY(LTC2450_DEFINE)
