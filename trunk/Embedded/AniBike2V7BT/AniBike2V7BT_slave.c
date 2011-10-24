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
volatile uint8_t g_flash_read_buffer_I	[48] = {0};
volatile uint8_t g_flash_read_buffer_II	[48] = {0};
volatile uint8_t *g_receive_buffer = NULL;
volatile uint8_t *g_proj_buffer = NULL;


/*****************************************************************
 *			GENERAL SYSTEM CONFIGURATION
 *****************************************************************/
void anibike_slave_initialize_hardware ( void )
{
	// Map port A to virtual port 3
	PORT_MapVirtualPort3( PORTCFG_VP3MAP_PORTA_gc );	
	
	g_receive_buffer = g_flash_read_buffer_II;
	g_proj_buffer = g_flash_read_buffer_I;
	set_projection_buffer ( g_proj_buffer );
	anibike_dl_slave_set_receive_buffer ( g_receive_buffer );
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
	stop_row_control;
	anibike_dl_slave_initialize (  );
	
	sei ( );
	
	while (1)
	{
		anibike_dl_slave_handle_data (  );
	}
}
