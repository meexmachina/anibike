/*
 * swUART.c
 *
 * Created: 8/18/2011 3:14:02 AM
 *  Author: David
 */ 

/*****************************************************************
 *			H E A D E R    F I L E S
 *****************************************************************/
#include "AniBike2V7BT_Internal.h"

/*****************************************************************
 *			G L O B A L   V A R I A B L E S
 *****************************************************************/
extern uint8_t g_cpu_speed;

/*****************************************************************
 *			S T A T I C   V A R I A B L E S
 *****************************************************************/
// set up the iostream here for the printf&scanf statement.
FILE file_str = FDEV_SETUP_STREAM( swUART_PutChar, swUART_ReadChar, _FDEV_SETUP_RW);
void (*swUART_RxHandler)( void ) = NULL;
bool bInterruptEnabled = false;

/*****************************************************************
 *			F U N C T I O N    I M P L E M E N T A T I O N
 *****************************************************************/

/*****************************************************************
 *	swUART_ConfigureDevice
 *	Description:
 *		Initializes the sw uart transceiver. allocates the 
 *		virtual port 0 for PORTB.
 *	Input: iIntLevel - the Rx interrupt level 
					   (see swUART_SetRxInterruptLevel function).
 *	Output: none
 *****************************************************************/
void swUART_ConfigureDevice ( int iIntLevel )
{
	// Map port B to Virtual Port 0
	#ifdef UART_PORT_IS_PORTA
		PORT_MapVirtualPort0( PORTCFG_VP0MAP_PORTA_gc );	
	#else
		#ifdef UART_PORT_IS_PORTB
			PORT_MapVirtualPort0( PORTCFG_VP0MAP_PORTB_gc );
		#else
			#ifdef UART_PORT_IS_PORTC
				PORT_MapVirtualPort0( PORTCFG_VP0MAP_PORTC_gc );
			#else 
				#ifdef UART_PORT_IS_PORTD
					PORT_MapVirtualPort0( PORTCFG_VP0MAP_PORTD_gc );
				#else
					#ifdef UART_PORT_IS_PORTE
						PORT_MapVirtualPort0( PORTCFG_VP0MAP_PORTE_gc );
					#endif 
				#endif 
			#endif 
		#endif 
	#endif
	
		
	stdout = &file_str;
	stdin = &file_str;
	
	// configure pin directions
	VPORT0_DIR &= ~(1<<UART_RX_PIN);
	VPORT0_DIR |= (1<<UART_TX_PIN);
	VPORT0_OUT |= (1<<UART_TX_PIN);
	
	swUART_SetRxInterruptLevel ( iIntLevel );
	swUART_SetBaudRate ( 9600 );
}

/*****************************************************************
 *	swUART_SetBaudRate
 *	Description:
 *		Set the baud rate. 
 *	Input: baud_rate - 4800, 9600, 19200, 38400, ...
 *
 *		delay = alpha/(baud-gamma)-beta
 *		
 *		32MHz =>
 *					alpha = 5733700
 *					beta = 5.4553
 *					gamma = -988.4352
 *		16MHz =>
 *					alpha = 1407500
 *					beta = 5.1261
 *					gamma = -16.8067
 *
 *	Output: none
 *****************************************************************/
void	swUART_SetBaudRate ( uint32_t baud_rate)
{
	switch (baud_rate)
	{
		case (9600):
			if (g_cpu_speed==2)		SWUART_SET_DELAY(31);		// checked
			if (g_cpu_speed==8)		SWUART_SET_DELAY(66);		// checked
			if (g_cpu_speed==16)	SWUART_SET_DELAY(140);		// checked
			break;
		case (19200):
			if (g_cpu_speed==16)	SWUART_SET_DELAY(68);
			//if (g_cpu_speed==32)	SWUART_SET_DELAY(278);
			break;
		case (38400):
			if (g_cpu_speed==16)	SWUART_SET_DELAY(32);
			if (g_cpu_speed==32)	SWUART_SET_DELAY(140);
			break;
		case (57600):
			if (g_cpu_speed==16)	SWUART_SET_DELAY(19);
			if (g_cpu_speed==32)	SWUART_SET_DELAY(92);
			break;
		case (115200):
			if (g_cpu_speed==16)	SWUART_SET_DELAY(7);
			if (g_cpu_speed==32)	SWUART_SET_DELAY(43);
			break;		
		case (230400):
			if (g_cpu_speed==32)	SWUART_SET_DELAY(12);
			break;		
		case (460800):
			if (g_cpu_speed==32)	SWUART_SET_DELAY(7);		// checked
			break;	
		default:	
			break;
	};	
}

/*****************************************************************
 *	swUART_SetRxInterruptLevel
 *	Description:
 *		Set the RX interrupt and its level. 
 *	Input: iIntLevel - the Rx interrupt level:
 *					0 - interrupt is disabled
 *					1 - interrupt enabled but off
 *					2 - interrupt enabled at low priority
 *					3 - interrupt enabled at medium priority
 *					4 - interrupt enabled at high priority
 *	Output: none
 *****************************************************************/
void swUART_SetRxInterruptLevel ( int iIntLevel )
{
	if (iIntLevel != 0)
	{
		bInterruptEnabled = true;
		// Configure the interrupts for UART_PORT
		PORT_ConfigurePins( &UART_PORT,
							(1<<UART_RX_PIN),
							false,
							false,
							PORT_OPC_TOTEM_gc,
							PORT_ISC_FALLING_gc );
		
		switch (iIntLevel)
		{
			case 1:
				PORT_ConfigureInterrupt0( &UART_PORT, PORT_INT1LVL_OFF_gc, (1<<UART_RX_PIN) );	
			break;
			case 2:
				PORT_ConfigureInterrupt0( &UART_PORT, PORT_INT0LVL_LO_gc, (1<<UART_RX_PIN) );		
			break;
			case 3:
				PORT_ConfigureInterrupt0( &UART_PORT, PORT_INT0LVL_MED_gc, (1<<UART_RX_PIN) );		
			break;
			case 4:
				PORT_ConfigureInterrupt0( &UART_PORT, PORT_INT0LVL_HI_gc, (1<<UART_RX_PIN) );		
			break;
			default:
			break;
		}				
		PMIC.CTRL |= PMIC_LOLVLEN_bm|PMIC_MEDLVLEN_bm|PMIC_HILVLEN_bm;		
	
		// I don't turn on the interrupt handling for now. The main program will choose what to do.
	}		
	else
	{
		bInterruptEnabled = false;		
		
		// Configure the interrupts for UART_PORT
		PORT_ConfigurePins( &UART_PORT,
							(1<<UART_RX_PIN),
							false,
							false,
							PORT_OPC_TOTEM_gc,
							PORT_ISC_INPUT_DISABLE_gc );		
	}
}	

/*****************************************************************
 *	swUART_SetInterruptHandler
 *	Description:
 *		Assigns the RX interrupt handler function.
 *	Input: handling function. should be of the type:
		void handlerXXX ( void );
		can be NULL and its as if the interrupt is doing nothing.
 *	Output: none
 *****************************************************************/
void	swUART_SetInterruptHandler ( void (*Handler)( void ) )
{
	swUART_RxHandler = Handler;
}

/*****************************************************************
 *	PORTB_INT0_vect - Interrupt Service Routine
 *	Description: ISR for RX interrupt
 *	Input: none
 *	Output: none
 *****************************************************************/
ISR(PORTB_INT0_vect)
{
	cli ();
	
	if (swUART_RxHandler!=NULL)
		swUART_RxHandler ( );
	
	sei ( );
}


/*****************************************************************
 *	swUART_Delay
 *	Description:
 *		Delay for half the bit-rate. The used function is:
 *			delay_cc = 3*SWUART_DELAY_TIME + 10
 *		For FCPU of "f_c", lets say for T=1/f_c, and baud for f_b
 *		this function should get SWUART_DELAY_TIME such that
 *			SWUART_DELAY_TIME = (f_c/(2*f_b)-10)/3;
 *		for example:
 *			f_c = 2MHz; f_b = 9600 => SWUART_DELAY_TIME=31.4
 *	Input: none
 *	Output: none
 *****************************************************************/
void swUART_Delay_Tx ( void )
{
	asm volatile (
			"					push	r24				\n\t"
			"UART_delay:		in		r24,%0			\n\t"
			"UART_delay1:		dec		r24				\n\t"
			"					brne	UART_delay1		\n\t"
			"					pop		r24				\n\t"
			"					nop						\n\t"
			"					ret						\n\t"
			::"M" (SWUART_DELAY_REGISTER_ADDR)
	);	
}

void swUART_Delay_Rx ( void )
{
	asm volatile (
			"					push	r24				\n\t"
			"UART_delay2:		in		r24,%0			\n\t"
			"UART_delay3:		dec		r24				\n\t"
			"					brne	UART_delay3		\n\t"
			"					pop		r24				\n\t"
			"					nop						\n\t"
			"					ret						\n\t"
			::"M" (SWUART_DELAY_REGISTER_ADDR)
	);	
}

/*****************************************************************
 *	swUART_PutChar
 *	Description:
 *		Writes a single char (uiData)
 *	Input: uint8_t uiData - 8bit char to write
 *	Output: none
 *****************************************************************/
void swUART_PutChar ( uint8_t uiData )
{	 
	asm volatile (
					"			push	r17				\n\t"
					"			in		r17, 0x3f		\n\t"		// store the status register
					"			push	r17				\n\t"
					"			push	r24				\n\t"
								
					"pchar:		ldi		r17,%0			\n\t"		// stop bit = 1			%0 = bitcnt;	%1 = total num of bits
					"			com		r24				\n\t"		// Invert everything	%2 = uiData
					"			sec						\n\t"		// Start bit
					"pchar0:	brcc	pchar1			\n\t"		// If carry set
					"			cbi		%1,%2			\n\t"		// send a '0'			%3 = UART_PORT; %4 = Pin_TxD
					"			rjmp	pchar2			\n\t"		// else					
					"pchar1:	sbi		%1,%2			\n\t"		// send a '1'
					"			nop						\n\t"
					"pchar2:	rcall	swUART_Delay_Tx	\n\t"		// One bit delay
					"			rcall	swUART_Delay_Tx	\n\t"
					"			lsr		r24				\n\t"		// Get next bit
					"			dec		r17				\n\t"		// If not all bit sent
					"			brne	pchar0			\n\t"		// send next
					
					"			pop		r24				\n\t"
					"			pop		r17				\n\t"
					"			out		0x3f, r17		\n\t"		// recall the status register
					"			pop		r17				\n\t"

					"			ret						\n\t"		// else return

					:: "M" (SWUART_TOTAL_NUM_BITS_TX), "I" (UART_PORT_OUT), "I" (UART_TX_PIN)
	);
}

/*****************************************************************
 *	swUART_ReadChar
 *	Description:
 *		Waits for a single char (8 bit). the moment it is accepted,
 *		it reads it and returns its value. The waiting part is
 *		done by polling the UART_RX_PIN - exhaustive process. 
 *	Input: none
 *	Output: uint8_t - read value
 *****************************************************************/
uint8_t swUART_ReadChar ( void )
{
	asm volatile (
					"			push	r19				\n\t"
					"			in		r19, 0x3f		\n\t"		// store the status register
					"			push	r19				\n\t"
													
					"getchar:	ldi		r19,%0			\n\t"		// stop bit = 1			%0 = bitcnt;	%1 = total num of bits
					"getchar1:	sbic	%1,%2			\n\t"		// wait for the start bit on the RX pin
					"			rjmp 	getchar1		\n\t"		// go back to the last line - polling failed
					"			rcall	swUART_Delay_Rx	\n\t"		// Half bit delay
					"getchar2:	rcall	swUART_Delay_Rx	\n\t"		// One bit delay
					"			rcall	swUART_Delay_Rx	\n\t"
				
					"			clc						\n\t"		// clear carry
					"			sbic 	%1,%2			\n\t"		// if RX pin high
					"			sec						\n\t"		
					"			dec 	r19				\n\t"		// If bit is stop bit
					"			breq 	getchar3		\n\t"		// return or... else...
					"			ror 	r24				\n\t"		// shift bit into Rxbyte
					"			rjmp 	getchar2		\n\t"		// go get next
										
					"getchar3:							\n\t"
					"			pop		r19				\n\t"
					"			out		0x3f, r19		\n\t"		// recall the status register
					"			pop		r19				\n\t"
					"			ret						\n\t"		// return stuff		
					
					:: "M" (SWUART_TOTAL_NUM_BITS_RX), "I" (UART_PORT_IN), "I" (UART_RX_PIN)
	);	
	
	return 0;		// only for compiler because the returning is already in assembly
}


/*int putchar_io (char c, FILE *unused)
{
    if (c == '\n')
    {
        swUART_PutChar ( (uint8_t)('\n') );
        swUART_PutChar ( (uint8_t)('\r') );
    }
    else
    {
      swUART_PutChar ( (uint8_t)(c) );
    }
    return 0;
}*/