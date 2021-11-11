// Libraries
/* -------------------------------------------------------*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART.h"
#include <stdio.h>

void decodeNMEA();

uint8_t ready = 0;
uint8_t ending = 0;
char receiveBuffer[80];
char gps[80];
volatile uint8_t receiveBufferSize = 0;

ISR(USART0_RXC_vect){
	if (USART0_STATUS & USART_RXCIF_bm)
	{
		uint8_t temp;
		temp = USART0_RXDATAL;
		receiveBuffer[receiveBufferSize] = temp;
		receiveBufferSize++;
		if(temp == 0x0D){
			ending = 1;
		}
		if (temp == 0x0A && ending ==1)
		{
			ending = 0;
			ready = 1;
			receiveBuffer[receiveBufferSize]=0;
			receiveBufferSize=0;
		}
		
		USART0_STATUS |= USART_RXCIF_bm;
	}
}

int main(void)
{
	CPU_CCP = 0xD8;
	CLKCTRL.MCLKCTRLB = 0x01;
	
	USART_init();
	sei();
	
	while (1)
	{
		decodeNMEA();
	}
}
void decodeNMEA(){
	
	if (ready)
	{	
		ready=0;
		if (receiveBuffer[0] == '$')
		{
			for (int i=0;i<80;i++){
				gps[i] = receiveBuffer[i];
				if(receiveBuffer[i] == 0x00){
					break;
				}
			}
			char *command = strtok(receiveBuffer, ",");
			if (strcmp(command, "$GPRMC") == 0)
			{
				USART_changeBAUD(9600);
				printString(gps);
				while (!(USART0.STATUS & USART_TXCIE_bm));
				USART0.STATUS |= USART_TXCIE_bm;
				USART_changeBAUD(19200);
			}
		}
	}
}
