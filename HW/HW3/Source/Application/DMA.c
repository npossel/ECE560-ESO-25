#include <stdint.h>
#include <MKL25Z4.h>
#include <cmsis_os2.h>

#include "timers.h"
#include "LEDs.h"
#include "threads.h"
#include "gpio_defs.h"
#include "debug.h"
#include "sound.h"

uint16_t * Reload_DMA_Source[2]={0,0};
uint32_t Reload_DMA_Byte_Count=0;
uint32_t DMA_Playback_Count=0;

void DMA_Init(void) {
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
}

void Configure_DMA_For_Playback(uint16_t * source1, uint16_t * source2, uint32_t count, uint32_t num_playbacks) {
	
	// Disable DMA channel in order to allow changes
	DMAMUX0->CHCFG[0] = 0;

	Reload_DMA_Source[0] = source1;
	Reload_DMA_Source[1] = source2;

	Reload_DMA_Byte_Count = count*2;
	DMA_Playback_Count = num_playbacks;
	
	// Generate DMA interrupt when done
	// Increment source, transfer words (16 bits)
	// Enable peripheral request
	DMA0->DMA[0].DCR = DMA_DCR_EINT_MASK | DMA_DCR_SINC_MASK | 
											DMA_DCR_SSIZE(2) | DMA_DCR_DSIZE(2) |
											DMA_DCR_ERQ_MASK | DMA_DCR_CS_MASK;
											// DMA_DCR_D_REQ_MASK;  // DON'T disable peripheral request when done

	
	// Configure NVIC for DMA ISR
	NVIC_SetPriority(DMA0_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(DMA0_IRQn); 
	NVIC_EnableIRQ(DMA0_IRQn);	

	// Select TPM2 overflow as trigger via mux
	// Don't use periodic triggering
	DMAMUX0->CHCFG[0] = DMAMUX_CHCFG_SOURCE(56);   
}

int32_t Get_DMA_Transfers_Completed(uint32_t ch) {
	// Get progress from byte count register
	int32_t bytes_xferred = Reload_DMA_Byte_Count - (DMA0->DMA[ch].DSR_BCR & DMA_DSR_BCR_BCR_MASK);
	if (bytes_xferred < 0)
		return -1;
	switch ((DMA0->DMA[ch].DCR & DMA_DCR_SSIZE_MASK) >> DMA_DCR_SSIZE_SHIFT)  {
		case 0: 
			return bytes_xferred/4;
			break;
		case 1: 
			return bytes_xferred;
			break;
		case 2: 
			return bytes_xferred/2;
			break;
		default:
			return -1;
			break;
	}
}

void Start_DMA_Playback() {
	// Select TPM2 overflow as trigger for DMA
	DMAMUX0->CHCFG[0] = DMAMUX_CHCFG_SOURCE(56);   
	// initialize source and destination pointers
#if USE_DOUBLE_BUFFER
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) Reload_DMA_Source[1 - WriteSBNum]);
#else
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) Reload_DMA_Source[0]);
#endif
	DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) (&(DAC0->DAT[0])));
	// byte count
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(Reload_DMA_Byte_Count);
	// verify done flag is cleared
	DMA0->DMA[0].DSR_BCR &= ~DMA_DSR_BCR_DONE_MASK; 
	// Enable DMA
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;
	// start the timer running
	TPM2_Start();
}


void DMA0_IRQHandler(void) {
	DEBUG_START(DBG_ISR_DMA);
	DEBUG_START(DBG_REFILL_PENDING);
	// Clear done flag 
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_DONE_MASK; 
	/// Prep for next DMA transfers
	// Reinitialize source and destination pointers
#if USE_DOUBLE_BUFFER		
	// switch to other buffer
	WriteSBNum = 1 - WriteSBNum;
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) Reload_DMA_Source[1 - WriteSBNum]);
#else
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) Reload_DMA_Source[0]);
#endif
	// Reload byte count
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(Reload_DMA_Byte_Count);
	// Signal to OS event requesting source buffer refill
	osThreadFlagsSet(t_Refill_SoundBuffer, EV_REFILL_SOUND_BUFFER);
	DEBUG_STOP(DBG_ISR_DMA);
	}
