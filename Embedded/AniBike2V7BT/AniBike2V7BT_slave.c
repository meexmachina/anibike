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
//volatile uint8_t g_flash_read_buffer_I	[48] = {0};
//volatile uint8_t g_flash_read_buffer_II	[48] = {0};
//volatile uint8_t *g_current_flash_buffer = NULL;


/*****************************************************************
 *			GENERAL SYSTEM CONFIGURATION
 *****************************************************************/
void anibike_master_initialize_hardware ( void )
{
	// Map port A to virtual port 3
	PORT_MapVirtualPort3( PORTCFG_VP3MAP_PORTA_gc );	
}

/*****************************************************************
 *			M A I N    F U N C T I O N 
 *****************************************************************/
int main(void)
{
	SetClockFreq ( 32 );
	
	swUART_ConfigureDevice ( 0 );
	swUART_SetRxInterruptLevel ( 3 );
	swUART_SetInterruptHandler ( NULL );
	
	initialize_lighting_system(  );	
	stop_row_control;
	anibike_dl_initialize ( ANIBIKE_DL_SLAVE );
	anibike_hlcomm_setup_rx_system (  );
	
	sei ( );
	
	while (1)
	{
		anibike_hlcomm_handle_data (  );
	}
}
