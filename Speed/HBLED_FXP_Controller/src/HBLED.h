#ifndef HBLED_H
#define HBLED_H

// Switching parameters
#define PWM_HBLED_CHANNEL (4)
#define PWM_PERIOD (2000) 
/* 48 MHz input clock. 
	PWM frequency = 48 MHz/(PWM_PERIOD*2) 
	Timer is in count-up/down mode. */

#define LIM_DUTY_CYCLE (PWM_PERIOD)

// Control approach configuration - select only one
#define USE_ASYNC_SAMPLING 	0
#define USE_SYNC_NO_FREQ_DIV 		1
#define USE_SYNC_SW_CTL_FREQ_DIV 0
// Not working yet
#define USE_SYNC_HW_CTL_FREQ_DIV 0

#define SW_CTL_FREQ_DIV_FACTOR (1) // Software division in ISR
#define HW_CTL_FREQ_DIV_CODE (0) // Not used

#if USE_ASYNC_SAMPLING
#define 	USE_TPM0_INTERRUPT 0
#define 	USE_ADC_HW_TRIGGER 0
#define 	USE_ADC_INTERRUPT 1
#endif

#if USE_SYNC_NO_FREQ_DIV
#define 	USE_TPM0_INTERRUPT 0
#define 	USE_ADC_HW_TRIGGER 1
#define 	USE_ADC_INTERRUPT 1
#endif

#if USE_SYNC_SW_CTL_FREQ_DIV
#define 	USE_TPM0_INTERRUPT 1
#define 	USE_ADC_HW_TRIGGER 0
#define 	USE_ADC_INTERRUPT 1
#endif

#if USE_SYNC_HW_CTL_FREQ_DIV
#define 	USE_TPM0_INTERRUPT 0
#define 	USE_ADC_HW_TRIGGER 1
#define 	USE_ADC_INTERRUPT 1
#endif

// Control Parameters
// default control mode: OpenLoop, BangBang, Incremental, PID, PID_FX
#define DEF_CONTROL_MODE (PID_FX)

// Incremental controller: change amount
#define INC_STEP (PWM_PERIOD/40)

// Proportional Gain, scaled by 2^8
#define PGAIN_8 (0x0028)

// PID (floating-point) gains. Guaranteed to be non-optimal. 
#define I_GAIN_FL (0.000f)
#define P_GAIN_FL (0.600f)
#define D_GAIN_FL (0.000f)

// PID_FX (fixed-point) gains. Guaranteed to be non-optimal. 
#define I_GAIN_FX (FL_TO_FX(0.0065f))
#define P_GAIN_FX (FL_TO_FX(0.0626f))
#define D_GAIN_FX (FL_TO_FX(0.1831f))

#define FLASH_PERIOD (20)
#define FLASH_CURRENT_MA (140)

// Hardware configuration
#define ADC_SENSE_CHANNEL (8)

#define R_SENSE (2.2f)
#define R_SENSE_MO ((int) (R_SENSE*1000))

#define V_REF (3.3f)
#define V_REF_MV ((int) (V_REF*1000))

#define ADC_FULL_SCALE (0x10000)
#define MA_SCALING_FACTOR (1000)

#define DAC_POS 30
#define DAC_RESOLUTION 4096

// #define MA_TO_DAC_CODE(i) (i*2.2*DAC_RESOLUTION/V_REF_MV) // Introduces timing delay and interesting bug!
#define MA_TO_DAC_CODE(i) ((i)*(2.2f*DAC_RESOLUTION/V_REF_MV))

#define MIN(a,b) ((a<b)?a:b)
#define MAX(a,b) ((a>b)?a:b)

#endif // HBLED_H
