/*
 * Init and start the modbus server.
 * - this is the last step in firmware start-up
 * - the actual work in response to coil & register changes is dispatched to system work queue immediately
 * - coil & register reading return the current status of internal registers (see modbus_ifc.h for all available registers)
 *
 */

#include "common.h"

#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/modbus/modbus.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

////// k_work to process coil/register commands
struct ctrl_cmd_t {
  struct k_work work;
  uint8_t addr;
};

static struct ctrl_cmd_t coil_cmd = {.addr = ARRAY_SIZE(coil_reg)};
void coil_work_handler(struct k_work *item) {
  struct ctrl_cmd_t *the_cmd = CONTAINER_OF(item, struct ctrl_cmd_t, work);
  COIL_REG_HANDLER_FOREACH(handler) { handler->callback(the_cmd->addr); }

  return;
}

static struct ctrl_cmd_t register_cmd = {.addr = ARRAY_SIZE(holding_reg)};
void register_work_handler(struct k_work *item) {
  struct ctrl_cmd_t *the_cmd = CONTAINER_OF(item, struct ctrl_cmd_t, work);
  HOLDING_REG_HANDLER_FOREACH(handler) { handler->callback(the_cmd->addr); }

  return;
}

static int coil_rd(uint16_t addr, bool *state) {
  if (addr >= ARRAY_SIZE(coil_reg)) {
    return -ENOTSUP;
  }

  *state = coil_reg[addr]->value;
  printk("Coil read, addr %u, %d", addr, (int)*state);

  return 0;
}

static int coil_wr(uint16_t addr, bool state) {
  if (addr >= ARRAY_SIZE(coil_reg)) {
    return -ENOTSUP;
  }

  // cmds without effects, just return
  if (state == coil_reg[addr]->value) {
    return 0;
  }

  //
  coil_reg[addr]->value=state;

  coil_cmd.addr = addr;
  k_work_submit(&coil_cmd.work);

  printk("Coil write, addr %u, %d", addr, (int)state);

  return 0;
}

static int holding_reg_rd(uint16_t addr, uint16_t *reg) {
  if (addr >= ARRAY_SIZE(holding_reg)) {
    return -ENOTSUP;
  }

  *reg = holding_reg[addr]->value;

  printk("Holding register read, addr %u", addr);

  return 0;
}

static int holding_reg_wr(uint16_t addr, uint16_t reg) {
  if (addr >= ARRAY_SIZE(holding_reg)) {
    return -ENOTSUP;
  }

  // cmds without effects, just return
  if ((addr == REG_UPDATE_INTERVAL) && (reg == holding_reg[addr]->value)) {
    return 0;
  }

  //
  holding_reg[addr]->value = reg;

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
  printk("mb iface: %s\n", iface_name);

  return modbus_init_server(iface, server_param);
}

/* only init & start the modbus server in main thread */
int main(void) {

  k_work_init(&coil_cmd.work, coil_work_handler);
  k_work_init(&register_cmd.work, register_work_handler);

  printk("mb server init\n");
  if (init_modbus_server()) {
    printk("Modbus RTU server initialization failed");
  }

  return 0;
}
