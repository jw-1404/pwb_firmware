#pragma once

// warning: no '_' in name for numeric sort in effect
#define XLAB_REG_DEFINE(struct_type, name, _addr, _value)                          \
  STRUCT_SECTION_ITERABLE(struct_type,                                         \
                          _CONCAT(name##_, _addr##_)) = {.value = _value}

#define XLAB_HANDLER_DEFINE(struct_type, name, _cb)                      \
  STRUCT_SECTION_ITERABLE(struct_type, _CONCAT(name##_, handler)) = {.callback = _cb}
