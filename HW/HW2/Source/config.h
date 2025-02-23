#ifndef CONFIG_H
#define CONFIG_H

// Platform
// Select one compiler
// #define USING_AC5
#define USING_AC6

// #define USING_RTOS

// Application Program
#define NUM_TESTS 										(200)
#define LAZY_TEXT_UPDATE  						(0) // Initially 0
#define LAZY_GRAPHICS_UPDATE 					(0) // Initially 0

#define UPDATE_TEXT 									(1) // Initially 1
#define UPDATE_GRAPHICS 							(1) // Initially 1

#define SIM_ACCEL_XYZ_DATA 						(0) // Initially 0
#define SIM_ACCEL_STEP 								(0.01)

#define LCD_BUS_DEFAULTS_TO_DATA 			(0) // Initially 0
#define DRAW_LINE_RUNS_AS_RECTANGLES 	(0) // Initially 0
#define USE_TEXT_BITMAP_RUNS 					(0) // Initially 0

#define READ_FULL_XYZ 								(0) // Initially 0
#define I2C_ICR_VALUE 								(0x20) // Initially 0x20

#endif
