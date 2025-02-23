#include <MK64F12.H>
#include "LEDs.h"
#include "gpio_defs.h"

void Init_RGB_LEDs(void) {
	// Enable clock to ports B and E
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK;;
	
	// Make 3 pins GPIO
	PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);          
	PORTE->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTE->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);          
	PORTB->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);          
	
	// Set ports to outputs
	PTB->PDDR |= MASK(RED_LED_POS) | MASK(BLUE_LED_POS);
	PTE->PDDR |= MASK(GREEN_LED_POS);
}

void Control_RGB_LEDs(unsigned int red_on, unsigned int green_on, unsigned int blue_on) {
	if (red_on) {
			PTB->PCOR = MASK(RED_LED_POS);
	} else {
			PTB->PSOR = MASK(RED_LED_POS); 
	}
	if (green_on) {
			PTE->PCOR = MASK(GREEN_LED_POS);
	}	else {
			PTE->PSOR = MASK(GREEN_LED_POS); 
	} 
	if (blue_on) {
			PTB->PCOR = MASK(BLUE_LED_POS);
	}	else {
			PTB->PSOR = MASK(BLUE_LED_POS); 
	}
}	
