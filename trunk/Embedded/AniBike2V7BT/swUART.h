/*
 * swUART.h
 *
 * Created: 8/18/2011 3:14:17 AM
 *  Author: David
 */ 

#ifndef SWUART_H_
#define SWUART_H_

#include <stdio.h>

/*****************************************************************
 *			C O N S T A N T    D E F I N I T I O N S
 *****************************************************************/
#define SWUART_NUM_OF_STOP_BITS		1
#define SWUART_TOTAL_NUM_BITS_TX	(9+SWUART_NUM_OF_STOP_BITS)
#define SWUART_TOTAL_NUM_BITS_RX	(8+SWUART_NUM_OF_STOP_BITS)
#define SWUART_DELAY_REGISTER		GPIO_GPIO3
#define SWUART_DELAY_REGISTER_ADDR	0x0003
#define SWUART_SET_DELAY (d)		{SWUART_DELAY_REGISTER = (d);}
//#define SWUART_DELAY_TIME_TX		140								
//#define SWUART_DELAY_TIME_RX		(SWUART_DELAY_TIME_TX)			

/* SWUART_DELAY_TIME_TX1
 * --------------------
 * SWUART_DELAY_TIME = (f_c/(2*f_b)-10)/3;
 * 9600 bps @ 2 MHz 	<= 31
 * 9600 bps @ 10 MHz	<= 105 
 * 9600 bps @ 16 MHz	<= 140
 * 9600 bps @ 8 MHz		<= 66
 * 460800 bps @ 32 MHz	<= 7
 */

/*****************************************************************
 *			F U N C T I O N    D E F I N I T I O N S
 *****************************************************************/
void	swUART_ConfigureDevice ( int iIntLevel );	// iIntLevel = 0,1,2,3,4 (not used, off, low, med, high)
void	swUART_SetBaudRate ( uint32_t baud_rate);
void	swUART_SetRxInterruptLevel ( int iIntLevel );	// iIntLevel = 0,1,2,3,4 (not used, off, low, med, high)
void	swUART_SetInterruptHandler ( void (*Handler)( void ) );
void	swUART_PutChar ( uint8_t uiData ) __attribute__((naked));
void	swUART_Delay_Rx ( void )  __attribute__((naked));
void	swUART_Delay_Tx ( void )  __attribute__((naked));
uint8_t swUART_ReadChar ( void ) __attribute__((naked));

//int		putchar_io (char c, FILE *unused);
#endif /* SWUART_H_ */