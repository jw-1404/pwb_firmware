#define DT_DRV_COMPAT adi_ad5541

#include <zephyr/drivers/dac.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(dac_ad5541, CONFIG_DAC_LOG_LEVEL);

#define AD5541_DAC_MAX_VAL 0xFFFF

struct ad5541_config 
{
  struct spi_dt_spec bus;
};

/* AD5541 is a single channel 16 bit DAC */
static int ad5541_channel_setup(const struct device* dev,
                                const struct dac_channel_cfg *channel_cfg)
{
  if (channel_cfg->channel_id != 0) {
    return -EINVAL;
  }

  if (channel_cfg->resolution != 16) {
    return -ENOTSUP;
  }

  return 0;
}

/* single channel dac: argument channel must be 0 */
static int ad5541_write_value(const struct device *dev,
                              uint8_t channel, uint32_t value)
{
  if (channel != 0) {
    return -EINVAL;
  }

  /* Check value isn't over 16 bits */
  if (value > AD5541_DAC_MAX_VAL) {
    return -ENOTSUP;
  }

  const struct ad5541_config *config = dev->config;

  // fill in tx buffers
  uint8_t tx_data[2];
  tx_data[0] = value >> 8;
  tx_data[1] = value & BIT_MASK(8);
  struct spi_buf tx_buf = {
    .buf = &tx_data,
    .len = 2
  };
  struct spi_buf_set tx_bufs = {
    .buffers = &tx_buf,
    .count = 1
  };

  return spi_write_dt(&config->bus, &tx_bufs);
}

static const struct dac_driver_api ad5541_driver_api = {
    .channel_setup = ad5541_channel_setup,
    .write_value = ad5541_write_value
};

static int ad5541_init(const struct device *dev)
{
  const struct ad5541_config *config = dev->config;

  if (!spi_is_ready_dt(&config->bus)) {
    LOG_ERR("SPI bus %s not ready", config->bus.bus->name);
    return -ENODEV;
  }

  return 0;
}

#define DAC_AD5541_INST_DEFINE(index)                                          \
  static const struct ad5541_config ad5541_config_##index = {                  \
      .bus = SPI_DT_SPEC_INST_GET(                                             \
          index, SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_WORD_SET(8),     \
          CONFIG_DAC_AD5541_CS_DELAY),                                  \
  };                                                                    \
  DEVICE_DT_INST_DEFINE(index, ad5541_init, NULL, NULL,                 \
                        &ad5541_config_##index, POST_KERNEL,            \
                        CONFIG_DAC_AD5541_INIT_PRIORITY, &ad5541_driver_api);

DT_INST_FOREACH_STATUS_OKAY(DAC_AD5541_INST_DEFINE)
