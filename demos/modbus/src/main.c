#include <zephyr/kernel.h>
#include <zephyr/modbus/modbus.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

static uint16_t holding_reg[8];
static uint8_t coils_state;

static int coil_rd(uint16_t addr, bool *state)
{
	if (addr >= 8) {
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

static int coil_wr(uint16_t addr, bool state)
{
	bool on;

	if (addr >= 8) {
		return -ENOTSUP;
	}


	if (state == true) {
		coils_state |= BIT(addr);
		on = true;
	} else {
		coils_state &= ~BIT(addr);
		on = false;
	}

	printk("Coil write, addr %u, %d", addr, (int)state);

	return 0;
}

static int holding_reg_rd(uint16_t addr, uint16_t *reg)
{
	if (addr >= ARRAY_SIZE(holding_reg)) {
		return -ENOTSUP;
	}

	*reg = holding_reg[addr];

	printk("Holding register read, addr %u", addr);

	return 0;
}

static int holding_reg_wr(uint16_t addr, uint16_t reg)
{
	if (addr >= ARRAY_SIZE(holding_reg)) {
		return -ENOTSUP;
	}

	holding_reg[addr] = reg;

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
	.server = {
		.user_cb = &mbs_cbs,
		.unit_id = 1,
	},
	.serial = {
		.baud = 19200,
		.parity = UART_CFG_PARITY_NONE,
	},
};

#define MODBUS_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(zephyr_modbus_serial)

static int init_modbus_server(void)
{
	const char iface_name[] = {DEVICE_DT_NAME(MODBUS_NODE)};
	int iface;

	iface = modbus_iface_get_by_name(iface_name);

	if (iface < 0) {
		printk("Failed to get iface index for %s", iface_name);
		return iface;
	}

	return modbus_init_server(iface, server_param);
}

int main(void)
{
  coils_state = 0xA9; // b10101001
  for (int i=0; i<8; i++) {
    holding_reg[i]=i+1;
  }

	if (init_modbus_server()) {
		printk("Modbus RTU server initialization failed");
  }
  printk("Modbus RTU server initialization success");
  return 0;
}
