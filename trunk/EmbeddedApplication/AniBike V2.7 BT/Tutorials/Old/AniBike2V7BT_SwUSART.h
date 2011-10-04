/*
 * AniBike2V7BT_SwUSART.h
 *
 * Created: 6/15/2011 11:26:04 PM
 *  Author: David
 */ 


#ifndef ANIBIKE2V7BT_SWUSART_H_
#define ANIBIKE2V7BT_SWUSART_H_

#include <avr/io.h>

/*****************************************************************
 *			C O N S T A N T    D E F I N I T I O N S
 *****************************************************************/

#define SW_UART_BAUD				9600			// UART baud rate
#define SW_UART_STOP_BITS			1				// Number of stop bits
#define SW_UART_TX_PIN				PORTB//.3			// UART TX pin
#define SW_UART_RX_PIN				PORTB//.2		// UART RX pin (Async Interrupt)     
#define SW_UART_RX_BUFFER_SIZE		128				// UART buffer size



/*****************************************************************
 *			T Y P E S    D E F I N I T I O N S
 *****************************************************************/
// watchdog reset ("borrow" this from Introl)
//#define wdogtrig();		#asm("wdr") 
#define TRUE	1
#define FALSE	0  

// TX & RX levels
#define HIGH	1
#define LOW		0

// UART3 specific
#define	D	8	           	// frequency divider => (CLK / 8) = 4MHz/8 = 500.000 Hz
#define	N 	(CLK/(D*BAUD)) 	// number associated by Timer0 to 1 bit length 
#define START_BIT_LENGTH	((N + N/2) - (80/D)) // adjusted start bit length    
#define DATA_BIT_LENGTH		(N - (56/D))         // adjusted data bit length 
#define UART_STOP			(8 + STOP_BITS)		 // total number of bits
// bit positions in the Status Register of UART
#define	TX_BUSY 	6 		// busy sending data (internal - read only)
#define	RX_BUSY		7 		// busy receiving data (internal - read only)   




#endif /* ANIBIKE2V7BT_SWUSART_H_ */