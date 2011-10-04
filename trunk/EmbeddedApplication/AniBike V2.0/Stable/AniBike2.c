#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <compat/deprecated.h>
#include "Anibike_Internal.h"
#include <avr/interrupt.h>
#include <ctype.h>
#include <stdlib.h>
#include "AniBike2_DataFlash.h"
#include "AniBike2_FileSystem.h"

//==============================================================================================
//
//  Global Variables
//
//==============================================================================================

volatile uint32_t	g_iTimeCount = 0;
volatile uint8_t	g_iLast=0, g_iCurrent=0;
volatile uint32_t	g_iTimePerAngleElement = 0;
volatile uint32_t	g_iTimeElapsedRadius = 0;
volatile uint8_t	g_iRadiusLast = 0, g_iRadiusCurrent = 0;

volatile uint8_t	g_iPORTB_Buf = 0;
volatile uint8_t	g_iPORTC_Buf = 0;
volatile uint8_t	g_iPORTD_Buf = 0;
volatile uint8_t	g_iPORTF_Buf = 0;

volatile uint8_t	g_iFlashRead = 96;		// Flash needs reading
volatile uint8_t	g_bVeryHighSpeed = 0;

volatile uint8_t	Line_Buff1[96];
volatile uint8_t	Line_Buff2[96];
volatile uint8_t* 	g_iPWM_Seq_Active = NULL;
volatile uint8_t* 	g_iPWM_Seq_NotActive = NULL;

volatile uint16_t	g_iTimer0OvfCounter = 0;

volatile uint8_t	g_bBothRows = 1;
volatile uint8_t	g_bPowerSafeMode = 1;

volatile uint16_t	g_iCurrentFrameIndex = 0;	// 0:339
volatile uint8_t	g_iCurrentPosition = 0;		// 0:199
volatile uint8_t	g_iCurrentPolarity = 0;		// 0 or 1
volatile uint16_t	g_iCurrentFrameCycle = 0; 	// if Type:Ani => 1 if Type:Pic => 30
volatile FileEntry_ST 	g_sCurrentFileInfo;
volatile uint8_t		g_iCurrentFrame = 0;
volatile uint32_t		g_iStartingAddress = 0;
volatile uint32_t		g_iCurrentAddress = 0;
volatile uint8_t		g_iInnerCounter = 0;
volatile uint8_t		g_bEnterToIdleMode = 1;

extern uint8_t		bFS_Empty;

//==============================================================================================
//
//  Global Functions
//
//==============================================================================================



//==============================================================================================
void Anibike_InitLeds ( void )
{
	// Direction of rows
	DDRA  = 0b11111111;
	PORTA = 0b00000000;

	PORTB&= 0b00001111;
	DDRB &= 0b00001111;

	PORTC = 0b00000000;
	DDRC  = 0b00000000;

	PORTD = 0b00000000;
	DDRD  = 0b11000000;

	PORTF = 0b00000000;
	DDRF  = 0b00000000;

	PORTG = 0b00000001;
	DDRG =  0b00011101;
}

// Check if the second row is connected PG0 = '1'  PG1 = 'Z'
//==============================================================================================
void AniBike_CheckSecondRow ( void )
{
	uint8_t temp = PING;
	if ( temp & 0b00000010 )  // Connected
		g_bBothRows = 1;	
	else // Not Connected
		g_bBothRows = 0;
}

// Other Initializations
//==============================================================================================
void AniBike_InitPorts ( void )
{
	// This is checking if the other row is connected
	DDRG = 0b00000001;
	PORTG = 0b00000001;

	AniBike_CheckSecondRow (  );
}

// Shut down ADC
//==============================================================================================
void  ShutOffADC(void)
{
    ACSR = (1<<ACD);                        // disable A/D comparator
    ADCSRA = (0<<ADEN);                     // disable A/D converter
}

// Power save mode
//==============================================================================================
void AniBike_EnterPowerDown ( void )
{

/*   #define SLEEP_MODE_IDLE         0
   #define SLEEP_MODE_PWR_DOWN     1
   #define SLEEP_MODE_PWR_SAVE     2
   #define SLEEP_MODE_ADC          3
   #define SLEEP_MODE_STANDBY      4
   #define SLEEP_MODE_EXT_STANDBY  5
*/

	// ADC Disabling
	cli();
	TIMSK0 &= ~(1<<TOIE0); 					// Set Overflow interrupt OFF
	TIMSK2 &= ~(1<<OCIE2A);					// Set Timer 2 interrupt OFF
	
	UCSR0B |= (1<<RXCIE0);	// Enable UART RX interrupt		

	//CLKPR = 0b10001111;

	PRR = 0b00011101;
	//PRR = (1<<PRTIM1) | (1<<PRSPI) | (1<<PRADC);
	set_sleep_mode(SLEEP_MODE_IDLE);
    
	sleep_enable();
	sleep_bod_disable();
	sei();
	sleep_cpu();
	PRR = 0b00010001;
	sleep_disable();


	TIMSK0 |= (1<<TOIE0); 					// Set Overflow interrupt ON
	TIMSK2 |= (1<<OCIE2A);					// Set Timer 2 interrupt ON
	UCSR0B |= (1<<RXCIE0);	// Enable UART RX interrupt		    

	sei();

	//CLKPR = 0b00000000;
	PRR = 0b00010001;
	g_bEnterToIdleMode = 0;
}


// Other Initializations
//==============================================================================================
void AniBike_InitAll ( void )
{
	UNSET_BUFFERREADY;

	// Command Flow
	// =============
	// Init the SPI device
	dataflash_spi_init(  );

	// Init LEDs
	Anibike_InitLeds ( );

	// Init Other Ports
	AniBike_InitPorts ( );

	// Init the USART with baud rate = 57600bps
	USARTInit(20);

	// Init External interrupt
	EICRA |= (1<<ISC01);	// External Interrupt sense HIGH->LOW transit
//	EICRA = 0;
	EIMSK |= (1<<INT0);		// External Interrupt Set the Hall Sensor (INT0 pin)

	// Init Uart RX interrupt
	UCSR0B |= (1<<RXCIE0);	// Enable UART RX interrupt

	// CTC mode inter radius timer
	// Set Overflow and counting on Timer 1
	TCCR1B |= (1<<CS11)|(1<<WGM12); 		// Prescalar 8 - why?????????????
	TIMSK1 |= (1<<OCIE1A);//(1<<TOIE0); 	// Interrupt
	OCR1A = 0xFFFF;

	// Between Hall sensor counter
	// Set Timer 0 counting Overflows
	TCCR0A |= (1<<CS01); 					// Prescalar 8
	TIMSK0 = (1<<TOIE0); 					// Set Overflow interrupt
	
	// PWM Timing counter
	// Set Timer 2 counting Overflows
	TCCR2A |= (1<<WGM21)|(1<<CS21); 		// Prescalar 8, Waveform generator form,
	TIMSK2 |= (1<<OCIE2A); 					// Set Compare interrupt
	

	g_iPWM_Seq_Active = Line_Buff1;
	g_iPWM_Seq_NotActive 	= Line_Buff2;

	// Set the timing regime
	OCR2A = 0x49;							// Set 30usec
	TCNT2 = 0xb7;

//	OCR2A = 0x3d;							// Set 25usec
//	TCNT2 = 0xc3;	

	g_iFlashRead = 96;	// Flash needs reading

	SET_ROW(0);
	ROW_PATTERN = 1;

	// ADC Disabling
	PRR = 0;
	PRR |= (1<<PRADC);

	ShutOffADC();
	
	AniBike_EnterPowerDown (  );
}




// MAIN THREAD
//==============================================================================================
int main ()
{
	uint8_t iReadTemp = 0;

	SET_FIRST_BUFFER;

	//================================================
	//////////////// INITIALIZATIONS /////////////////
	//================================================
	AniBike_InitAll ();

	sei (); 
	//================================================
	////////////////// OPERATIONAL ///////////////////
	//================================================

	for (;;)
	{	
#ifdef __ENABLE_POWER_SAFE
		if ( g_bPowerSafeMode ) 
		{
			//ROW_PORT = 0x00;
			ROW_PATTERN = 0;
			if (g_bEnterToIdleMode == 1 )
				AniBike_EnterPowerDown (  );
			continue;		// 6cc
		}
#endif

		// Advance Row Number
		ADVANCE_ROW;

		if (ROW_NUM==0) 				// 5cc
		{
			ADVANCE_COUNTER;
		}


		asm ( "call PrepareBuffers" );			// 10.8usec / 7usec	
		
		// Buffers Ready flag
		SET_BUFFERREADY;						// 2cc
		

		// Next Memory Lines Read
		if ( IS_HIGHSPEED )
		{
			while ( (BUFFER_IS_READY) && g_iFlashRead)
			{
				// Read next memory slots
				READ_FLASH_BYTE;
				iReadTemp = SPDR;

				if (g_bVeryHighSpeed)
					iReadTemp = (iReadTemp>>1)&0b01110111;
			
				if ( (WHICH_BUFFER) == 0 )
				{

					Line_Buff2[96-g_iFlashRead] = iReadTemp;

				}
				else
				{
					Line_Buff1[96-g_iFlashRead] = iReadTemp;
				}
				g_iFlashRead --;
			}			
		}
		else
		{
			if ( g_iFlashRead > 48 )
			{
				while ( (BUFFER_IS_READY) && g_iFlashRead > 48)
				{
					// Read next memory slots
					READ_FLASH_BYTE;	

					//USARTWriteChar(c);
						
					if ( (WHICH_BUFFER) == 0 )
					{
						if ( g_iCurrentPolarity == 0 )	// First is L2 and second is L1
						{
							Line_Buff2[96-g_iFlashRead] = SPDR;	// L2
						}
						else
						{
							Line_Buff2[144-g_iFlashRead] = SPDR;
						}
					}
					else
					{
						if ( g_iCurrentPolarity == 0 )	// First is L2 and second is L1
						{
							Line_Buff1[96-g_iFlashRead] = SPDR;	// L2
						}
						else
						{
							Line_Buff1[144-g_iFlashRead] = SPDR;
						}
					}
					// PUSH information to Line2	

					g_iFlashRead --;
				}
			}
			else if (g_iFlashRead)
			{
				while ( (BUFFER_IS_READY) && g_iFlashRead)
				{
					// Read next memory slots
					READ_FLASH_BYTE;
				
				
					//USARTWriteChar(c);
						
					if ( (WHICH_BUFFER) == 0 )
					{
						if ( g_iCurrentPolarity == 0 )	// First is L2 and second is L1
						{
							Line_Buff2[96-g_iFlashRead] = SPDR;	// L2
						}
						else
						{
							Line_Buff2[48-g_iFlashRead] = SPDR;
						}
					}
					else
					{
						if ( g_iCurrentPolarity == 0 )	// First is L2 and second is L1
						{
							Line_Buff1[96-g_iFlashRead] = SPDR;	// L2
						}
						else
						{
							Line_Buff1[48-g_iFlashRead] = SPDR;
						}
					}

					g_iFlashRead --;
				}
			}
		}

		// Wait for next request
		while ( BUFFER_IS_READY ) {}
	}

}



//==============================================================================================
//
//  Interrupt Procesing Code
//
//==============================================================================================

// Interrupt for changing single angle units
//==============================================================================================
ISR (TIMER1_COMPA_vect/*, ISR_NOBLOCK*/)
{
	unsigned char adrByte1, adrByte2, adrByte3;

	ROW_PORT = 0;
	ROW_PATTERN = 0;

#ifdef __ENABLE_POWER_SAFE
	if (g_bPowerSafeMode == 1)
		return;
#endif

	g_iInnerCounter++;
	if ( g_iInnerCounter == 200 ) 
	{
		g_iCurrentPolarity = 0;		

#ifdef __ENABLE_POWER_SAFE
		g_bPowerSafeMode = 1;		
#endif
		return;
	}

	// Switch Buffers
	if ( WHICH_BUFFER == 0 )
	{
		SET_SECOND_BUFFER;
	}
	else
	{
		SET_FIRST_BUFFER;
	}

	if ( IS_HIGHSPEED )
	{
		g_iFlashRead = 48;		// Flash needs reading

		CS_UP;
		if (g_iInnerCounter == 100)
		{
			g_iCurrentPolarity = 1;	
			g_iCurrentAddress = g_iStartingAddress;
		}
		else
		{
			g_iCurrentAddress += 96;
		}

		adrByte1 = (g_iCurrentAddress>>16)&0xff;
		adrByte2 = (g_iCurrentAddress>>8)&0xff;
		adrByte3 = g_iCurrentAddress&0xff;  
		CS_DOWN;

		SPDR = READ_ARRAY;
		while (! (SPSR & (1<<SPIF))) {}
		SPDR = adrByte1;
		while (! (SPSR & (1<<SPIF))) {}
		SPDR = adrByte2;
		while (! (SPSR & (1<<SPIF))) {}
		SPDR = adrByte3;
		while (! (SPSR & (1<<SPIF))) {}

	}
	else
	{
		g_iFlashRead = 96;		// Flash needs reading

		g_iCurrentAddress += 96;

		if ( g_iInnerCounter == 100 )
		{	
			CS_UP;
			g_iCurrentAddress = g_iStartingAddress;
	
			adrByte1 = (g_iStartingAddress>>16)&0xff;
			adrByte2 = (g_iStartingAddress>>8)&0xff;
			adrByte3 = g_iStartingAddress&0xff;  
			CS_DOWN;

			SPDR = READ_ARRAY;
			while (! (SPSR & (1<<SPIF))) {}
			SPDR = adrByte1;
			while (! (SPSR & (1<<SPIF))) {}
			SPDR = adrByte2;
			while (! (SPSR & (1<<SPIF))) {}
			SPDR = adrByte3;
			while (! (SPSR & (1<<SPIF))) {}
			
			g_iCurrentPolarity = 1;

		}
	}

	SET_ROW(0);
	ROW_PATTERN = 1;
	PWM_COUNTER = 0;

}


// Interrupt for PWM timing
//==============================================================================================
ISR (TIMER2_COMP_vect, ISR_NAKED/*, ISR_NOBLOCK*/)		// Takes 2 usec
{
//	if (g_bPowerSafeMode == 1)
//		return;
	asm ( "push r1" );
	asm ( "push r0" );
	asm ( "in r0, 0x3f" );				// Store the status register
	asm ( "push r0" );
	asm ( "clr r1" ); 					// Clear register

	asm ( "push r24" );	
	asm ( "push r25" );

	asm ( "in	r24, 0x04" );			// Load the contants of DDRB
	asm ( "andi	r24, 0x0F" );			// Make and with 0x0F
	asm ( "out	0x04, r24" );			// Update DDRB

	asm ( "out	0x07, r1" );			// Update DDRC to be 0

	asm ( "in	r24, 0x0A" );			// Load the contants of DDRD
	asm ( "andi	r24, 0xC3" );			// Make and with 0b00111100
	asm ( "out	0x0A, r24" );			// Update DDRD
	
	asm ( "out	0x10, r1" );			// Update DDRF to be 0

	asm ( "out	0x02, r1" );			// Update PORTA to be 0

	// Manipulate DDRB
	asm ( "lds	r25, g_iPORTB_Buf" );
	asm ( "in	r24, 0x04" );
	asm ( "or	r24, r25" );
	asm ( "out	0x04, r24" );

	// Manipulate DDRC
	asm ( "lds	r25, g_iPORTC_Buf" );
	asm ( "out	0x07, r25" );

	// Manipulate DDRD
	asm ( "lds	r25, g_iPORTD_Buf" );
	asm ( "in	r24, 0x0A" );
	asm ( "or	r24, r25" );
	asm ( "out	0x0A, r24" );

	// Manipulate DDRF
	asm ( "lds	r25, g_iPORTF_Buf" );
	asm ( "out	0x10, r25" );

	PORTA = ROW_PATTERN;
	
	// Store 0 to g_iBuffersReady
	UNSET_BUFFERREADY;

	asm ( "pop r25" );
	asm ( "pop r24" );

	asm ( "pop r0" );
	asm ( "out 0x3F,r0" );
	asm ( "pop r0" );
	asm ( "pop r1" );

	reti ();
}


// Timer0 (8bit) Overflow interrupt
// Usage: for timing wheel cycle
//==============================================================================================
ISR (TIMER0_OVF_vect, /*ISR_NAKED*//*, ISR_NOBLOCK*/)
{
/*
PUSH      R1             Push register on stack
PUSH      R0             Push register on stack
IN        R0,0x3F        In from I/O location
PUSH      R0             Push register on stack
CLR       R1             Clear Register
PUSH      R24            Push register on stack
PUSH      R25            Push register on stack
LDS       R24,0x05E6     Load direct from data space
LDS       R25,0x05E7     Load direct from data space
ADIW      R24,0x01       Add immediate to word
STS       0x05E7,R25     Store direct to data space
STS       0x05E6,R24     Store direct to data space
POP       R25            Pop register from stack
POP       R24            Pop register from stack
POP       R0             Pop register from stack
OUT       0x3F,R0        Out to I/O location
POP       R0             Pop register from stack
POP       R1             Pop register from stack
RETI 
*/
	if (g_iTimer0OvfCounter&0x4000)
	{
		g_bEnterToIdleMode = 1;
		g_bPowerSafeMode = 1;
		g_iTimer0OvfCounter = 0;
		g_bVeryHighSpeed = 0;
		return;
	}
	
	g_iTimer0OvfCounter++;
}


// Hall Sensor Interrupt
// Each Hall signal we calculate the appropriate timing parameters
//==============================================================================================
ISR (INT0_vect/*, ISR_NOBLOCK*/)
{ 
	unsigned char adrByte1, adrByte2, adrByte3;

//	if ( g_bOutOfPowerSafeMode )
//	{	
//		g_bOutOfPowerSafeMode = 0;
//		g_iTimer0OvfCounter = 0;
//		TCNT2 = 0;
//		EICRA |= (1<<ISC01);	// External Interrupt sense HIGH->LOW transit
//		return ;
//	}

	TCNT2 = 0;

#ifdef __ENABLE_POWER_SAFE
	g_bPowerSafeMode = 0;		//2cc
	g_bEnterToIdleMode = 0;
#endif

	g_iInnerCounter = 0;		//2cc

	// if >440 msec -> low speed
	// if <400 msec -> hi speed
	// Hysteresis
	if (g_iTimer0OvfCounter<1400)
	{
#ifdef __ENABLE_POWER_SAFE
		g_bPowerSafeMode = 1;
#endif
		g_iTimer0OvfCounter = 0;
		g_bVeryHighSpeed = 0;
		return ;
	}
	else if (g_iTimer0OvfCounter<1800)
	{
//		OCR2A = 0x2e;							// Set 19usec
//		OCR2A = 0x24;							// Set 15usec
		OCR2A = 0x29;							// Set 17usec
		TCNT2 = 0;//0xd2;
		g_bVeryHighSpeed = 1;
		SET_HIGHSPEED;
	}
	else if (g_iTimer0OvfCounter<2800)
	{
//		OCR2A = 0x2e;							// Set 19usec
//		OCR2A = 0x1d;							// Set 12usec
//		OCR2A = 0x29;							// Set 17usec
		OCR2A = 0x24;							// Set 15usec
		TCNT2 = 0;//0xd2;
		g_bVeryHighSpeed = 0;
		SET_HIGHSPEED;
	}
#ifdef __ENABLE_POWER_SAFE
	else if (g_iTimer0OvfCounter<12000)
#else
	else
#endif
	{
//		OCR2A = 0x3d;							// Set 27usec
//		OCR2A = 0x33;							// Set 21usec
		OCR2A = 0x38;							// Set 23usec
//		OCR2A = 0x3c;							// Set 25usec
		TCNT2 = 0;//0xc3;
		g_bVeryHighSpeed = 0;
		UNSET_HIGHSPEED
	}
#ifdef __ENABLE_POWER_SAFE
	else	// Enter Power-down mode
	{
//		AniBike_EnterPowerDown ( );
		g_bEnterToIdleMode = 1;
		g_bPowerSafeMode = 1;
		g_iTimer0OvfCounter = 0;
		g_bVeryHighSpeed = 0;
		return;
	}
#endif

	// Frame managing stuff
	if ( g_iCurrentFrameCycle == 0 )
	{	// just finished a cycle

		// prepare next file projection
		// ============================
			
		// read next 
		if ( FS_ReadNextEntry ( &g_sCurrentFileInfo ) )
		{
			CS_UP;
			g_iCurrentPolarity = 0;
			g_iCurrentFrameCycle = 48;

			// start reading first frame
			g_iCurrentFrame = 0;
			g_iStartingAddress = (uint32_t)(g_sCurrentFileInfo.iBlockList[0]) * (uint32_t)(FS_FRAME_CAP);
			g_iCurrentAddress = g_iStartingAddress;
			
			if (IS_HIGHSPEED)
				g_iCurrentAddress += FS_HALF_FRAME_SIZE;	

			adrByte1 = (g_iCurrentAddress>>16)&0xff;
			adrByte2 = (g_iCurrentAddress>>8)&0xff;
			adrByte3 = g_iCurrentAddress&0xff;  
			CS_DOWN;

			SPDR = READ_ARRAY;
			while (! (SPSR & (1<<SPIF))) {}
			SPDR = adrByte1;
			while (! (SPSR & (1<<SPIF))) {}
			SPDR = adrByte2;
			while (! (SPSR & (1<<SPIF))) {}
			SPDR = adrByte3;
			while (! (SPSR & (1<<SPIF))) {}
		}
		else
		{
			g_bPowerSafeMode = 1;

		}
	}
	else
	{	// in the middle of projecting the same picture
		CS_UP;
		
		g_iCurrentFrameCycle --;	// decrease the projection time
		g_iCurrentPolarity = 0;

		g_iCurrentFrame ++;
		if ( g_sCurrentFileInfo.iNumFrames <= g_iCurrentFrame )
			g_iCurrentFrame = 0;

		g_iStartingAddress = (uint32_t)(g_sCurrentFileInfo.iBlockList[g_iCurrentFrame]) * (uint32_t)(FS_FRAME_CAP);
		g_iCurrentAddress = g_iStartingAddress;
		
		if (IS_HIGHSPEED)
			g_iCurrentAddress += FS_HALF_FRAME_SIZE;

		
		adrByte1 = (g_iCurrentAddress>>16)&0xff;
		adrByte2 = (g_iCurrentAddress>>8)&0xff;
		adrByte3 = g_iCurrentAddress&0xff;  

		CS_DOWN;

		SPDR = READ_ARRAY;
		while (! (SPSR & (1<<SPIF))) {}
		SPDR = adrByte1;
		while (! (SPSR & (1<<SPIF))) {}
		SPDR = adrByte2;
		while (! (SPSR & (1<<SPIF))) {}
		SPDR = adrByte3;
		while (! (SPSR & (1<<SPIF))) {}
		
		
	}


	
	SET_ROW(0);
	ROW_PATTERN = 1;
	PWM_COUNTER = 0;
//	SET_FIRST_BUFFER;
	
	if ( IS_HIGHSPEED )
		g_iFlashRead = 48;		// Flash needs reading
	else 
		g_iFlashRead = 96;		// Flash needs reading

	// divide by ANGULAR_RESOLUTION = 200
	// for ANGULAR_RESOLUTION radial pixels 
	//	more accurate = 1+1/4+1/16-1/32 = 1.281 ~= 256/200
	OCR1A =  g_iTimer0OvfCounter + (g_iTimer0OvfCounter/4)+(g_iTimer0OvfCounter/16)-(g_iTimer0OvfCounter/32);
	TCNT1 = 0;

//	sprintf( text, "OCR:%d, Frm#:%d, Addr:%lu, Cyc:%d\r\n", OCR1A, g_iCurrentFrame, g_iCurrentAddress, g_iCurrentFrameCycle);
//	USARTWriteString ( text );

	g_iTimer0OvfCounter = 0;		// 8cc
}



// Uart Communication interrupt
//==============================================================================================
ISR (USART0_RX_vect)
{ 
	unsigned char dummy;
	PRR = 0b00010001;

	// Shut down all leds
	ROW_PORT = 0x00;

	// Disable All Interrupts
	cli ( );

	// Run communication function
	DataTransferMode ( );

	// Flush input buffer
	while ( UCSR0A & (1<<RXC0 ) ) dummy = UDR0; 


	// Enable all interrupts
	sei ( );
}



















