/*
 * UART.h
 *
 * Created: 13.11.2018 12:56:46
 *  Author: nilsk
 */ 


#ifndef UART_H_
#define UART_H_

#define USART0_BAUD_RATE(BAUD_RATE) ((float)F_CPU * 64 / (16 * (float)BAUD_RATE))

int8_t USART_init(void);
void USART_changeBAUD(uint16_t BAUD);
void USART_write(const uint8_t data);
uint8_t USART_read(void);
void sendChar(char c);
void printString(const char myString[]);


#endif /* UART_H_ */