#ifndef TIMERS_H
#define TIMERS_H
#include "MK64F12.h"

void TPM_Init(unsigned period_ms);

void TPM0_Init(void);
void TPM0_Start(void);
void Configure_TPM0_for_DMA(uint32_t period_us);

void PWM_Init(TPM_Type * TPM, uint8_t channel_num, uint16_t period, uint16_t duty);
void PWM_Set_Value(TPM_Type * TPM, uint8_t channel_num, uint16_t value);

void LPTMR_Init(void);
void LPTMR_Start(void);
void LPTMR_Stop(void);

#endif
