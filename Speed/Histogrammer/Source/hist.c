#include <MKL25Z4.h>

#include "hist.h"
#include "font.h" // for sim data

#include "LCD.h"
#include "LCD_driver.h"
#include "font.h"


volatile HIST_T hist;

void Hist_Init(void) {
	hist.BinWidth = 1;
	hist.MaxCount = 0;
	hist.VScaleFactor = 1;

	for (int n = 0; n < HIST_NUM_BINS; n++) {
		hist.Bin[n].Count = 0;
		hist.Bin[n].New = 0;
	}
}

int Hist_Bin_Clip(int n) {
	if (n >= HIST_NUM_BINS)
		n = HIST_NUM_BINS - 1;
	return n;
}

int Hist_Bin_Valid(int n) {
	return ((n < 0) ||  (n >= HIST_NUM_BINS))? 0 : 1;
}

int Hist_Add_Sample(int time) {
	int n;
	if (time < 0)	return 0;
	n = time/hist.BinWidth;
	n = Hist_Bin_Clip(n);
	hist.Bin[n].Count++;
	hist.Bin[n].New = 1;
	if (hist.Bin[n].Count > hist.MaxCount)
		hist.MaxCount = hist.Bin[n].Count;
	return 1;
}

int Hist_Read_Bin(int n) {
	int ct = -1;
	if (Hist_Bin_Valid(n)) {
		ct = hist.Bin[n].Count;
		hist.Bin[n].New = 0;
	} 
	return ct;
}


void TPM0_IRQHandler() {
	static uint32_t t1=0, t2=0;
	if (TPM0->STATUS & TPM_STATUS_CH0F_MASK) {
		t1 = TPM0->CONTROLS[0].CnV; 
		TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK; 
	}
	if (TPM0->STATUS & TPM_STATUS_CH1F_MASK) {
		t2 = TPM0->CONTROLS[1].CnV;
		// read pulse width from timer peripheral
		TPM0->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK; 
		Hist_Add_Sample(t2-t1);
	}
}

void Hist_Add_Sim_Samples(int n) {
	static const uint8_t * data_ptr = P_LUCIDA_CONSOLE8x13;
	while (n--) {
		Hist_Add_Sample(((int) *data_ptr) & 0x3f);
		data_ptr++;
	}
}


void Hist_Update_Display(int full_update) {
	PT_T p1, p2;
	COLOR_T fg = green, bg=black;
	
	if (full_update) {
		LCD_Erase();
		// Hist_Draw_Grid();
	}
	int redraw = 0;
	
	do {
		// redraw? may need to erase LCD
		redraw = 0;
		p1.X = 0;
		p2.X = 1;
		for (int n = 0; n < HIST_NUM_BINS; n++) {
			// Top half: empty
			p1.Y = HIST_PLOT_TOP;
			p2.Y = BIN_TO_Y(hist.Bin[n].Count); 
			if (p2.Y < 0) {
				// Overflow!
				p2.Y = 0;
/*				hist.VScaleFactor *= 2;		redraw = 1; 	break; // Restart! */
			}
			LCD_Fill_Rectangle(&p1, &p2, &bg);
				
			// Bottom half: full
			p1.Y = HIST_PLOT_BOTTOM;
			LCD_Fill_Rectangle(&p1, &p2, &fg);

			p1.X += 3;
			p2.X += 3;
		}
	} while (redraw);
}

void new_Hist_Update_Display(int full_update) {
	PT_T p1, p2;
	COLOR_T fg = green, bg=black;
	
	if (full_update) {
		LCD_Erase();
		// Hist_Draw_Grid();
	}
	int redraw = 0;
	
	do {
		// redraw? may need to erase LCD
		redraw = 0;
		p1.X = 0;
		p2.X = 1;
		for (int n = 0; n < HIST_NUM_BINS; n++) {
			// p1.X = BIN_TO_X(n); 
			// p2.X = BIN_TO_X(n+1);
			
			// Top half: empty
			p1.Y = HIST_PLOT_TOP;
			// p2.Y = BIN_TO_Y(n); 
			//			int c = hist.Bin[n].Count;
			p2.Y = BIN_TO_Y(hist.Bin[n].Count); 
			if (p2.Y < 0) {
				// Overflow!
				p2.Y = 0;
				hist.VScaleFactor *= 2;
				redraw = 1;
				break; // Restart!
			}
			LCD_Fill_Rectangle(&p1, &p2, &bg);
				
			// Bottom half: full
			p1.Y = HIST_PLOT_BOTTOM;
			LCD_Fill_Rectangle(&p1, &p2, &fg);

			p1.X += 3;
			p2.X += 3;
		}
	} while (redraw);
}
