/*
 * AniBike2V7BT.c
 *
 * Created: 4/8/2011 9:45:05 PM
 *  Author: David
 */ 
#include "AniBike2V7BT_Internal.h"

/*****************************************************************
 *			BUFFERS
 *****************************************************************/


/*****************************************************************
 *			GENERAL SYSTEM CONFIGURATION
 *****************************************************************/
void anibike_slave_initialize_hardware ( void )
{
	// Map port A to virtual port 3
	PORT_MapVirtualPort3( PORTCFG_VP3MAP_PORTA_gc );	
	
	//	anibike_dl_slave_set_receive_buffer ( g_receive_buffer );

	/////////////////////////////////////
	// RTC
	/* Turn on internal 32kHz. */
	//OSC.CTRL |= OSC_RC32KEN_bm;

	//do {
		/* Wait for the 32kHz oscillator to stabilize. */
	//} while ( ( OSC.STATUS & OSC_RC32KRDY_bm ) == 0);
	
	/* Set internal 32kHz oscillator as clock source for RTC. */
	//CLK.RTCCTRL = CLK_RTCSRC_RCOSC_gc | CLK_RTCEN_bm;
	
	//do {
		/* Wait until RTC is not busy. */
	//} while ( RTC_Busy() );
	
	//RTC.PER = 2047;
	//RTC.CNT = 0;
	//RTC.COMP = 2047;
	//RTC.CTRL = ( RTC.CTRL & ~RTC_PRESCALER_gm ) | RTC_PRESCALER_DIV1_gc;
}

/*****************************************************************
 *			M A I N    F U N C T I O N 
 *****************************************************************/
int main(void)
{
	SetClockFreq ( 32 );
	anibike_slave_initialize_hardware (  );
	
	swUART_ConfigureDevice ( 0 );
	swUART_SetRxInterruptLevel ( 3 );
	swUART_SetInterruptHandler ( NULL );
	swUART_SetBaudRate ( 115200 );
	initialize_lighting_system(  );	
	anibike_dl_slave_initialize (  );
	run_row_control;
	MUX_ENABLE;
	
	// enable interrupts
	sei ( );
	
	while (1)
	{
		anibike_dl_slave_handle_data (  );
	}
}
