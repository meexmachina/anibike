#include "AniBike2_Usart_Comm.h"

//__________________________________________________________________________________________________
// This function is used to initialize the USART
// at a given UBRR value
void USARTInit(uint16_t ubrr_value)
{
   // Set Directions
   DDRE = 0b11111110;

   //Set Baud rate

   UBRR0L = (unsigned char)ubrr_value;
   UBRR0H = (unsigned char)(ubrr_value>>8);

   //Enable The receiver and transmitter
   UCSR0B=(1<<RXEN0)|(1<<TXEN0);

   /*Set Frame Format
   >> Asynchronous mode
   >> No Parity
   >> 1 StopBit
   >> char size 8
   */
   UCSR0C=/*(1<<UMSEL0)|*/(3<<UCSZ00);



}

//__________________________________________________________________________________________________
// This function is used to read the available data
// from USART. This function will wait untill data is
// available.
char USARTReadChar()
{
   //Wait untill a data is available

   while(!(UCSR0A & (1<<RXC0)))
   {
      //Do nothing
   } 

   //Now USART has got data from host
   //and is available is buffer
   return UDR0;
}

//__________________________________________________________________________________________________
// This fuction writes the given "data" to
// the USART which then transmit it via TX line
void USARTWriteChar(char data)
{
   //Wait untill the transmitter is ready
   while(!(UCSR0A & (1<<UDRE0)))
   {
      //Do nothing
   }

   //Now write the data to USART buffer

   UDR0=data;
}

//__________________________________________________________________________________________________
void USARTWriteString ( char* st )
{
	uint8_t i=0;
	while (st[i]!='\0')
		USARTWriteChar(st[i++]);
}
