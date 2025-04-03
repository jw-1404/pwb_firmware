#include "common.h"

#include <zephyr/sys/util.h>

/* define the internal registers */
// [todo] over-skilled, better use STRUCT_SECTION_ITERABLE_ARRAY
LISTIFY(REG_END, HOLDING_REG_DEFINE, (;), 0);
LISTIFY(COIL_END, COIL_REG_DEFINE, (;), false);

/* these global pointers as stubs to internal registers */
// [todo] ugly, define a macro for in-place fetching as needed (see zephyr's
// device.h impl)
struct holding_register_t *holding_reg[REG_END];
struct coil_register_t *coil_reg[COIL_END];

// initialize stub register pointers
static int registers_init(void) {
  for (int i = 0; i < REG_END; i++) {
    STRUCT_SECTION_GET(holding_register_t, i, holding_reg + i);
  }
  for (int i = 0; i < COIL_END; i++) {
    STRUCT_SECTION_GET(coil_register_t, i, coil_reg + i);
  }
  return 0;
}

SYS_INIT(registers_init, POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY);
