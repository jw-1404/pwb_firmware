#pragma once

#include "registers.h"
#include <zephyr/kernel.h>

/* global buffers */
extern uint16_t holding_reg[REG_END];
extern uint16_t coils_state;


/* peripheral devices' dts node */
#define DS18S20_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(maxim_ds18s20)
#define SHT3XA_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(sensirion_sht3xd)
