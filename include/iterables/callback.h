#pragma once

#include "macro_internal.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/iterable_sections.h>

//
struct status_handler_t {
  void (*const callback)();
};

#define POLL_HANDLER_DEFINE(_name, _cb)                                          \
  XLAB_HANDLER_DEFINE(status_handler_t, _name, _cb)

#define POLL_HANDLER_FOREACH(handler) STRUCT_SECTION_FOREACH(status_handler_t, handler)

//
struct coil_register_handler_t {
  void (*const callback)(uint8_t addr);
};

#define COIL_REG_HANDLER_DEFINE(_name, _cb)                                   \
  XLAB_HANDLER_DEFINE(coil_register_handler_t, _name, _cb)

#define COIL_REG_HANDLER_FOREACH(handler)                                        \
  STRUCT_SECTION_FOREACH(coil_register_handler_t, handler)

//
struct holding_register_handler_t {
  void (*const callback)(uint8_t addr);
};

#define HOLDING_REG_HANDLER_DEFINE(_name, _cb)                \
  XLAB_HANDLER_DEFINE(holding_register_handler_t, _name, _cb)

#define HOLDING_REG_HANDLER_FOREACH(handler)                                          \
  STRUCT_SECTION_FOREACH(holding_register_handler_t, handler)
