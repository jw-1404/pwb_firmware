# [totest] flash & debug toolchain

board_runner_args(jlink "--device=STM32F051R8" "--speed=4000")
board_runner_args(jlink "--device=STM32F103CB" "--speed=4000")

include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
