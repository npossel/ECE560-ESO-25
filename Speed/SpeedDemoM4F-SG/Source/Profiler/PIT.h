#ifndef PIT_H
#define PIT_H
#include "MK64F12.h"

void PIT0_Init(unsigned period);
void PIT0_Start(void);
void PIT0_Stop(void);

#endif
