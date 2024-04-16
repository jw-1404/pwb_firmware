#include "common.h"
#include <zephyr/kernel.h>

// in ms
#define STATUS_UPDATE_PERIOD 1000

/* work handler for polling timer */
static void status_work_handler(struct k_work *work) {
  POLL_HANDLER_FOREACH(handler) { handler->callback(); }

  /* test */
  coil_reg[COIL_ONOFF_CH1]->value = !coil_reg[COIL_ONOFF_CH1]->value;

  /* 1. read present V & I */

  /* 2. read present temepature & humidity */

  /* 3. trip or not */
  return;
}

K_WORK_DEFINE(status_work, status_work_handler);

static void status_timer_callback(struct k_timer *timer) {
  /* immediately submit to system work queue */
  k_work_submit(&status_work);
}

static K_TIMER_DEFINE(status_timer, status_timer_callback, NULL);

/* holding register handler */
static void status_holding_register_handler(uint8_t addr) {
  if (addr == REG_UPDATE_INTERVAL) {
    k_timer_start(&status_timer, K_NO_WAIT, K_MSEC(holding_reg[addr]->value));
  }
  return;
}

HOLDING_REG_HANDLER_DEFINE(status_holding_reg, status_holding_register_handler);

/* start the timer */
static int status_init(void) {
  holding_reg[REG_UPDATE_INTERVAL]->value = STATUS_UPDATE_PERIOD;

  k_timer_start(&status_timer, K_NO_WAIT,
                K_MSEC(holding_reg[REG_UPDATE_INTERVAL]->value));

  return 0;
}

// [todo] need lower priority than peripherals init
SYS_INIT(status_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
