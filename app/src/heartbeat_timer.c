#include "common.h"

#include <zephyr/kernel.h>

// in ms
#define HB_COUNTING_PERIOD 1000

static uint32_t hb_counts = 0;

/* heartbeat timer */
static void hb_timer_callback(struct k_timer *timer) {
  uint32_t _cv = ++hb_counts;
  holding_reg[REG_HEARTBEAT_LW]->value = _cv & BIT_MASK(16);
  holding_reg[REG_HEARTBEAT_HI]->value = _cv >> 16;
}

static K_TIMER_DEFINE(hb_timer, hb_timer_callback, NULL);

/* start the timer */
static int heartbeat_init(void) {
  k_timer_start(&hb_timer, K_NO_WAIT, K_MSEC(HB_COUNTING_PERIOD));
  return 0;
}

SYS_INIT(heartbeat_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
