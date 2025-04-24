#include <stddef.h>
#include "text_UI.h"
#include "font.h"
#include "LCD.h"
#include "threads.h"
#include "sound.h"
#include "i2c.h"
#include "MMA8451.h"

void UI_Audio_TF(UI_Control_t * ctl), UI_Accel_TF(UI_Control_t * ctl),UI_RGB_TF(UI_Control_t * ctl), UI_LCD_TF(UI_Control_t * ctl), UI_MCU_TF(UI_Control_t * ctl);
char BlankStateLabel[] = "            ";

UI_Control_t UIC_Audio = {
	.CtlLabel = "Audio",	.NumStates = 2,	.State = 0,	
	.StateLabel = {"Off", "On"},	.TransitionFunction = UI_Audio_TF,
}, 
UIC_RGB = {
	.CtlLabel = "RGB Mode",	.NumStates = 2,	.State = 1,	
	.StateLabel = {"Cycle", "Pause"},	.TransitionFunction = UI_RGB_TF, 
},
UIC_LCD = {
	.CtlLabel = "LCD PSv",	.NumStates = 4,	.State = 0,	
	.StateLabel = {"None", "BL Dim", "BL Off", "BL+Ctlr Off"},	.TransitionFunction = UI_LCD_TF,
},
UIC_MCU = {
	.CtlLabel = "MCU PSv",	.NumStates = 2,	.State = 0,	
	.StateLabel = {"None", "Wait"},	.TransitionFunction = UI_MCU_TF, 
},
UIC_Acc = {
	.CtlLabel = "Accel",	.NumStates = 2,	.State = 0,	
	.StateLabel = {"Active", "Standby"},	.TransitionFunction = UI_Accel_TF, 
};

UI_Control_t * UICs[UI_N_CTLS] = {&UIC_RGB, &UIC_Acc, &UIC_LCD, &UIC_MCU, &UIC_Audio};

// Transition functions have code which runs upon entry to state
void UI_Audio_TF(UI_Control_t * ctl) {
	if (ctl->State)
		Sound_Enable_Amp(1);
	else
		Sound_Enable_Amp(0);	
}
void UI_RGB_TF(UI_Control_t * ctl) {
	if (ctl->State == 0) {
		RGB_cycle = 1;
		osTimerStart(LED_timer, TIMER_LED_PERIOD_MS);
	} else {
		RGB_cycle = 0;
		osTimerStop(LED_timer);
	}
}

void UI_Accel_TF(UI_Control_t * ctl) {
	if (ctl->State==0) {
		enable_accel = 1;
		// Select Active mode
	  i2c_write_byte(MMA_ADDR, REG_CTRL1, 0x05); // 0x1D
	}	else {
		enable_accel = 0;
		// Select Standby mode
	  i2c_write_byte(MMA_ADDR, REG_CTRL1, 0x04); 
	}
}

void UI_MCU_TF(UI_Control_t * ctl) {
	if (ctl->State == 1)
		enable_wfi = 1;
	else
		enable_wfi = 0;
}
void UI_LCD_TF(UI_Control_t * ctl) {
	switch (ctl->State) {
		default:
			ctl->State = 0;
			// and go right into state 0
		case 0: // None
			LCD_BL_auto_off = 0;
			LCD_BL_PSave_brightness = LCD_BL_BR_FULL; // No power saving
			LCD_Ctlr_auto_off = 0;
			break;
		case 1: // BL Dims after time-out
			LCD_BL_auto_off = 1;
			LCD_BL_time_left = LCD_BL_TIME_OUT;
			LCD_BL_PSave_brightness = LCD_BL_BR_DIM; // After BL timeout, dim BL
			LCD_Ctlr_auto_off = 0;
			break;
		case 2: // BL turns off after time-out
			LCD_BL_auto_off = 1;
			LCD_BL_time_left = LCD_BL_TIME_OUT;
			LCD_BL_PSave_brightness = 0;  // After BL timeout, turn off BL
			LCD_Ctlr_auto_off = 0;
			break;
		case 3: // BL & Ctlr Off
			LCD_BL_auto_off = 1;
			LCD_BL_time_left = LCD_BL_TIME_OUT;
			LCD_BL_PSave_brightness = 0;  // After BL timeout, turn off BL
			LCD_Ctlr_auto_off = 1; // After LCD Ctlr timeout, turn off Ctlr
			LCD_Ctlr_time_left = LCD_CTLR_TIME_OUT;
			break;
	}
}
// End of transition functions

void UI_Process_Touch(PT_T * p){
	int enable=1, row, n=-1;
	
	row = ((p->Y + (UI_ROWS_PER_CTL-1)*CHAR_HEIGHT/2)/CHAR_HEIGHT); // Round up lower portion of row 
	
	for (int i = 0; (n < 0) && (i < UI_N_CTLS); i++) {
		if ((row >= UICs[i]->Row) & (row <= UICs[i]->LastRow))
				n = i;
	}
	if (n > -1) { // Valid touch on control
		if (UICs[n]->State >= UICs[n]->NumStates-1) // Wrap around if state would inc. past NumStates-1
				UICs[n]->State = 0;
		else
				UICs[n]->State++;

		if (UICs[n]->TransitionFunction != NULL)
			UICs[n]->TransitionFunction(UICs[n]);
		UI_Draw_Labels(n);
	}
}

void UI_Init(void) {
	int n=0, r=UI_START_ROW;
	for (n=0; n<UI_N_CTLS; n++) {
		// fill in row numbers here
		UICs[n]->Row = r;
		r += UI_ROWS_PER_CTL;
		UICs[n]->LastRow = r-1;
		
		// Perform actions for entering initial states
		if (UICs[n]->TransitionFunction != NULL)
			UICs[n]->TransitionFunction(UICs[n]);
	}
}

// Draw label n, or all labels if n < 0
void UI_Draw_Labels(int n) {
	if (n >= 0) {
		LCD_Text_PrintStr_RC(UICs[n]->Row, 0, UICs[n]->CtlLabel);
		LCD_Text_PrintStr_RC(UICs[n]->Row, UI_STL_COL, BlankStateLabel);
		LCD_Text_PrintStr_RC(UICs[n]->Row, UI_STL_COL, UICs[n]->StateLabel[UICs[n]->State]);
	} else {
		for (n=0; n < UI_N_CTLS; n++) {
			LCD_Text_PrintStr_RC(UICs[n]->Row, 0, UICs[n]->CtlLabel);
			LCD_Text_PrintStr_RC(UICs[n]->Row, UI_STL_COL, BlankStateLabel);
			LCD_Text_PrintStr_RC(UICs[n]->Row, UI_STL_COL, UICs[n]->StateLabel[UICs[n]->State]);
		}
	}
}
