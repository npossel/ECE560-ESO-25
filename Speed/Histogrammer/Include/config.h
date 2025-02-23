#ifndef CONFIG_H
#define CONFIG_H

#include "GPIO_defs.h"

// Platform
// Select one compiler
// #define USING_AC5
#define USING_AC6

#define USING_RTOS

// Application Program
#define USE_ADC_SERVER 				(0)

// LCD and Graphics Optimizations
#define LCD_BUS_DEFAULTS_TO_DATA 1 
#define DRAW_LINE_RUNS_AS_RECTANGLES 1 
#define USE_TEXT_BITMAP_RUNS 1 

#endif // CONFIG_H
