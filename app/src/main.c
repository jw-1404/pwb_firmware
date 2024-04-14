#include <zephyr/kernel.h>
#include <zephyr/modbus/modbus.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

// in ms
#define STATUS_UPDATE_PERIOD 1000

////// internal registers as well as external communication interfaces
#include "registers.h"
static uint16_t holding_reg[REG_END];
static uint16_t coils_state; // max 16 coils, actual defined in registers.h

////// k_work to process coil/register commands
typedef struct {
  struct k_work work;
  uint8_t addr;
} control_cmd;

static struct k_work coil_work;
static control_cmd coil_cmd = {.work = coil_work, .addr = COIL_END};
void coil_work_handler(struct k_work *item) {
  struct control_cmd *the_cmd = CONTAINER_OF(item, struct control_cmd, work);
  switch (the_cmd->addr) {
  case COIL_ONOFF_CH1:
    // check the coil state
    // on: send vpv to dac
    // off: send 0 to dac
    printk("turn on ch1");
    break;
  default:
    printk("unsupported");
  }
  return;
}

static struct k_work register_work;
static struct k_work register_cmd = {.work = register_work, .addr = REG_END};
void register_work_handler(struct k_work *work) {
  struct control_cmd *the_cmd = CONTAINER_OF(item, struct control_cmd, work);
  switch (the_cmd->addr) {
  case REG_RESET:
    // check the coil state
    printk("reset the boards");
    break;
  default:
    printk("unsupported");
  }
  return;
}

////// modbus
static int coil_rd(uint16_t addr, bool *state) {
  if (addr >= COIL_END) {
    return -ENOTSUP;
  }

  if (coils_state & BIT(addr)) {
    *state = true;
  } else {
    *state = false;
  }

  printk("Coil read, addr %u, %d", addr, (int)*state);

  return 0;
}

static int coil_wr(uint16_t addr, bool state) {
  if (addr >= COIL_END) {
    return -ENOTSUP;
  }

  if (state == true) {
    coils_state |= BIT(addr);
  } else {
    coils_state &= ~BIT(addr);
  }

  coil_cmd.addr = addr;
  k_work_submit(&coil_cmd.work);

  printk("Coil write, addr %u, %d", addr, (int)state);

  return 0;
}

static int holding_reg_rd(uint16_t addr, uint16_t *reg) {
  if (addr >= ARRAY_SIZE(holding_reg)) {
    return -ENOTSUP;
  }

  *reg = holding_reg[addr];

  printk("Holding register read, addr %u", addr);

  return 0;
}

static int holding_reg_wr(uint16_t addr, uint16_t reg) {
  if (addr >= ARRAY_SIZE(holding_reg)) {
    return -ENOTSUP;
  }

  holding_reg[addr] = reg;

  register_cmd.addr = addr;
  k_work_submit(&register_cmd.work);

  printk("Holding register write, addr %u", addr);

  return 0;
}

static struct modbus_user_callbacks mbs_cbs = {
    .coil_rd = coil_rd,
    .coil_wr = coil_wr,
    .holding_reg_rd = holding_reg_rd,
    .holding_reg_wr = holding_reg_wr,
};

const static struct modbus_iface_param server_param = {
    .mode = MODBUS_MODE_RTU,
    .server =
        {
            .user_cb = &mbs_cbs,
            .unit_id = 1,
        },
    .serial =
        {
            .baud = 19200,
            .parity = UART_CFG_PARITY_NONE,
        },
};

#define MODBUS_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(zephyr_modbus_serial)

static int init_modbus_server(void) {
  const char iface_name[] = {DEVICE_DT_NAME(MODBUS_NODE)};
  int iface;

  iface = modbus_iface_get_by_name(iface_name);

  if (iface < 0) {
    printk("Failed to get iface index for %s", iface_name);
    return iface;
  }

  return modbus_init_server(iface, server_param);
}

////// internal status polling
void status_work_handler(struct k_work *work) {
  /* 1. read present V & I */

  /* 2. read present temepature & humidity */

  /* 3. trip or not */
  if (!(coils_state & BIT(COIL_TRIPPED_CH1))) {
    // check trip condition & do trip
  }
}
K_WORK_DEFINE(status_work, status_work_handler);

static void status_timer_callback(struct k_timer *timer) {
  k_work_submit(&status_work);
}

static K_TIMER_DEFINE(status_timer, status_timer_callback, NULL);

///////// main thread
int main(void) {
  /* internal registers init */
  coils_state = 0x0000;
  for (int i = 0; i < REG_END; i++) {
    holding_reg[i] = i + 1;
  }

  /* work init */
  k_work_init(&coil_cmd.work, coil_work_handler);
  k_work_init(&register_cmd.work, register_work_handler);

  /* modbus server init */
  if (init_modbus_server()) {
    printk("Modbus RTU server initialization failed");
  }

  /* status polling loop init */
  k_timer_start(&status_timer, 0, K_MSEC(STATUS_UPDATE_PERIOD));

  return 0;
}