#ifndef HIST_H
#define HIST_H
#include <stdint.h>
#include "LCD.h"
#include "LCD_driver.h"
#include "ST7789.h"

#define HIST_NUM_BINS (80)
#define HIST_BIN_WIDTH_DEFAULT (10)

// Display Configuration
#define HIST_UI_CONTROLS_HEIGHT (64)

#define HIST_PLOT_WIDTH (LCD_WIDTH)
#define HIST_PLOT_TOP (0)
#define HIST_PLOT_BOTTOM (LCD_HEIGHT - HIST_UI_CONTROLS_HEIGHT - 1)
#define HIST_PLOT_HEIGHT (HIST_PLOT_BOTTOM - HIST_PLOT_TOP + 1)

#define BIN_TO_X(b) ((b*HIST_PLOT_WIDTH)/(HIST_NUM_BINS))
#define HIST_SCALE_Y(v) ((v) / hist.VScaleFactor)
#define BIN_TO_Y(v) (HIST_PLOT_HEIGHT - HIST_SCALE_Y(v))


typedef struct {
	struct {
		uint32_t Count:31; // starts with LSB
		uint32_t New:1; // is MSB, allows test for negative
	}	Bin[HIST_NUM_BINS];
	int MaxCount;
	int BinWidth;
	int VScaleFactor;
} HIST_T;

extern volatile HIST_T hist;

void Hist_Init(void);
int Hist_Read_Bin(int n);
int Hist_Add_Sample(int time);
void Hist_Add_Sim_Samples(int n);

void Hist_Update_Display(int full_update);


#endif // HIST_H
