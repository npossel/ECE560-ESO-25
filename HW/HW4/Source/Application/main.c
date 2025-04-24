/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"
#include <cmsis_os2.h>
#include "config.h"
#include "threads.h"

#include "LCD.h"
#include "LCD_driver.h"
#include "font.h"

#include "LEDs.h"
#include "debug.h"

#include "timers.h"
#include "sound.h"
#include "DMA.h"
#include "I2C.h"
#include "mma8451.h"
#include "delay.h"
#include "profile.h"
#include "math.h"
#include "EventRecorder.h"

#include <arm_acle.h>

/* Debug_Set Duration
f_cpu				f_Bus 		Duration		OUTDIV
    1 MHz   1/4 MHz		96.2 us			3,3
    1 MHz   1/2  			63.7 us			3,1
    1 MHz   1  				56.7 us			3,0
	  2 MHz   1					31.8 us			1,1
	  4 MHz		1					24.16 us		0,3
20.98 MHz		20.98		 	 2.78 us		0,0
   48 MHz		24		 		 1.20 us		0,1
*/

#define DELAY_LOOP_COUNT (43) // f_cpu = 48 MHz.  43: 10.08 useconds, 42: Pulse width = 9.87 useconds 
/*
	Duration   	f_cpu
	10.08 us		48 MHz
	20.2 us			24 MHz
	23.17 us		20.98 MHz
	121.6 us		4 MHz
*/

#define FLL_DELAY_COUNT (40) // Number of times to run delay loop while waiting for PLL frequency to stabilize
/* 
	FLL Acquisition Times (ROUGH!)
	21 -> 48 MHz 	500 us
	48 -> 24 MHz	300 us
	
	Note that MCU power is dependent on CPU (and so FLL) frequency.
	I_cpu stabilizes when FLL freq stabilizes
*/

#define USE_LEDS_FOR_POWER_DEBUG (0)
#define ENABLE_KEY_INST_DLY_LPS (0)

#if 0
#define INST_DEBUG_SET(x,y,z) { DEBUG_START(DBG_TIMING); \
																Debug_Set(x,y,z); \
																DEBUG_STOP(DBG_TIMING); }
#else
#define INST_DEBUG_SET(x,y,z) Debug_Set(x,y,z);
#endif
																
void Set_OUTDIV(uint32_t outdiv1, uint32_t outdiv4) {
/* 	VLP data
		F_CORE  F_BUS		OUTDIV1		OUTDIV4
		1				1				3					0
		1				0.5			3					1
		1.33		0.677		2					1
		2				0.667		1					2
		4				0.8			0					4
*/	
	// Divide MCG OutClk 
	SIM->CLKDIV1 = (SIM->CLKDIV1 & ~SIM_CLKDIV1_OUTDIV1_MASK) | SIM_CLKDIV1_OUTDIV1(outdiv1);
	
	// Divide core clock to ensure bus clock < 800 kHz in VLP Run mode
	SIM->CLKDIV1 = (SIM->CLKDIV1 & ~SIM_CLKDIV1_OUTDIV4_MASK)| SIM_CLKDIV1_OUTDIV4(outdiv4);
}

void Fast_Instrumented_Delay_Loop(void) {
	DEBUG_START(DBG_INST_DLY);
	for (int i=DELAY_LOOP_COUNT; i>0; i--) // Loop
		DEBUG_TOGGLE(DBG_IDLE_LOOP);
	DEBUG_STOP(DBG_INST_DLY);
}

void Instrumented_Delay_Loop(int toggle) {
	DEBUG_START(DBG_INST_DLY);
	if (toggle) {
		for (volatile int i=DELAY_LOOP_COUNT; i>0; i--) // Loop
			DEBUG_TOGGLE(DBG_IDLE_LOOP);
	} else {
		for (volatile int i=DELAY_LOOP_COUNT; i>0; i--) { // Loop
			__nop();
		}
	}
	DEBUG_STOP(DBG_INST_DLY);
}

void Repeat_Instrumented_Delay_Loop(int n, int toggle) {
	while (n--) 
		Instrumented_Delay_Loop(toggle);
}

void Instrumented_Delay_Loop_Ten_Cycles(void) {
	DEBUG_START(DBG_INST_DLY);
	for (int i=DELAY_LOOP_COUNT; i>0; i--) { // Loop
		DEBUG_TOGGLE(DBG_IDLE_LOOP);
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
	}
	DEBUG_STOP(DBG_INST_DLY);
}

void Select_FEI_Fast(void) {
#if CLOCK_SETUP == 0
	OSC0->CR &= ~OSC_CR_ERCLKEN_MASK;	// disable external oscillator

	// Starting out with FEI: Using FLL (32.768 kHz * 640 = 20.98 MHz as clock source. 
	// Speed up to 48 MHz 
	// First set f_bus = f_cpu/2
	Set_OUTDIV(0,1); 
	// Then Update DCO range
	// Note: Datasheet says FLL target frequency acquisition time is up to 1 ms
	// when enabling FLL or changing source, divider, trim, DMX32, or DRS. 
	// Here takes around 500 us.
	MCG->C4 = (MCG->C4 & (~(MCG_C4_DRST_DRS_MASK | MCG_C4_DMX32_MASK))) 
						| (MCG_C4_DRST_DRS(1) | MCG_C4_DMX32(1));
	SystemCoreClockUpdate();
	Repeat_Instrumented_Delay_Loop(FLL_DELAY_COUNT, 1);
	//	FLL frequency should be stabilized now
#endif
}

void LP_Testing2(void) {
	uint32_t ps_pins[] = {DBG_PWR_B0, DBG_PWR_B1, DBG_PWR_B2, DBG_PWR_B3};
	uint32_t ps_pc = sizeof(ps_pins)/sizeof(ps_pins[0]);
	uint32_t del = FLL_DELAY_COUNT;
	
	INST_DEBUG_SET(ps_pins, ps_pc, 15);
	DEBUG_START(DBG_NORM_NVLP); // Normal run mode
	DEBUG_STOP(DBG_IRC_F_NS); // Using Slow IRC
	DEBUG_START(DBG_USING_FLL); 
	DEBUG_START(DBG_CPU_F_NS); // CPU clock is fast
	
	LCD_Disable();
	Sound_Init_Amp();
	Sound_Enable_Amp(0);
#if USE_LEDS_FOR_POWER_DEBUG
	Control_RGB_LEDs(1,0,0);	// Red: normal run // L.81
#endif
	
#if CLOCK_SETUP == 0
	// *** PREP ***
	OSC0->CR &= ~OSC_CR_ERCLKEN_MASK;	// disable external oscillator
	INST_DEBUG_SET(ps_pins, ps_pc, 0);

	// Select_FEI_Fast();
	// while (1);

	// *** STAGE 1 ***
	// Starting out with FEI: Using FLL (32.768 kHz * 640 = 20.98 MHz as clock source. 
	INST_DEBUG_SET(ps_pins, ps_pc, 1);
	Set_OUTDIV(0, 0); 	// f_bus = f_cpu
	Instrumented_Delay_Loop(1);  // L.110

// *** STAGE 2 ***
	INST_DEBUG_SET(ps_pins, ps_pc, 2);
	// Speed up to 48 MHz 
	Set_OUTDIV(0,1); // f_bus = f_cpu/2
	// Update DCO range
	// Note: Datasheet says FLL target frequency acquisition time is up to 1 ms
	// when enabling FLL or changing source, divider, trim, DMX32, or DRS. 
	// Here takes around 500 us.
	MCG->C4 = (MCG->C4 & (~(MCG_C4_DRST_DRS_MASK | MCG_C4_DMX32_MASK))) 
						| (MCG_C4_DRST_DRS(1) | MCG_C4_DMX32(1));

	// Before FLL frequency stabilizes
	Repeat_Instrumented_Delay_Loop(FLL_DELAY_COUNT, 1);
	//	FLL frequency should be stabilized now

	// *** STAGE 3 ***
	// Switch through FBI to BLPI and into VLP Run mode
	INST_DEBUG_SET(ps_pins, ps_pc, 3);
	// Slow down to 4 MHz
	MCG->SC &= ~MCG_SC_FCRDIV_MASK;	// Ensure FCRDIV is 0000: divide by 1
	MCG->SC |= MCG_SC_FLTPRSRV(1); // Preserve FLL Filter values
	MCG->C2 |= MCG_C2_IRCS(1);	// Use Fast IRC for IRCSCLK
	DEBUG_START(DBG_TIMING);
	while (!(MCG->S & MCG_S_IRCST_MASK))	// Confirm it... 60.9 us
		;
	DEBUG_STOP(DBG_TIMING);
	
	MCG->C1 = (MCG->C1 & ~MCG_C1_CLKS_MASK) | MCG_C1_CLKS(1);	// CLKS = 01. Use IRCSKCLK for MCGOUT_CLK, switching from FLL (00) to IRC (01)
	DEBUG_START(DBG_TIMING);
	while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(1))	// Confirm it ... 4.9 us
		;		
	DEBUG_STOP(DBG_TIMING);
	DEBUG_START(DBG_IRC_F_NS); // Using FIRC
	DEBUG_STOP(DBG_USING_FLL); // Not using FLL, but still powered
	DEBUG_STOP(DBG_CPU_F_NS); // CPU clock is not fast
#if USE_LEDS_FOR_POWER_DEBUG
	Control_RGB_LEDs(1, 1, 0);	// Yellow: Switched to 4 MHz FIRC. Should be in FBI now
#endif
	// Set clock divider here in Run mode, since can't change it in VLP Run mode
	Set_OUTDIV(0,3); 	// 0,3: f_cpu = 4 MHz, f_bus = f_cpu/4 = 1 MHz. 
	//	 Set_OUTDIV(1, 1); 	// 1 MHz: 3,1; 1.33 MHz: 2,1; 2 MHz: 1,1 or 1,2
#if ENABLE_KEY_INST_DLY_LPS
	Instrumented_Delay_Loop();
#endif

	// *** STAGE 4 *** /
	INST_DEBUG_SET(ps_pins, ps_pc, 4);
	// MUST disable PLL, FLL to switch to BLPI 
	MCG->C2 |= MCG_C2_LP_MASK; // set C2 LP to 1 to disable PLL, FLL
	DEBUG_START(DBG_USING_FLL); // Pulse to indicate disabling of FLL
	DEBUG_STOP(DBG_USING_FLL); 
	
	// Switch from Normal Run to VLP Run mode
	SMC->PMCTRL = (SMC->PMCTRL & ~SMC_PMCTRL_RUNM_MASK) | SMC_PMCTRL_RUNM(2);
	DEBUG_START(DBG_TIMING);
	while ((SMC->PMSTAT & SMC_PMSTAT_PMSTAT_MASK) != 4) 	// wait until in VLPR mode ... 38.78 us
		; 
	DEBUG_STOP(DBG_TIMING);
	DEBUG_STOP(DBG_NORM_NVLP); // VLP run mode
#if USE_LEDS_FOR_POWER_DEBUG
	Control_RGB_LEDs(0, 1, 0);	// Green: Entered VLP run mode
#endif
	// Not needed, but delay to see current fall and stabilize
	// Instrumented_Delay_Loop(0); 

	// *** STAGE 6 ***
	// Blue: About to enter (VLP) wait or stop mode
	INST_DEBUG_SET(ps_pins, ps_pc, 6);
#if USE_LEDS_FOR_POWER_DEBUG
	Control_RGB_LEDs(0,0,1);			
#endif
#if ENABLE_KEY_INST_DLY_LPS
	Instrumented_Delay_Loop(); // To check clock frequency
#endif
	//  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;	// Enable Stop mode
	// __wfi();  // Need an interrupt to wake us up here
	
// *** STAGE 7 ***
	INST_DEBUG_SET(ps_pins, ps_pc, 7);
	// LEDs Off: woke up
#if USE_LEDS_FOR_POWER_DEBUG
	Control_RGB_LEDs(0,0,0);			
#endif	
	// Still in BLPI
	// switch from VLP Run to Normal Run mode 
	SMC->PMCTRL &= ~SMC_PMCTRL_RUNM_MASK; // RUNM is 00
	DEBUG_START(DBG_TIMING);
	while ((SMC->PMCTRL & SMC_PMCTRL_RUNM_MASK) != 0) // Wait until in run mode ... 10.64 us
		;
	DEBUG_STOP(DBG_TIMING);
	DEBUG_START(DBG_NORM_NVLP); // Normal run mode

	// *** STAGE 8 ***
	INST_DEBUG_SET(ps_pins, ps_pc, 8);
	DEBUG_START(DBG_TIMING);
	while ((PMC->REGSC & PMC_REGSC_REGONS_MASK) == 0) // Wait until regulators are on ... 8.44 us
		;
	DEBUG_STOP(DBG_TIMING);

	// *** STAGE 9 ***
	INST_DEBUG_SET(ps_pins, ps_pc, 9);
	// Enable FLL
	MCG->C2 &= ~MCG_C2_LP_MASK; // clear C2 LP to 0 to ensure PLL, FLL not disabled
	DEBUG_START(DBG_USING_FLL); // Powered, but not used
	DEBUG_STOP(DBG_USING_FLL); 
#if ENABLE_KEY_INST_DLY_LPS
	Instrumented_Delay_Loop();
#endif

	// *** STAGE 10 ***
	// Select Slow IRC FLL
	INST_DEBUG_SET(ps_pins, ps_pc, 10);
	MCG->C1 = (MCG->C1 & ~MCG_C1_IREFS_MASK) | MCG_C1_IREFS(1);
	DEBUG_START(DBG_TIMING);
	while ((MCG->S & MCG_S_IREFST_MASK)!=(MCG_S_IREFST(1)))	// Confirm it ... 2.48 us
		;
	DEBUG_STOP(DBG_TIMING);
	DEBUG_STOP(DBG_IRC_F_NS); // SIRC selected

	
// *** STAGE 11 ***
	INST_DEBUG_SET(ps_pins, ps_pc, 11);
	// Use PLLS to select FLL output
	MCG->C6 &= ~MCG_C6_PLLS_MASK;	// PLLS = 0
	DEBUG_START(DBG_TIMING);
	while ((MCG->S & MCG_S_PLLST_MASK)!=MCG_S_PLLST(0))	// Confirm FLL output selected ... 2 us
		;
	DEBUG_STOP(DBG_TIMING);

	// Use CLKS to select PLLS output (FLL output) for MCGOUT
	MCG->C1 = (MCG->C1 & ~MCG_C1_CLKS_MASK) | MCG_C1_CLKS(0);	
	DEBUG_START(DBG_TIMING);
	while ((MCG->S & MCG_S_CLKST_MASK)!=(MCG_S_CLKST(0)))	// Confirm it ... 0.3 us
		;
	DEBUG_STOP(DBG_TIMING);
	DEBUG_START(DBG_USING_FLL); // Really using FLL now
	DEBUG_START(DBG_CPU_F_NS); // CPU clock is fast

// *** STAGE 12 ***
	INST_DEBUG_SET(ps_pins, ps_pc, 12);
	Instrumented_Delay_Loop(1); 
	Repeat_Instrumented_Delay_Loop(FLL_DELAY_COUNT/5, 1);
	while (1)
		;
	
#endif // CLOCK_SETUP == 0
	INST_DEBUG_SET(ps_pins, ps_pc, 15); // Done!

	while(1) // L.266
		;
}

#if 0
void LP_Testing(void) {
	uint32_t ps_pins[] = {DBG_PWR_B0, DBG_PWR_B1, DBG_PWR_B2, DBG_PWR_B3};
	uint32_t ps_pc = sizeof(ps_pins)/sizeof(ps_pins[0]);
	uint32_t del = FLL_DELAY_COUNT;
	
	
	Debug_Set(ps_pins, ps_pc, 15);

	LCD_Disable();
	Sound_Init_Amp();
	Sound_Enable_Amp(0);
	Control_RGB_LEDs(1,0,0);	// Red: normal run // L.81
	
#if CLOCK_SETUP == 1
	// PEE: 48 MHz
	// Step Frequency valuate current consumption
	while (1) {
		Control_RGB_LEDs(1,0,0);	// Red: normal run
		Instrumented_Delay_Loop();
		Control_RGB_LEDs(0,0,0);	// Off for current analysis
		for (int div = 2; div <= 16; div++) { // 16
			DEBUG_START(DBG_PM_TRANSITION);
			int bus_div;
			if (div>2)
				bus_div = 0;
			else
				bus_div = 1;
			Set_OUTDIV(div-1,bus_div); // f_bus = f_cpu/2
			DEBUG_STOP(DBG_PM_TRANSITION);
			Instrumented_Delay_Loop();
		}
	}
#elif CLOCK_SETUP == 0
	// *** PREP ***
	OSC0->CR &= ~OSC_CR_ERCLKEN_MASK;	// disable external oscillator
	Debug_Set(ps_pins, ps_pc, 0);

	// *** STAGE 1 ***
	// Starting out with FEI: Using FLL (32.768 kHz * 640 = 20.98 MHz as clock source. 
	Debug_Set(ps_pins, ps_pc, 1);
	Set_OUTDIV(0, 0); 	// f_bus = f_cpu
	Instrumented_Delay_Loop(1);  // L.110

#if 1
	// FLL current vs. frequency test
	while (1) {
		for (int n = 0; n < 4; n++) {
			// Update DCO range
			// Note: Datasheet says FLL target frequency acquisition time is up to 1 ms
			// when enabling FLL or changing source, divider, trim, DMX32, or DRS. 
			Debug_Set(ps_pins, ps_pc, n);
			uint32_t t = MCG->C4 & ~(MCG_C4_DRST_DRS_MASK | MCG_C4_DMX32_MASK);
			t |= MCG_C4_DRST_DRS(n>>1) | MCG_C4_DMX32(n&1);
			MCG->C4 = t;
			
			Set_OUTDIV(0, 1); // max freq
			DEBUG_START(DBG_PM_TRANSITION);
			Repeat_Instrumented_Delay_Loop(FLL_DELAY_COUNT,1);
			Repeat_Instrumented_Delay_Loop(FLL_DELAY_COUNT,1);
			DEBUG_STOP(DBG_PM_TRANSITION);
			
			for (int d = 2; d < 32; d <<= 1) {
				Set_OUTDIV(d-1, 1);
				DEBUG_START(DBG_PM_TRANSITION);
				Repeat_Instrumented_Delay_Loop(FLL_DELAY_COUNT/d, 1);
				DEBUG_STOP(DBG_PM_TRANSITION);
			}
			Set_OUTDIV(0, 1); // max freq
		}
	}
#endif

	// for testing wait mode power 
	// __wfi();
	
#if 1
// *** STAGE 2 ***
	Debug_Set(ps_pins, ps_pc, 2);
	DEBUG_START(DBG_PM_TRANSITION);
	// Speed up to 48 MHz 
	Set_OUTDIV(0,1); // f_bus = f_cpu/2
	// Set_OUTDIV(15,0); 
	// Update DCO range
	// Note: Datasheet says FLL target frequency acquisition time is up to 1 ms
	// when enabling FLL or changing source, divider, trim, DMX32, or DRS. 
	// Here takes around 500 us.
	MCG->C4 &= ~(MCG_C4_DRST_DRS_MASK | MCG_C4_DMX32_MASK);
	MCG->C4 |= MCG_C4_DRST_DRS(1) | MCG_C4_DMX32(1);

	// Before FLL frequency stabilizes
	DEBUG_START(DBG_PM_TRANSITION);
	Repeat_Instrumented_Delay_Loop(FLL_DELAY_COUNT,1);
	DEBUG_STOP(DBG_PM_TRANSITION);

	// for testing wait mode power 
	// __wfi();

	// for testing run mode power
	// while (1) Instrumented_Delay_Loop();  
#endif	
	
#if 1
	// *** STAGE 3 ***
	// Slow back down to 24 MHz
	Debug_Set(ps_pins, ps_pc, 3);
	DEBUG_START(DBG_PM_TRANSITION); // L.133
	// Using FLL (32.768 kHz * 732 = 24 MHz as clock source. 
	MCG->C4 &= ~(MCG_C4_DRST_DRS_MASK | MCG_C4_DMX32_MASK);
	MCG->C4 |= MCG_C4_DRST_DRS(0) | MCG_C4_DMX32(1);
	Set_OUTDIV(0, 0); 	// f_bus = f_cpu
	Instrumented_Delay_Loop(1); // L.139
	Repeat_Instrumented_Delay_Loop(FLL_DELAY_COUNT/5, 1);
	DEBUG_STOP(DBG_PM_TRANSITION);
#endif

#if 0
	// *** STAGE 3.5 ***
	// Slow down with OUTDIV1
	DEBUG_START(DBG_PM_TRANSITION);
	// Using FLL (32.768 kHz * 732 = 24 MHz as clock source. 
	Set_OUTDIV(3, 0); 	// f_bus = f_cpu
	DEBUG_STOP(DBG_PM_TRANSITION);
	Instrumented_Delay_Loop(); // 6 MHz = 4586 us; 1.5 MHz = 
	// Set_OUTDIV(0, 0); 	// f_bus = f_cpu // Clean up, restore state.
#endif

	// *** STAGE 4 ***
	// Switch through FBI to BLPI and into VLP Run mode
	Debug_Set(ps_pins, ps_pc, 4);
	DEBUG_START(DBG_PM_TRANSITION); // LOOOOOOONG transition!
	MCG->C6 &= ~MCG_C6_PLLS_MASK;	// PLLS = 0
	MCG->C1 |= MCG_C1_IREFS_MASK;	// IREFS = 1
	MCG->C2 &= ~MCG_C2_LP_MASK;	// LP = 0: Keep PLL/FLL enabled 
	// Slow down to 4 MHz
	MCG->SC &= ~MCG_SC_FCRDIV_MASK;	// Set FCRDIV to 0000: divide by 1
	MCG->C2 |= MCG_C2_IRCS(1);	// Select Fast IRC for IRCSCLK
	DEBUG_START(DBG_TIMING);
	while (!(MCG->S & MCG_S_IRCST_MASK))	// Confirm it
		;
	DEBUG_STOP(DBG_TIMING);
	
	DEBUG_START(DBG_TIMING);
	MCG->C1 |= MCG_C1_CLKS(1);	// CLKS = 01. Select IRCSKCLK for MCGOUT_CLK. Switch from FLL (00) to IRC (01)
	while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(1))	// Confirm it
		;		// L.181
	
	DEBUG_START(DBG_TIMING);
	Control_RGB_LEDs(1, 1, 0);	// Yellow: Switched to 4 MHz FIRC. Should be in FBI now
	// Set clock divider here in Run mode, since can't change it in VLP Run mode
	Set_OUTDIV(0,4); 	// f_cpu = 4 MHz, f_bus = f_cpu/5 = 0.8 MHz
	//	Set_OUTDIV(3, 1); 	// 1 MHz: 3,1; 1.33 MHz: 2,1; 2 MHz: 1,2

	Repeat_Instrumented_Delay_Loop(FLL_DELAY_COUNT/4, 1);
	Instrumented_Delay_Loop(1); // L.139
	DEBUG_STOP(DBG_PM_TRANSITION);

	// *** 4.1 ***
	// Instrumented_Delay_Loop(); 

	// *** 5 *** /
	Debug_Set(ps_pins, ps_pc, 5);

	// Switch to BLPI //L.189
	MCG->C2 |= MCG_C2_LP_MASK; // set C2 LP to 1 to disable PLL, FLL

	// switch from Normal Run to VLP Run mode //L.192
	SMC->PMCTRL &= ~SMC_PMCTRL_RUNM_MASK;
	SMC->PMCTRL |= SMC_PMCTRL_RUNM(2);
	DEBUG_START(DBG_TIMING);
	while ((SMC->PMSTAT & SMC_PMSTAT_PMSTAT_MASK) != 4) 	// wait until in VLPR mode
		; 
	// L.196
	DEBUG_STOP(DBG_TIMING);
	Control_RGB_LEDs(0, 1, 0);	// Green: Entered VLP run mode
	DEBUG_STOP(DBG_PM_TRANSITION); // End of LOOOONG transition

	Repeat_Instrumented_Delay_Loop(FLL_DELAY_COUNT/10, 1);

// *** STAGE 6 ***
	// Blue: About to enter (VLP) wait or stop mode
	Debug_Set(ps_pins, ps_pc, 6);
	Control_RGB_LEDs(0,0,1);			
	DEBUG_START(DBG_PM_TRANSITION);
	//  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;	// Enable Stop mode
	/* Note: stop mode doesn't seem to engage with first run after 
	download. Need to reset board and rerun to see stop mode. */
	
	DEBUG_STOP(DBG_PM_TRANSITION);
	Instrumented_Delay_Loop(1);
	// __wfi();  // Need an interrupt to wake us up here
	
	Debug_Set(ps_pins, ps_pc, 7);
	// LEDs Off: woke up
	Control_RGB_LEDs(0,0,0);			
	
	// Still in BLPI
	// Switch from VLPR to Run mode
	// switch from VLP Run to Normal Run mode 
	SMC->PMCTRL &= ~SMC_PMCTRL_RUNM_MASK; // RUNM is 00
	DEBUG_START(DBG_PM_TRANSITION);
	while ((SMC->PMCTRL & SMC_PMCTRL_RUNM_MASK) != 0) // Wait until in run mode
		;
	Debug_Set(ps_pins, ps_pc, 8);
	while ((PMC->REGSC & PMC_REGSC_REGONS_MASK) == 0) // Wait until regulators are on
		;
	Debug_Set(ps_pins, ps_pc, 9);
	DEBUG_STOP(DBG_PM_TRANSITION);

	// Enable FLL
	MCG->C2 &= ~MCG_C2_LP_MASK; // clear C2 LP to 0 to not disable PLL, FLL
	Repeat_Instrumented_Delay_Loop(FLL_DELAY_COUNT/4, 1);
	// What else to do to select FLL instead of PLL?

	Debug_Set(ps_pins, ps_pc, 10);
	
	// Next: switch from BLPI to FBI and then FEI mode
	// Check MCG->S for statuses: LOLS, PLLST, IREFST, CLKST, IRCST
	/**********************************************************/
		// *** Backwards STAGE 4 ***
	// Switch through FBI to BLPI and out of VLP Run mode
	MCG->C1 |= MCG_C1_CLKS(1);	// CLKS = 01. Select IRCSKCLK for MCGOUT_CLK. Switch from FLL (00) to IRC (01)
	while (!(MCG->S & MCG_S_IRCST_MASK))	// Confirm it
		;
	Debug_Set(ps_pins, ps_pc, 11);
	MCG->C2 |= MCG_C2_IRCS(1);	// Select Fast IRC for IRCSCLK
	// Slow down to 4 MHz
	MCG->SC &= ~MCG_SC_FCRDIV_MASK;	// Set FCRDIV to 0000: divide by 1
	MCG->C2 &= ~MCG_C2_LP_MASK;	// LP = 0: Keep PLL/FLL enabled 
	MCG->C1 |= MCG_C1_IREFS_MASK;	// IREFS = 1
	MCG->C6 &= ~MCG_C6_PLLS_MASK;	// PLLS = 0

	/********************************************************/

	// and more. See RefMan 24.5.3.3 but change for enabling going back to FEI
	// C2: range?, EREFS = 0
	// Select PLL
	MCG->C6 |= MCG_C6_PLLS_MASK;
	MCG->C1 |= MCG_C1_IREFS_MASK;
	
	// Select slow IRC
	MCG->C2 &= ~MCG_C2_IRCS_MASK;

	// C1: CLKS = 00, FRDIV = ???, IREFS = 1
	// Loop until ?, IREFST = 1, CLKST is --
	// C4, adjust FLL frequency if needed
	
	// Raise clock frequency to 48 MHz
	
	// ** Make table showing register settings as we go through different mode changes steps
	Instrumented_Delay_Loop(1); 
	
#endif // CLOCK_SETUP == 0
	Debug_Set(ps_pins, ps_pc, 15);

	while(1) // L.266
		;
}
#endif

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	Init_Debug_Signals();
	Init_RGB_LEDs();
	
	// LP_Testing2();
	Select_FEI_Fast();
	
	Sound_Init();	
	Sound_Enable_Amp(1);
	Play_Tone();
	Sound_Enable_Amp(0);
	
	LCD_Init();
	LCD_Text_Init(1);
	LCD_Erase();
	LCD_Text_PrintStr_RC(0,0, "HW4");

#if 0
	LCD_Set_Backlight_Brightness(15);
	while (1);
#endif
	
	LCD_Text_PrintStr_RC(1,0, "Accel...");
	i2c_init();											// init I2C peripheral
	if (!init_mma()) {							// init accelerometer
		I2C_OK = 0;	// code won't read accelerometer
		// Initialize variables for fake data
		acc_X = 0;
		acc_Y = 0;
		acc_Z = -4000;
	} else {
		I2C_OK = 1;
	}
	LCD_Text_PrintStr_RC(1,9, "Done");
	
	Delay(700);
	LCD_Erase();
	Control_RGB_LEDs(0,0,0);			

	osKernelInitialize();
	Create_OS_Objects();
	EventRecorderInitialize(EventRecordAll, 1U);
	osKernelStart();	
	while(1);
}
