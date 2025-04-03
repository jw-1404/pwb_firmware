#pragma once

/*** holding registers ***/

// voltage value
#define REG_PVV_CH0 0
#define REG_PVV_CH1 1
#define REG_PVV_CH2 2
#define REG_PVV_CH3 3
#define REG_PVV_CH4 4
#define REG_PVV_CH5 5
#define REG_PVV_CH6 6
#define REG_PVV_CH7 7
#define REG_PVV_CH8 8
#define REG_PVV_CH9 9

// current value
#define REG_PVI_CH0 10
#define REG_PVI_CH1 11
#define REG_PVI_CH2 12
#define REG_PVI_CH3 13
#define REG_PVI_CH4 14
#define REG_PVI_CH5 15
#define REG_PVI_CH6 16
#define REG_PVI_CH7 17
#define REG_PVI_CH8 18
#define REG_PVI_CH9 19

// measuring interval (in ms)
#define REG_MEASURE_INTERVAL 100

// system alive time (high-16 bits & low-16 bits)
#define REG_HEARTBEAT_LW 21
#define REG_HEARTBEAT_HI 22

#define REG_END 23

/*** coils ***/

// turn voltage on/off
#define COIL_ONOFF_CH0 0
#define COIL_ONOFF_CH1 1
#define COIL_ONOFF_CH2 2
#define COIL_ONOFF_CH3 3
#define COIL_ONOFF_CH4 4
#define COIL_ONOFF_CH5 5
#define COIL_ONOFF_CH6 6
#define COIL_ONOFF_CH7 7
#define COIL_ONOFF_CH8 8
#define COIL_ONOFF_CH9 9

#define COIL_END 10
