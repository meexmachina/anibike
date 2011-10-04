/*
 * AniBike2V7BT_SwUSART.c
 *
 * Created: 6/15/2011 11:25:44 PM
 *  Author: David
 */ 

#include "AniBike2V7BT_SwUSART.h"

// UART Global Registers
uint8_t u_buffer ; // serial buffer
uint8_t u_bit_nbr; // bit counter
uint8_t u_status ; // status register

// UART Receiver buffer
uint8_t rx_buffer[SW_UART_RX_BUFFER_SIZE];
uint8_t rx_wr_index, rx_rd_index, rx_counter;
// This flag is set on UART Receiver buffer overflow
uint8_t rx_buffer_overflow3;

/******************************************************************************/  

// External Interrupt 0 service routine (used to initiate UART3 receive)
interrupt [EXT_INT0] void external_INT0(void)
{
	TCNT0 = 0x00;      
	OCR0  = START_BIT_LENGTH;  	// set timer reload value (to 1.5 bit length)
	TIFR  |= 0x02;		   		// clear timer compare flag
	TIMSK |= 0x02;		   		// enable timer compare interrupt
    /************************/
    EIMSK &= 0xFE;			   	// disable external interrupt INT0
    /************************/
	u3_status  = (1<<RX_BUSY); 	// set RX busy flag (clear all others)
	u3_buffer  = 0x00;         	// clear UART3 buffer
	u3_bit_nbr = 0xFF;    	   	// erase bit counter (set all bits)	 
}

// Timer 0 compare interrupt service routine (sends & receives the UART3 bits)
interrupt [TIM0_COMP] void TIMER0_compare(void)
{
  TCNT0 = 0x00; 
  OCR0  = DATA_BIT_LENGTH;  
  u3_bit_nbr++;  
  
  /*** check what are we doing: send or receive ? ***/
  if(u3_status & (1<<TX_BUSY)) 	// transmit process
  {
     if(u3_bit_nbr < 8)   	   	// data bits (bit 0...7)
     {
        TX3_PIN = (u3_buffer & 0x01);
        u3_buffer >>= 1;  	   	// next bit, please ! 
     }
     else                	   	// stop bit (bit #8)
     {
        TX3_PIN = HIGH;
        if(u3_bit_nbr == UART3_STOP) // ready! stop bit(s) sent
        {
           TIMSK &= 0xFD;   	// disable timer 0 interrupt
           u3_status = 0x00;   	// clear UART3 status register
           EIMSK |= 0x01;  	   	// enable external interrupt
        }
     }
  }
  /***********************************************************/
  else // receive process (1<<RX_BUSY)
  {
     if(u3_bit_nbr < 8)        	// data bits (bit 0...7) 
     {
        u3_buffer |= (RX3_PIN<<(u3_bit_nbr)); // sample here !
     }
     else       	   		   	// ready! it's the stop bit
     {
        TIMSK &= 0xFD;      	// disable timer 0 interrupt
        u3_status = 0x00;      	// clear UART3 status register
        // test the stop bit: (RX3_PIN == HIGH)
        if(RX3_PIN) // if no frame error...
   		{
   		   rx_buffer3[rx_wr_index3] = u3_buffer;
           if(++rx_wr_index3 == RX_BUFFER_SIZE3) 
              rx_wr_index3 = 0;
           if(++rx_counter3 > RX_BUFFER_SIZE3)
              rx_buffer_overflow3 = TRUE;
        }
        EIMSK |= 0x01;        	// enable external interrupt
     }   
  }
}

// get a character stored in the UART3 RX-buffer      
byte getchar3(void)
{
  byte data;
  
  while(rx_counter3 == 0)
        wdogtrig();
  data = rx_buffer3[rx_rd_index3];
  if(++rx_rd_index3 == RX_BUFFER_SIZE3) 
     rx_rd_index3 = 0;
  #asm("cli")
  --rx_counter3;
  #asm("sei")
  return data;
}

// put a character DIRECTLY to the UART3 buffer (it will CANCEL the RX flow)
void putchar3(byte c)
{
	while(u3_status & (1<<TX_BUSY)); // wait while UART3 is busy with sending 
	/************************/    
	EIMSK &= 0xFE;  				 // disable external interrupt
    /************************/
	TX3_PIN = LOW;				     // clear output (start bit) 
	/************************/
	TCNT0 = 0x00; 
	OCR0  = DATA_BIT_LENGTH;	     // set timer reload value (1 bit)
	TIFR  |= 0x02;	  		         // clear timer compare flag
	TIMSK |= 0x02;	  		         // enable timer compare interrupt
    /************************/
	u3_status  = (1<<TX_BUSY); 	     // set TX busy flag (clear all others)
	u3_buffer  = c;		  			 // copy data to buffer
	u3_bit_nbr = 0xFF;	  		     // erase bit counter (set all bits)
}

// UART3 initialization function (we MUST call this function BEFORE using UART3)
void initUART3(void) // UART3 = software UART
{
//RX
PORTD.0 = 1; // pull-up
DDRD.0  = 0; // input
//TX
PORTD.4 = 1; // logic 1 (high level = STOP TX)
DDRD.4  = 1; // output

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 500,000 kHz
// Mode: CTC top = OCR0
// OC0 output: Disconnected
ASSR =0x00;
TCCR0=0x0A;

// External Interrupt 0 initialization (for RX)
EICRA &= 0xFC;
EIMSK |= 0x01;
EIFR  |= 0x01;

// Timer/Counter 0 Interrupt initialization
TIMSK &= 0xFC;
}


// simple demo application: test it by connecting to a PC (COM1 for ex.)
void main1(void)
{
 initUART3(); // initialize the software UART3
 #asm("sei")  // enable global interrupts

 while(TRUE)
 {
   wdogtrig();     // trigger the doggy...
   if(rx_counter3) // if we received something (in the UART3 buffer)
   {
      putchar3(getchar3()); // just send it back !
   }    
 }
}  
