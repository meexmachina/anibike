/*
 * AniBike2V7BT_DataLink.c
 *
 * Created: 8/22/2011 8:40:02 PM
 *  Author: David
 */ 
#include "AniBike2V7BT_Internal.h"

SPI_Master_t		spiMasterC;
volatile uint8_t	rxBuffer[128];
volatile uint8_t	rxLength = 0;
volatile uint8_t	reDataReady = 0;

//__________________________________________________________________________________________________
void anibike_dl_initialize		( ANIBIKE_DL_TYPE_EN enNodeType )
{
	
	if (enNodeType == ANIBIKE_DL_MASTER)
	{				
		// Set pull-down and wired-or so that there will be no problems
		PORT_ConfigurePins( &DATALINK_PORT,
							DATALINK_CLK_PIN,
							false,
							false,
							PORT_OPC_WIREDORPULL_gc,
							PORT_ISC_LEVEL_gc );
							
		PORT_ConfigurePins( &DATALINK_PORT,
							DATALINK_DATA_PIN,
							false,
							false,
							PORT_OPC_WIREDORPULL_gc,
							PORT_ISC_INPUT_DISABLE_gc );
		
		/* Initialize SPI master on port DATAFLASH_PORT. */
		SPI_MasterInit(&spiMasterC,
	               &DATALINK_SPI,
	               &DATALINK_PORT,
	               false,
	               SPI_MODE_1_gc,
	               SPI_INTLVL_OFF_gc,
	               false,
	               SPI_PRESCALER_DIV4_gc,0);
				   
		// Disable the spi interface for now
		SPI_MasterEnable (&spiMasterC, 0);
		
		// Set directions - data out, clk in
		DATALINK_PORT.DIRSET = DATALINK_DATA_PIN;
		DATALINK_PORT.DIRCLR = DATALINK_CLK_PIN;
		
		// drive data to high
		DATALINK_PORT.OUTSET = DATALINK_DATA_PIN;		

	}
	else
	{
		// map PORT C to virtual port 1
		PORT_MapVirtualPort1( PORTCFG_VP1MAP_PORTC_gc );
		
		// set clk out and data in
		DATALINK_PORT.DIRCLR = DATALINK_DATA_PIN;
		DATALINK_PORT.DIRSET = DATALINK_CLK_PIN;
		
		// drive clk to high
		DATALINK_PORT.OUTSET = DATALINK_CLK_PIN;
		
		// Set pull-down and wired-or so that there will be no problems
		// Setup interrupt for the data pin
		PORT_ConfigurePins( &DATALINK_PORT,
							DATALINK_CLK_PIN,
							false,
							false,
							PORT_OPC_WIREDORPULL_gc,
							PORT_ISC_BOTHEDGES_gc );
							
		PORT_ConfigurePins( &DATALINK_PORT,
							DATALINK_DATA_PIN,
							false,
							false,
							PORT_OPC_WIREDORPULL_gc,
							PORT_ISC_BOTHEDGES_gc );
		
		PORT_ConfigureInterrupt0( &DATALINK_PORT, PORT_INT0LVL_HI_gc, DATALINK_DATA_PIN );		
		PMIC.CTRL |= PMIC_LOLVLEN_bm|PMIC_MEDLVLEN_bm|PMIC_HILVLEN_bm;		
	}
}


//__________________________________________________________________________________________________
uint8_t anibike_dl_send_data		( uint8_t *aData, uint8_t iLength )
{
	uint8_t i = iLength;
	uint8_t chs = 0;
	uint8_t d;
	uint8_t timeout = ANIBIKE_DL_RX_TIMEOUT;


	// check if there is someone in the other side
	if ((DATALINK_CLK_PIN&(DATALINK_PORT.IN))==0)
	{
		return 1;	// no-one found
	}

	// start transaction by clearing data pin
	DATALINK_PORT.OUTCLR = DATALINK_DATA_PIN;		
	
	// wait for the clk to be low as well
	while (--timeout && (DATALINK_PORT.IN&DATALINK_CLK_PIN))	
	{
		// wait and do nothing
	}
	if (!timeout)
	{
		// the other side doesn't respond
		// end transaction by setting data pin
		DATALINK_PORT.OUTSET = DATALINK_DATA_PIN;
		return 2;	// no response
	}
	
	// here we know that there is someone on SLAVE and it responds

	// Set both lines as totem-pole and activate SPI
	DATALINK_PORT.DIRSET = DATALINK_DATA_PIN|DATALINK_CLK_PIN;
	PORT_ConfigurePins( &DATALINK_PORT,
						DATALINK_CLK_PIN|DATALINK_DATA_PIN,
						false,
						false,
						PORT_OPC_TOTEM_gc,
						PORT_ISC_INPUT_DISABLE_gc );
	SPI_MasterEnable (&spiMasterC, 1);
	
	// the length is not part of the checksum
	anibike_dl_send_byte (spiMasterC, iLength); 
	
	// transfer iLength bytes
	do 
	{
		d = *aData++;
		
		spiMasterC.module->DATA = d;
		/* Wait for transmission complete. */
		while(!(spiMasterC.module->STATUS & SPI_IF_bm)) {}
			
		chs += d;
	} while (--i);
	
	// transfer checksum
	anibike_dl_send_byte (spiMasterC, chs);
	
	// disable spi and set both lines as usual - pulled low wired-or
	SPI_MasterEnable (&spiMasterC, 0);
	PORT_ConfigurePins( &DATALINK_PORT,
						DATALINK_CLK_PIN,
						false,
						false,
						PORT_OPC_WIREDORPULL_gc,
						PORT_ISC_LEVEL_gc );
							
	PORT_ConfigurePins( &DATALINK_PORT,
						DATALINK_DATA_PIN,
						false,
						false,
						PORT_OPC_WIREDORPULL_gc,
						PORT_ISC_INPUT_DISABLE_gc );	
	SPI_MasterEnable (&spiMasterC, 0);
	DATALINK_PORT.DIRSET = DATALINK_DATA_PIN;
	DATALINK_PORT.DIRCLR = DATALINK_CLK_PIN;
	
	// set data as low (waiting for ACK)
	DATALINK_PORT.OUTCLR = DATALINK_DATA_PIN;
	
	// wait for ACK
	timeout = ANIBIKE_DL_RX_TIMEOUT;
	// wait for the clk to be high as well (ACK)
	while (--timeout && !(DATALINK_PORT.IN&DATALINK_CLK_PIN))	
	{
		// wait and do nothing
	}
	if (!timeout)
	{
		// the other side didn't send ACK (clock high)
		// end transaction by setting data pin
		DATALINK_PORT.OUTSET = DATALINK_DATA_PIN;
		return 3;	// no ACK
	}
	
	// if there was a response
	if (DATALINK_PORT.IN&DATALINK_CLK_PIN)
	{
		// end transaction by setting data pin
		DATALINK_PORT.OUTSET = DATALINK_DATA_PIN;
	}
	
	return 0;
}

//__________________________________________________________________________________________________
uint8_t anibike_dl_receive_byte ( void )
{
	asm volatile (
					"			push	r19				\n\t"		// [1CC]
					"			in		r19, 0x3f		\n\t"		// [1CC] store the status register
					"			push	r19				\n\t"		// [1CC]
					"			clr		r24				\n\t"		// [1CC] clear r3, 1cc
					"			ldi		r19, 0xff		\n\t"		// [1CC]
					"			clc						\n\t"		// [1CC] clear carry (c=0), 1cc
					
					"waitclk0%=:dec		r19				\n\t"
					"			sbis	%1,%2			\n\t"		// wait for clk (bit7) to be set in DATALINK_PORT, 2cc without skip, 3cc with skip
					"			brne	waitclk0%=		\n\t"		// we know that carry is cleared, 1cc no jump, 2cc with jump
					"			sbic	%1,%0			\n\t"		// check data line now
					"			sec						\n\t"		// if data line
					"			rol		r24				\n\t"		// push carry and pull c=0	
					"			ldi		r19, 0x8		\n\t"					
							
					"waitclk1%=:dec		r19				\n\t"		// 
					"			sbis	%1,%2			\n\t"		// wait for clk (bit7) to be set in DATALINK_PORT, 2cc without skip, 3cc with skip
					"			brne	waitclk1%=		\n\t"		// we know that carry is cleared, 1cc no jump, 2cc with jump
					//"			clc						\n\t"		// clear carry (c=0), 1cc
					"			sbic	%1,%0			\n\t"		// check data line now
					"			sec						\n\t"		// if data line
					"			rol		r24				\n\t"		// push carry and pull c=0	
					"			ldi		r19, 0x8		\n\t"
					
					"waitclk2%=:dec		r19				\n\t"		// 
					"			sbis	%1,%2			\n\t"		// wait for clk (bit7) to be set in DATALINK_PORT, 2cc without skip, 3cc with skip
					"			brne	waitclk2%=		\n\t"		// we know that carry is cleared, 1cc no jump, 2cc with jump
					//"			clc						\n\t"		// clear carry (c=0), 1cc
					"			sbic	%1,%0			\n\t"		// check data line now
					"			sec						\n\t"		// if data line
					"			rol		r24				\n\t"		// push carry and pull c=0	
					"			ldi		r19, 0x8		\n\t"
					
					"waitclk3%=:dec		r19				\n\t"		// 
					"			sbis	%1,%2			\n\t"		// wait for clk (bit7) to be set in DATALINK_PORT, 2cc without skip, 3cc with skip
					"			brne	waitclk3%=		\n\t"		// we know that carry is cleared, 1cc no jump, 2cc with jump
					//"			clc						\n\t"		// clear carry (c=0), 1cc
					"			sbic	%1,%0			\n\t"		// check data line now
					"			sec						\n\t"		// if data line
					"			rol		r24				\n\t"		// push carry and pull c=0	
					"			ldi		r19, 0x8		\n\t"
					
					"waitclk4%=:dec		r19				\n\t"		// 
					"			sbis	%1,%2			\n\t"		// wait for clk (bit7) to be set in DATALINK_PORT, 2cc without skip, 3cc with skip
					"			brne	waitclk4%=		\n\t"		// we know that carry is cleared, 1cc no jump, 2cc with jump
					//"			clc						\n\t"		// clear carry (c=0), 1cc
					"			sbic	%1,%0			\n\t"		// check data line now
					"			sec						\n\t"		// if data line
					"			rol		r24				\n\t"		// push carry and pull c=0	
					"			ldi		r19, 0x8		\n\t"
					
					"waitclk5%=:dec		r19				\n\t"		// 
					"			sbis	%1,%2			\n\t"		// wait for clk (bit7) to be set in DATALINK_PORT, 2cc without skip, 3cc with skip
					"			brne	waitclk5%=		\n\t"		// we know that carry is cleared, 1cc no jump, 2cc with jump
					//"			clc						\n\t"		// clear carry (c=0), 1cc
					"			sbic	%1,%0			\n\t"		// check data line now
					"			sec						\n\t"		// if data line
					"			rol		r24				\n\t"		// push carry and pull c=0	
					"			ldi		r19, 0x8		\n\t"
					
					"waitclk6%=:dec		r19				\n\t"		// 
					"			sbis	%1,%2			\n\t"		// wait for clk (bit7) to be set in DATALINK_PORT, 2cc without skip, 3cc with skip
					"			brne	waitclk6%=		\n\t"		// we know that carry is cleared, 1cc no jump, 2cc with jump
					//"			clc						\n\t"		// clear carry (c=0), 1cc
					"			sbic	%1,%0			\n\t"		// check data line now
					"			sec						\n\t"		// if data line
					"			rol		r24				\n\t"		// push carry and pull c=0	
					"			ldi		r19, 0x8		\n\t"
					
					"waitclk7%=:dec		r19				\n\t"		// 
					"			sbis	%1,%2			\n\t"		// wait for clk (bit7) to be set in DATALINK_PORT, 2cc without skip, 3cc with skip
					"			brne	waitclk7%=		\n\t"		// we know that carry is cleared, 1cc no jump, 2cc with jump
					//"			clc						\n\t"		// clear carry (c=0), 1cc
					"			sbic	%1,%0			\n\t"		// check data line now
					"			sec						\n\t"		// if data line
					"			rol		r24				\n\t"		// push carry and pull c=0	
					
					"			pop		r19				\n\t"
					"			out		0x3f, r19		\n\t"		// recall the status register
					"			pop		r19				\n\t"
					"			ret						\n\t"		// return stuff		
					:: "I" (DATALINK_DATA_PINN), "I" (DATALINK_PORT_IN), "I" (DATALINK_CLK_PINN)
	);				
}

//__________________________________________________________________________________________________
void anibike_dl_receive_data	( void )
{
	uint8_t len, cnt;
	uint8_t *b = rxBuffer;
	uint8_t chs = 0, chs_here = 0;
	uint8_t timeout = ANIBIKE_DL_RX_TIMEOUT;
		
	// set direction of the clk pin as input. hopefully will be pulled-low
	DATALINK_PORT.OUTCLR = DATALINK_CLK_PIN;
	DATALINK_PORT.DIRCLR = DATALINK_CLK_PIN;
	
	PORT_ConfigurePins( &DATALINK_PORT,
							DATALINK_CLK_PIN,
							false,
							false,
							PORT_OPC_TOTEM_gc,
							PORT_ISC_BOTHEDGES_gc );
							
	PORT_ConfigurePins( &DATALINK_PORT,
							DATALINK_DATA_PIN,
							false,
							false,
							PORT_OPC_TOTEM_gc,
							PORT_ISC_BOTHEDGES_gc );
		
	// convert both lines to input totem pole - maybe its not needed
	
	// start waiting for data length
	cnt = len = anibike_dl_receive_byte ( );
//	printf_P( PSTR("length: %d"), rxLength);

	cnt = len &= 0x7f;
	
/*	if (len>128)
	{
		len=128;
		cnt=128;
	}*/
	
	// read out the data
	while (cnt--)
		*b++ = anibike_dl_receive_byte ( );
	
	// read out checksum
	chs = anibike_dl_receive_byte ( );
	
	//printf_P( PSTR("length: %d; data: %s; chs: %d"), len, rxBuffer, chs);

	PORT_ConfigurePins( &DATALINK_PORT,
							DATALINK_CLK_PIN,
							false,
							false,
							PORT_OPC_WIREDORPULL_gc,
							PORT_ISC_BOTHEDGES_gc );
	
	// reset the pin directions
	DATALINK_PORT.OUTCLR = DATALINK_CLK_PIN;
	DATALINK_PORT.DIRSET = DATALINK_CLK_PIN;
							

	// calculate checksum
	b = rxBuffer;
	rxLength = cnt = len;
	while (cnt--)
		chs_here += *b++;
		
	//printf_P( PSTR(" chs_here: %d"), chs_here);

	// if ACK then set CLK high and return
	if ( chs_here == chs )
	{
		DATALINK_PORT.OUTSET = DATALINK_CLK_PIN;
		printf_P( PSTR("ACK"));
		return 0;	
	}

	// wait for the data to be high (after ACK/NACK)
	while (!(DATALINK_PORT.IN&DATALINK_DATA_PIN))	
	{
		printf_P( PSTR("W"));
		// wait and do nothing
	}

	// set clock high
	DATALINK_PORT.OUTSET = DATALINK_CLK_PIN;	
		
	PORT_ConfigurePins( &DATALINK_PORT,
							DATALINK_DATA_PIN,
							false,
							false,
							PORT_OPC_WIREDORPULL_gc,
							PORT_ISC_BOTHEDGES_gc );
						
	rxBuffer[rxLength]='\0';	
	printf_P( PSTR("data: %s"), rxBuffer);
}

//__________________________________________________________________________________________________
ISR(PORTC_INT0_vect,  ISR_BLOCK)
{
	// Data was cleared
	if ( (DATALINK_PORT.IN&DATALINK_DATA_PIN)!=0 )
		return;

	cli ();
	rxLength = 0;
	anibike_dl_receive_data ( );
	reDataReady = 1;
	sei ( );
}


