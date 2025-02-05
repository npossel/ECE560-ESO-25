#include <MKL25Z4.H>
#include "LEDs.h"
#include "gpio_defs.h"

void Init_RGB_LEDs(void) {
	// Enable clock to ports B and D
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;;
	
	// Make 3 pins GPIO
	PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);          
	PORTB->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);          
	PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);          
	
	// Set ports to outputs
	PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
	PTD->PDDR |= MASK(BLUE_LED_POS);
}

void Control_RGB_LEDs(unsigned int red_on, unsigned int green_on, unsigned int blue_on) {
	switch (red_on) {
		case 0: 	
			PTB->PSOR = MASK(RED_LED_POS); 
			break;
		case 1: 	
			PTB->PCOR = MASK(RED_LED_POS); 
			break;
		default:
			break;
	}
	switch (green_on) {
		case 0: 	
			PTB->PSOR = MASK(GREEN_LED_POS); 
			break;
		case 1: 	
			PTB->PCOR = MASK(GREEN_LED_POS); 
			break;
		default:
			break;
	}
	switch (blue_on) {
		case 0: 	
			PTD->PSOR = MASK(BLUE_LED_POS); 
			break;
		case 1: 	
			PTD->PCOR = MASK(BLUE_LED_POS); 
			break;
		default:
			break;
	}
}	
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
