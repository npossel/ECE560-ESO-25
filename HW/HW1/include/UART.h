#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <MKL25Z4.H>


#define UART_OVERSAMPLE_RATE 	(16)
#define SYS_CLOCK							(48e6)
#define BUS_CLOCK							(24e6)

void Init_UART2(uint32_t baud_rate);
void UART2_Transmit_Poll(uint8_t data);
uint8_t UART2_Receive_Poll(void);

void Send_String_Poll(uint8_t * str);
#endif
// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************   
