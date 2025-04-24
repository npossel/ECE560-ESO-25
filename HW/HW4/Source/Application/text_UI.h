#ifndef TEXT_UI_H
#define TEXT_UI_H

#include <stdint.h>
#include "LCD.h"

#define UI_START_ROW 	(3) 
#define UI_N_CTLS 		(5)
#define UI_ROWS_PER_CTL (2)
#define MAX_STATES 		(5)
#define UI_STL_COL 		(8)

typedef struct UI_Control_t {
	char CtlLabel[10];
	uint8_t NumStates;
	uint8_t State;
	uint8_t Row, LastRow;
	char StateLabel[MAX_STATES][12];
	void (* TransitionFunction)(struct UI_Control_t *);
} UI_Control_t;


extern UI_Control_t * UICs[UI_N_CTLS];

void UI_Init(void);
void UI_Draw_Labels(int n);
void UI_Process_Touch(PT_T * p);

#endif // TEXT_UI_H