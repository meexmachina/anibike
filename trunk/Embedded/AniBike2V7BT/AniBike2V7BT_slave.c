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
extern volatile uint8_t g_needed_flush;
extern volatile	uint8_t	g_data_valid;

/*****************************************************************
 *			GENERAL SYSTEM CONFIGURATION
 *****************************************************************/
void anibike_slave_initialize_hardware ( void )
{
	// Map port A to virtual port 3
	PORT_MapVirtualPort3( PORTCFG_VP3MAP_PORTA_gc );	
}

/*****************************************************************
 *			M A I N    F U N C T I O N 
 *****************************************************************/
int main(void)
{
	//_delay_ms(200);
	SetClockFreq ( 32 );
	anibike_slave_initialize_hardware (  );
	
	swUART_ConfigureDevice ( 0 );
	swUART_SetRxInterruptLevel ( 1 );
	swUART_SetInterruptHandler ( NULL );
	swUART_SetBaudRate ( 115200 );
	initialize_lighting_system(  );	
	anibike_dl_slave_initialize (  );
	
	// enable interrupts
	sei ( );
	
	while (1)
	{
		anibike_dl_slave_handle_data (  );
		
		if (g_needed_flush)
		{
			g_needed_flush = 0;
			DL_SLAVE_CIRC_BUFFER_FLUSH;
			g_data_valid=0;
		}
	}
}
