// Libraries
/* -------------------------------------------------------*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART.h"
#include <stdio.h>

void decodeNMEA();

#define BUFFER_SIZE 100
#define MAX_WORD_COUNT 4
#define INPUT_BAUD 19200
#define OUTPUT_BAUD 4800


uint8_t ready = 0;
uint8_t readyFinal = 0;
uint8_t ending = 0;
char receiveBuffer[BUFFER_SIZE];
char gps[MAX_WORD_COUNT][BUFFER_SIZE];
uint8_t gpsSize = 0;
volatile uint8_t receiveBufferSize = 0;
volatile uint32_t TCA_t_counter = 0;


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

ISR(TCA0_OVF_vect)
{
	
	TCA_t_counter += 1;
	// Clear the interrupt flag, to prevent instantly jumping back into the ISR again.
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}
int main(void)
{
	CPU_CCP = 0xD8;
	CLKCTRL.MCLKCTRLB = 0x01;
	
	TCA0.SINGLE.PER = 10;//1920 = 100ms; 191 = 10ms
	TCA0.SINGLE.CTRLA |= (TCA_SINGLE_CLKSEL_DIV1024_gc) | (TCA_SINGLE_ENABLE_bm);
	TCA0.SINGLE.INTCTRL |= (TCA_SINGLE_OVF_bm);
	
	USART_init();
	USART_changeBAUD(INPUT_BAUD);
	sei();
	
	while (1)
	{
		decodeNMEA();
	}
}
void decodeNMEA(){
	if (ready)
	{
		ready = 0;
		if (gpsSize < 5)
		{
			for (int i=0;i<80;i++){
				gps[gpsSize][i] = receiveBuffer[i];
				if(receiveBuffer[i] == 0x00){
					break;
				}
			}
			TCA_t_counter = 0;
			char *command = strtok(receiveBuffer, ",");
			if ((strcmp(command, "$GPRMC") == 0) || (strcmp(command, "$GPGGA") == 0) || (strcmp(command, "$GPGSA") == 0))
			{
				
				gpsSize ++;
			}
		}
	}
	if (TCA_t_counter >= 200 && gpsSize >= 1)
	{
		
		USART_changeBAUD(OUTPUT_BAUD);
		for (int i=0;i<gpsSize;i++){
			printString(gps[i]);
			while (!(USART0.STATUS & USART_TXCIE_bm));
			USART0.STATUS |= USART_TXCIE_bm;
		}
		gpsSize = 0;
		USART_changeBAUD(INPUT_BAUD);
	}
}
