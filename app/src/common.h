#pragma once

#include "modbus_ifc.h"
#include <iterables/callback.h>
#include <iterables/register.h>

extern struct holding_register_t* holding_reg[REG_END];
extern struct coil_register_t* coil_reg[COIL_END];
