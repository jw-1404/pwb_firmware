
zephyr_library()

zephyr_library_sources_ifdef(CONFIG_DAC_AD5541 ad5541.c)

