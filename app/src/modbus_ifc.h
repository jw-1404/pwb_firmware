#pragma once

/*** holding registers ***/

/// status registers
// voltage set value (V)
#define REG_SVV_CH1 0
#define REG_SVV_CH2 1
#define REG_SVV_CH3 2
#define REG_SVV_CH4 3

// voltage present value (V)
#define REG_PVV_CH1 4
#define REG_PVV_CH2 5
#define REG_PVV_CH3 6
#define REG_PVV_CH4 7

// current present value (uA)
#define REG_PVI_CH1 8
#define REG_PVI_CH2 9
#define REG_PVI_CH3 10
#define REG_PVI_CH4 11

// trip current settings (uA)
#define REG_TRIP_CH1 12
#define REG_TRIP_CH2 13
#define REG_TRIP_CH3 14
#define REG_TRIP_CH4 15

// ramp up/down settings (step in V)
#define REG_RUP_STEP 16
#define REG_RDN_STEP 17

// status polling interval (in ms)
#define REG_UPDATE_INTERVAL 18

// system alive time (high-16 bits & low-16 bits)
/// alive-time = polling-times * update_interval
#define REG_ALIVE_LW 19
#define REG_ALIVE_HI 20

// others
#define REG_TEMPERATURE_INT 21
#define REG_TEMPERATURE_FLT 22
#define REG_HUMIDITY_INT 23
#define REG_HUMIDITY_FLT 24
// 64-bit uuid, addr range (little endian): 25-28
#define REG_BOARD_UUID 25

/// control command registers
#define REG_CLR_TRIPS 29
#define REG_RESET 30

#define REG_END 31

/*** coils ***/

/// status coils
// voltage tripped or not
#define COIL_TRIPPED_CH1 0
#define COIL_TRIPPED_CH2 1
#define COIL_TRIPPED_CH3 2
#define COIL_TRIPPED_CH4 3

/// control command coils
// turn voltage on/off
#define COIL_ONOFF_CH1 4
#define COIL_ONOFF_CH2 5
#define COIL_ONOFF_CH3 6
#define COIL_ONOFF_CH4 7

#define COIL_END 8
