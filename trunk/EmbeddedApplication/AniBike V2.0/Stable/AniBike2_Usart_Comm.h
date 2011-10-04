#include <avr/io.h>
#include <compat/deprecated.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

// This function is used to initialize the USART
// at a given UBRR value
void USARTInit(uint16_t ubrr_value);

// This function is used to read the available data
// from USART. This function will wait untill data is
// available.
char USARTReadChar();

// This fuction writes the given "data" to
// the USART which then transmit it via TX line
void USARTWriteChar(char data);

void USARTWriteString ( char* st );
