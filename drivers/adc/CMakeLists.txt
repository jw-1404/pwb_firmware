# zephyr_syscall_header(${ZEPHYR_BASE}/include/zephyr/drivers/adc.h)

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_ADC_LTC2450 ltc2450.c)
