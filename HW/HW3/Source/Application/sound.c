#include <stdint.h>
#include <MKL25Z4.h>
#include <math.h>
#include <cmsis_os2.h>

#include "config.h"
#include "sound.h"
#include "misc.h"
#include "delay.h"
#include "gpio_defs.h"
#include "LEDs.h"
#include "timers.h"
#include "DMA.h"
#include "threads.h"
#include "game.h"
#include "debug.h"

int16_t SineTable[NUM_STEPS];
uint16_t SoundBuffer[2][NUM_SAMPLES_PER_SOUND_BUFFER];
uint8_t WriteSBNum = 0; // Which SoundBuffer is currently being written

VOICE_T Voice[NUM_VOICES];
const uint16_t ScalePeriods[] = {	
	1223, 1090, 971, 916, 816, 727, 648,	// 0-6
	612, 545, 485, 458, 408, 364, 324,		// 7-13
	306, 272, 243, 229, 204, 182, 162, 		// 14-20
	153, 136, 121, 115, 102, 91, 81, 			// 21-27
	76, 68, 61, 57, 51, 45, 40, 38, 			// 28-34
	34, 30, 29, 26, 23, 20, 19, 17, 			// 35-41
	15, 14, 13, 11, 10, 10, 9, 8, 				// 42-48
	7, 6, 6, 5, 5, 4, 4, 4, 3, 						// 49-55
	3, 3, 2};															// 56-58

// RTOS Objects
osThreadId_t t_Sound_Manager, t_Refill_SoundBuffer;

const osThreadAttr_t Sound_Manager_attr = {
  .priority = THREAD_SM_PRIO 
}; 
const osThreadAttr_t Refill_SoundBuffer_attr = {
  .priority = THREAD_RSB_PRIO 
};

osMutexId_t Voice_mutex;

const osMutexAttr_t Voice_mutex_attr = {
  "Voice_mutex",     // human readable mutex name
  osMutexPrioInherit    // attr_bits
};

// Functions
void DAC_Init(void) {
  // Init DAC output
	
	SIM->SCGC6 |= MASK(SIM_SCGC6_DAC0_SHIFT); 
	SIM->SCGC5 |= MASK(SIM_SCGC5_PORTE_SHIFT); 
	
	PORTE->PCR[DAC_POS] &= ~PORT_PCR_MUX_MASK;	
	PORTE->PCR[DAC_POS] |= PORT_PCR_MUX(0);	// Select analog 
		
	// Disable buffer mode
	DAC0->C1 = 0;
	DAC0->C2 = 0;
	
	// Enable DAC, select VDDA as reference voltage
	DAC0->C0 = MASK(DAC_C0_DACEN_SHIFT) | MASK(DAC_C0_DACRFS_SHIFT);
}

/*
	Code for driving DAC
*/
void Play_Sound_Sample(uint16_t val) {
	DAC0->DAT[0].DATH = DAC_DATH_DATA1(val >> 8);
	DAC0->DAT[0].DATL = DAC_DATL_DATA0(val);
}

void SineTable_Init(void) {
	unsigned n;
	
	for (n=0; n<NUM_STEPS; n++) {
		SineTable[n] = (MAX_DAC_CODE/2)*sinf(n*(2*3.1415927/NUM_STEPS));
	}
}

/* Fill waveform buffers with silence. */
void Init_Waveform(void) {
	uint32_t i;
	
	for (i=0; i<NUM_SAMPLES_PER_SOUND_BUFFER; i++) {
		SoundBuffer[0][i] = (MAX_DAC_CODE/2);
		SoundBuffer[1][i] = (MAX_DAC_CODE/2);
	}
}

void Init_Voices(void) {
	uint16_t i;
	
	for (i=0; i<NUM_VOICES; i++) {
		Voice[i].Volume = 0;
		Voice[i].Decay = 0;
		Voice[i].Duration = 0;
		Voice[i].Period = 0;
		Voice[i].Counter = 0;
		Voice[i].CounterIncrement = 0;
		Voice[i].Type = VW_UNINIT;
		Voice[i].Updated = 0;
	}
}

/* Initialize sound hardware, sine table, and waveform buffer. */
void Sound_Init(void) {
	SineTable_Init();	
	Init_Waveform();
	Init_Voices();
	WriteSBNum = 0; // Start writing to waveform buffer 0
	
	DAC_Init();
	DMA_Init();
	TPM2_Init();
	Configure_TPM2_for_DMA(AUDIO_SAMPLE_PERIOD_US); 

	SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);

	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; 
	
	PORTE->PCR[AMP_ENABLE_POS] &= ~PORT_PCR_MUX_MASK;	
	PORTE->PCR[AMP_ENABLE_POS] |= PORT_PCR_MUX(1);	// Select GPIO
	PTE->PDDR |= MASK(AMP_ENABLE_POS); // set to output
	PTE->PSOR = MASK(AMP_ENABLE_POS);  // enable audio amp

}

void Sound_Create_OS_Objects(void) {
	Voice_mutex = osMutexNew(&Voice_mutex_attr);

	t_Sound_Manager = osThreadNew(Thread_Sound_Manager, NULL, &Sound_Manager_attr);
	t_Refill_SoundBuffer = osThreadNew(Thread_Refill_SoundBuffer, NULL, &Refill_SoundBuffer_attr);
}

void Sound_Enable_Amp(int enable) {
	if (enable)
		PTE->PSOR = MASK(AMP_ENABLE_POS);  // enable audio amp
	else
		PTE->PCOR = MASK(AMP_ENABLE_POS);  // disable audio amp
		
}
/* Simple audio test function using busy-waiting. */
void Play_Tone(void) {
	int i, d=MAX_DAC_CODE>>5, n;
	float p = 40000;
	
	
	for (i=0; i<60; i++) {
		n = 20000.0f/p;
		while (n--) {
			Play_Sound_Sample((MAX_DAC_CODE>>1)+d);
			ShortDelay(p);
			Play_Sound_Sample((MAX_DAC_CODE>>1)-d);
			ShortDelay(p);
		}
		// Delay(40);
		p *= 0.9f;
	}
}

int16_t Sound_Generate_Next_Sample (VOICE_T *voice) {
	uint16_t lfsr, bit;
	int16_t sample;

	switch (voice->Type) {
		case VW_NOISE:
			lfsr = voice->Counter;
			// source code from http://en.wikipedia.org/wiki/Linear_feedback_shift_register
			// taps: 16 14 13 11; characteristic polynomial: x^16 + x^14 + x^13 + x^11 + 1
			bit	= 1 & ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) );
			lfsr =  (lfsr >> 1) | (bit << 15);
			voice->Counter = lfsr;
			sample = (lfsr >> 4) - (MAX_DAC_CODE/2); // scale to get 12-bit value
			break;
		case VW_SQUARE:
			if (voice->Counter < voice->Period/2) 
				sample = MAX_DAC_CODE/2 - 1;
			else 
				sample = -MAX_DAC_CODE/2;
			voice->Counter++;
			if (voice->Counter == voice->Period)
				voice->Counter = 0;
			break;
		case VW_SINE:
			sample = SineTable[((voice->Counter)/256)]; 
			voice->Counter += voice->CounterIncrement;
			if (voice->Counter >= voice->Period * voice->CounterIncrement)
				voice->Counter = 0;
			break;
		default:
			sample = 0;
			break;
	}
	return sample;
}

void Play_Waveform_with_DMA(void) {
	Configure_DMA_For_Playback(SoundBuffer[0], SoundBuffer[1], NUM_SAMPLES_PER_SOUND_BUFFER, 1);
	Start_DMA_Playback();
}

 void Thread_Sound_Manager(void * arg) {
	uint16_t lfsr=12345;
	uint16_t bit;
	uint32_t tick;
	uint16_t freq_sel=MIN_FREQ_SEL;
	volatile uint16_t base_volume=MIN_VOICE_VOLUME, volume_factor = PERIOD_TO_VOLUME_FACTOR;
	int background_music_note_interval = 2*THREAD_SOUND_MANAGER_PERIOD_MS;

  Play_Waveform_with_DMA();
 	tick = osKernelGetTickCount();

	tick += MS_TO_TICKS(background_music_note_interval);
	osDelayUntil(tick);
	 
	while (1) {
		osMutexAcquire(Voice_mutex, osWaitForever);

		// Find next free voice
		unsigned int v=0;
		while ((Voice[v].Duration != 0) && (v < NUM_VOICES))
			v++;
		if (v < NUM_VOICES) {
			Control_RGB_LEDs(0, 1, 0);
			Voice[v].Duration = AUDIO_SAMPLE_FREQ/2;
			bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
			lfsr =  (lfsr >> 1) | (bit << 15);
			
#if FREQUENCY_SWEEP
			freq_sel +=1;
			if (freq_sel > MAX_FREQ_SEL)
				freq_sel = MIN_FREQ_SEL;
#else
			freq_sel = (lfsr % (MAX_FREQ_SEL - MIN_FREQ_SEL)) + MIN_FREQ_SEL;
#endif
			
#if USE_SCALE // Confine notes to diatonic scale
			if (freq_sel > MAX_FREQ_SEL)
				freq_sel = MAX_FREQ_SEL;
			Voice[v].Period = ScalePeriods[freq_sel]; 
#else
			// make a new random sound periodically
			Voice[v].Period = FREQ_TO_PERIOD((freq_sel & 0x03FF) + 40); 
#endif
			uint16_t temp_volume = base_volume + (Voice[v].Period << volume_factor); // 0x4fff 
			if (temp_volume > MAX_VOICE_VOLUME)
				temp_volume = MAX_VOICE_VOLUME;
			Voice[v].Volume = temp_volume;
			
			Voice[v].Decay = 20; // (lfsr>>4)&0x1f;
			Voice[v].Counter = 1; 
			Voice[v].CounterIncrement = (NUM_STEPS*256)/Voice[v].Period; 
#if SOUND_ONLY_SINES
			Voice[v].Type = VW_SINE;
#else
			switch ((lfsr & 0x30) >> 4) {
				case 0:
				case 1:
					Voice[v].Type = VW_SINE;
					break;
				case 2:
					Voice[v].Type = VW_SQUARE;
					break;
				default:
					Voice[v].Type = VW_NOISE;
					Voice[v].Counter = 1234; // Pseudonoise LFSR seed 
					Voice[v].Decay <<= 2; // Make noise decay faster  
					break;
			}
#endif
			Voice[v].Updated = 1;
			osMutexRelease(Voice_mutex);
			
#if NEW_SOUND_TRIGGERS_BUFFER_UPDATE
			osThreadFlagsSet(t_Refill_SoundBuffer, EV_START_NEW_SOUND);	
#endif
		} else { // ran out of voices, 
			osMutexRelease(Voice_mutex);
			Control_RGB_LEDs(1, 0, 0);
			//			v = 0; // so overwrite first one.... later
		}

		tick += MS_TO_TICKS(MAX(15, background_music_note_interval - stress));
		osDelayUntil(tick);
	}
}

 void Add_Sound(int st) {
	static uint16_t lfsr=12345;
	uint16_t bit;
	unsigned int v=0;
	uint16_t freq_sel=30;
	uint16_t base_volume=MAX_VOICE_VOLUME-0x2000, volume_factor = PERIOD_TO_VOLUME_FACTOR;
	
	// Find next free voice
	while ( (Voice[v].Duration != 0) && (v < NUM_VOICES))
	 v++;
	if (v == NUM_VOICES) // ran out of voices, so overwrite first one
		v = 0;

	Voice[v].Duration = AUDIO_SAMPLE_FREQ;
	Voice[v].Decay = 8; 

	switch (st) {
		case 0:
			Voice[v].Volume = MAX_VOICE_VOLUME;
			Voice[v].Type = VW_SINE;
			Voice[v].Counter = 1; 
			Voice[v].Period = 300;
			Voice[v].CounterIncrement = (NUM_STEPS*256)/Voice[v].Period; 
			break;
		case 1:
		default: 
			Voice[v].Volume = 4*MIN_VOICE_VOLUME;
			Voice[v].Type = VW_NOISE;
			Voice[v].Counter = 1234; // Pseudonoise LFSR seed 
			Voice[v].Decay = 40; // Make noise decay faster  
			break;
	}
		
	Voice[v].Updated = 1;
	osMutexRelease(Voice_mutex);
	
#if NEW_SOUND_TRIGGERS_BUFFER_UPDATE
	osThreadFlagsSet(t_Refill_SoundBuffer, EV_START_NEW_SOUND);	
#endif

}


 void Thread_Refill_SoundBuffer(void * arg) {
	int32_t i, v, sum, sample, is_update=0;
	uint32_t events;
#if USE_DOUBLE_BUFFER
	uint8_t initialized = 0;
#endif
	 
	while (1) {
		// wait for trigger
		events = osThreadFlagsWait(EV_REFILL_SOUND_BUFFER | EV_START_NEW_SOUND, osFlagsWaitAny, osWaitForever); 	
		if (events & EV_REFILL_SOUND_BUFFER) { 
			/* Updates all samples, regardless of whether any voices have been added 
			(i.e. even if EV_REFILL_REMAINING_SB is also set) */
			i = 0; // start with sample 0
		} else if (events & EV_START_NEW_SOUND) { 
			// One or more voices has been added, so update remaining buffer which hasn't been read yet
			i = Get_DMA_Transfers_Completed(0);
			if ((i < 0) || (i > TOTAL_SOUND_BUFFER_SAMPLES))
				while (1); // debugging: catch this errror!
		}			

		osMutexAcquire(Voice_mutex, osWaitForever);
		for (; i<NUM_SAMPLES_PER_SOUND_BUFFER; i++) {
			sum = 0;
			for (v=0; v<NUM_VOICES; v++) {
				if (Voice[v].Duration > 0) {
					sample = Sound_Generate_Next_Sample(&(Voice[v]));
					sample = (sample*Voice[v].Volume)>>16;
					// update volume with decayed version
					Voice[v].Volume = (Voice[v].Volume * (((uint32_t) 65536) - Voice[v].Decay)) >> 16; 
					Voice[v].Duration--;
					sum += sample;
				} 
			}		
			sum = MIN(sum + (MAX_DAC_CODE/2), MAX_DAC_CODE-1); // Center at 1/2 DAC range, clip top
			SoundBuffer[WriteSBNum][i] = sum; 
			DEBUG_STOP(DBG_REFILL_PENDING);
			// Sample written
		}
		// Clear updated flags for all voices
		for (v=0; v<NUM_VOICES; v++) 
			Voice[v].Updated = 0;
		
		osMutexRelease(Voice_mutex);
#if USE_DOUBLE_BUFFER
		if (!initialized) { // Fill up both buffers the first time
			initialized = 1; 
			for (i=0; i<NUM_SAMPLES_PER_SOUND_BUFFER; i++) {
				sum = 0;
				for (v=0; v<NUM_VOICES; v++) {
					if (Voice[v].Duration > 0) {
						sample = Sound_Generate_Next_Sample(&(Voice[v]));					
						sample = (sample*Voice[v].Volume)>>16;
						sum += sample;
						// update volume with decayed version
						Voice[v].Volume = (Voice[v].Volume * (((uint32_t) 65536) - Voice[v].Decay)) >> 16; 
						if (Voice[v].Volume == 0)
							Voice[v].Duration = 0;
						else
						Voice[v].Duration--;
					} 
				}
				sum = sum + (MAX_DAC_CODE/2);
				sum = MIN(sum, MAX_DAC_CODE-1);
				SoundBuffer[WriteSBNum][i] = sum; 
			}
		}
#endif			
	}
}

