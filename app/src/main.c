#include "common.h"
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/modbus/modbus.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

// in ms
#define STATUS_UPDATE_PERIOD 1000

#define ALIVE_COUNTING_PERIOD 1000
static uint32_t alive_counts;

////// internal registers as well as external communication interfaces
/* #include "registers.h" */
uint16_t holding_reg[REG_END];
uint16_t coils_state; // max 16 coils, actual defined in registers.h

////// [todo] peripheral nodes
static const struct device *dev_sht3xa;;

////// internal status polling
void status_work_handler(struct k_work *work) {
  /* test */
  if (coils_state & BIT(COIL_ONOFF_CH1)) {
    coils_state &= ~BIT(COIL_ONOFF_CH1);
  } else {
    coils_state |= BIT(COIL_ONOFF_CH1);
  }

  /* 1. read present V & I */

  /* 2. read present temepature & humidity */
  struct sensor_value temp, hum;
  sensor_sample_fetch(dev_sht3xa);
  sensor_channel_get(dev_sht3xa, SENSOR_CHAN_AMBIENT_TEMP, &temp);
  sensor_channel_get(dev_sht3xa, SENSOR_CHAN_HUMIDITY, &hum);
  holding_reg[REG_TEMPERATURE_INT] = temp.val1 & BIT_MASK(16);
  holding_reg[REG_TEMPERATURE_FLT] = temp.val2 & BIT_MASK(16); // = flt/1000000
  holding_reg[REG_HUMIDITY_INT] = hum.val1 & BIT_MASK(16);
  holding_reg[REG_HUMIDITY_FLT] = hum.val2 & BIT_MASK(16); // = flt/1000000

  /* 3. trip or not */
  if (!(coils_state & BIT(COIL_TRIPPED_CH1))) {
    // check trip condition & do trip
  }
}

K_WORK_DEFINE(status_work, status_work_handler);

static void status_timer_callback(struct k_timer *timer) {
  /* submit to system work queue */
  k_work_submit(&status_work);
}

static K_TIMER_DEFINE(status_timer, status_timer_callback, NULL);

////// alive counts update
static void alive_timer_callback(struct k_timer *timer) {
  uint32_t _cv = ++alive_counts;
  holding_reg[REG_ALIVE_LW] = _cv & BIT_MASK(16);
  holding_reg[REG_ALIVE_HI] = _cv >> 16;
}

static K_TIMER_DEFINE(alive_timer, alive_timer_callback, NULL);

////// k_work to process coil/register commands
struct ctrl_cmd_t {
  struct k_work work;
  uint8_t addr;
};

static struct ctrl_cmd_t coil_cmd = {.addr = COIL_END};
void coil_work_handler(struct k_work *item) {
  struct ctrl_cmd_t *the_cmd = CONTAINER_OF(item, struct ctrl_cmd_t, work);
  switch (the_cmd->addr) {
  case COIL_ONOFF_CH2:
    /* test */
    if (coils_state & BIT(COIL_ONOFF_CH2)) {
      holding_reg[REG_PVV_CH2] = 111;
    } else {
      holding_reg[REG_PVV_CH2] = 0;
    }
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

static struct ctrl_cmd_t register_cmd = {.addr = REG_END};
void register_work_handler(struct k_work *item) {
  struct ctrl_cmd_t *the_cmd = CONTAINER_OF(item, struct ctrl_cmd_t, work);
  switch (the_cmd->addr) {
  case REG_SVV_CH3:
    /* test */
    holding_reg[REG_PVV_CH3] = 333;
    // check the coil state
    printk("reset the boards");
    break;
  case REG_UPDATE_INTERVAL:
    k_timer_start(&status_timer, K_NO_WAIT,
                  K_MSEC(holding_reg[REG_UPDATE_INTERVAL]));
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

  // cmds without effects, just return
  if ((addr == REG_UPDATE_INTERVAL) && (reg == holding_reg[addr])) {
    return 0;
  }

  //
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

///////// main thread
int main(void) {
  /* [todo] internal registers init */
  alive_counts=0;
  holding_reg[REG_UPDATE_INTERVAL] = STATUS_UPDATE_PERIOD;

  coils_state = 0x0000;

  /* peripheral device init (not real init, just fetch existing devices already initialized) */
  dev_sht3xa = DEVICE_DT_GET(SHT3XA_NODE);
  if (!device_is_ready(dev_sht3xa)) {
    printk("\nError: Device \"%s\" is not ready; "
           "check the driver initialization logs for errors.\n",
           dev_sht3xa->name);
    return -ENOENT;
  }

  /* work init */
  k_work_init(&coil_cmd.work, coil_work_handler);
  k_work_init(&register_cmd.work, register_work_handler);

  /* modbus server init */
  if (init_modbus_server()) {
    printk("Modbus RTU server initialization failed");
  }

  /* status polling loop init */
  k_timer_start(&status_timer, K_NO_WAIT,
                K_MSEC(holding_reg[REG_UPDATE_INTERVAL]));

  /* alive counting init */
  k_timer_start(&status_timer, K_NO_WAIT, K_MSEC(ALIVE_COUNTING_PERIOD));

  return 0;
}
