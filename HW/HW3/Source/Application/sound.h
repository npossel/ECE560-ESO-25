#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>
#include <cmsis_os2.h>

// Hardware characteristics 
#define AMP_ENABLE_POS (29) // On Port E
#define DAC_POS (30)
#define MAX_DAC_CODE (4095)

// Audio Configuration
#define AUDIO_SAMPLE_FREQ (20000.0f)
#define NUM_VOICES (8)
#define NUM_STEPS (64) // for DDS sine wave table

// Audio time/frequency conversions
#define AUDIO_SAMPLE_PERIOD_US (1000000/AUDIO_SAMPLE_FREQ)
#define FREQ_TO_PERIOD(f) (AUDIO_SAMPLE_FREQ/(f))

// Volume characteristics
#define MAX_VOICE_VOLUME (0x8000) // Maximum volume for a voice
#define MIN_VOICE_VOLUME (0x0400) // Minimum volume for a voice
#define PERIOD_TO_VOLUME_FACTOR (8) // Low frequency boost: add period<<(this factor) to volume

// Buffering
#define USE_DOUBLE_BUFFER (0) // DEFAULT: 0
#define TOTAL_SOUND_BUFFER_SAMPLES (1024)
#define NUM_SAMPLES_PER_SOUND_BUFFER (TOTAL_SOUND_BUFFER_SAMPLES/(1+USE_DOUBLE_BUFFER))

// Thread Configuration
// Period
#define THREAD_SOUND_MANAGER_PERIOD_MS (123)	

// Priorities
#define THREAD_SM_PRIO 		osPriorityNormal
#define THREAD_RSB_PRIO 	osPriorityNormal // DEFAULT: osPriorityNormal 

// Events for sound generation and control
#define EV_START_NEW_SOUND 			(0x01)
#define EV_REFILL_SOUND_BUFFER  (0x02)


// Sound Manager note generation configuration
#define SOUND_ONLY_SINES (1)		// Generate only sine waves
#define USE_SCALE (1) // Confine notes to diatonic scale
#define MIN_FREQ_SEL (24) // Lowest note to play (usually 18)
#define MAX_FREQ_SEL (51) // Highest note to play
#define FREQUENCY_SWEEP (0)	// Step through notes instead of random selection 
#define NEW_SOUND_TRIGGERS_BUFFER_UPDATE (0) // Need to implement partial update code correctly in Thread_Refill_SoundBuffer

// Data Type Definitions
// Voice types
typedef enum {VW_UNINIT, VW_NOISE, VW_SQUARE, VW_SINE} VW_E;

// Voice characteristics
typedef struct {
	uint16_t Volume; // scaled by 65536
	int16_t Decay; // scaled by 65536. 0 means no decay in volume.
	uint32_t Duration; // measured in samples
	uint32_t Counter; // internal, measured in samples (for DDS, is scaled by 256)
	uint32_t CounterIncrement; // for DDS only: internal, samples scaled by 256 
	uint16_t Period; // measured in samples
	VW_E Type; // Sine, square, white noise
	uint8_t Updated; 
} VOICE_T;

// Global variables
extern uint8_t WriteSBNum;
extern osThreadId_t t_Sound_Manager, t_Refill_SoundBuffer;

// Function interface
void DAC_Init(void);
void Play_Tone(void);
void Sound_Init(void);
void Sound_Create_OS_Objects(void);

void Sound_Enable_Amp(int);

void Sound_Refill_Buffer(uint32_t samples);
void Play_Waveform_with_DMA(void);

void Thread_Sound_Manager(void * arg);
void Thread_Refill_SoundBuffer(void * arg);
 
 void Add_Sound(int);


#endif // SOUND_H
