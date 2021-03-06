#include <avr/io.h>
#include "UART.h"

int8_t USART_init(){
	PORTB.DIRSET = PIN2_bm; // RX pin as output
	PORTB.OUTCLR = PIN2_bm; //

	PORTB.DIRCLR = PIN3_bm; // TX pin set as input
	//PORTB.OUTCLR = PIN3_bm;
	PORTB_PIN3CTRL &= ~PORT_PULLUPEN_bm; // Leave it as pulled-off

	USART0.BAUD = (uint16_t)USART0_BAUD_RATE(19200); /* set baud rate register */
	//USART0.BAUD = (uint16_t) 1429;
	USART0.CTRLA |= USART_RXCIE_bm;
	
	USART0.CTRLB = 0 << USART_MPCM_bp       /* Multi-processor Communication Mode: disabled */
	| 0 << USART_ODME_bp     /* Open Drain Mode Enable: disabled */
	| 1 << USART_RXEN_bp     /* Receiver enable: enabled */
	| USART_RXMODE_NORMAL_gc /* Normal mode */
	| 0 << USART_SFDEN_bp    /* Start Frame Detection Enable: disabled */
	| 1 << USART_TXEN_bp;    /* Transmitter Enable: enabled */

	USART0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc /* Asynchronous Mode */
	| USART_CHSIZE_8BIT_gc /* Character size: 8 bit */
	| USART_PMODE_DISABLED_gc /* No Parity */
	| USART_SBMODE_1BIT_gc; /* 1 stop bit */

	return 0;
}

void USART_changeBAUD(uint16_t BAUD){
	USART0.BAUD = (uint16_t)USART0_BAUD_RATE(BAUD); /* set baud rate register */
}

void USART_write(const uint8_t data)
{
	while (!(USART0.STATUS & USART_DREIF_bm));
	USART0.TXDATAL = data;
}

void printString(const char myString[]) {
	uint8_t i = 0;
	while (myString[i]) {
		USART_write((int)myString[i]);
		//sendChar(myString[i]);
		i++;
	}
}

void sendChar(char c)
{

	while( !(USART0.STATUS & USART_DREIF_bm) ); //Wait until DATA buffer is empty

	USART0.TXDATAL = c;

}

uint8_t USART_read()
{
	while (!(USART0.STATUS & USART_RXCIF_bm))
	;
	return USART0.RXDATAL;
}