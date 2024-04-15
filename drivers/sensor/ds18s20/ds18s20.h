/*
 * Copyright (c) 2022 Thomas Stranger
 * Copyright (c) 2024 Yong Zhou
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/w1.h>
#include <zephyr/sys/util_macro.h>

#define DS18S20_CMD_CONVERT_T         0x44
#define DS18S20_CMD_WRITE_SCRATCHPAD  0x4E
#define DS18S20_CMD_READ_SCRATCHPAD   0xBE
#define DS18S20_CMD_COPY_SCRATCHPAD   0x48
#define DS18S20_CMD_RECALL_EEPROM     0xB8
#define DS18S20_CMD_READ_POWER_SUPPLY 0xB4

#define DS18S20_MEASURE_WAIT_TIME 94

struct ds18s20_scratchpad {
	int16_t temp;
	uint8_t alarm_temp_high;
	uint8_t alarm_temp_low;
	uint8_t res[2];
	uint8_t count_remain;
  uint8_t count_per;
  uint8_t crc;
};

struct ds18s20_config {
	const struct device *bus;
	uint8_t family;
};

struct ds18s20_data {
	struct w1_slave_config config;
	struct ds18s20_scratchpad scratchpad;
	bool lazy_loaded;
};
