/*
 * AniBike2V7BT.c
 *
 * Created: 4/8/2011 9:45:05 PM
 *  Author: David
 */ 

#include "AniBike2V7BT_Internal.h"

/*****************************************************************
 *			GLOBAL VARIABLES
 *****************************************************************/
volatile uint8_t	g_flash_read_buffer_I	[96] = {0};
volatile uint8_t	g_flash_read_buffer_II	[96] = {0};
volatile uint8_t*	g_current_flash_buffer;
volatile uint8_t*	g_current_proj_buffer;
volatile uint32_t	g_current_flash_addr = 0;
volatile uint8_t	g_current_polarity = 0;		// can be 0 or 48
volatile uint8_t	g_data_valid = 0;

/*****************************************************************
 *			HALL SENSOR HANDLE
 *****************************************************************/
void hall_sensor_handler ( void )
{
	//printf_P ( PSTR("Hall Sensor\r\n"));
}

/*****************************************************************
 *			GENERAL SYSTEM CONFIGURATION
 *****************************************************************/
void anibike_master_initialize_hardware ( void )
{
	// Set the bt configuration mode to uart
	PORTA.DIRSET = PIN3_bm;
	PORTA.OUTCLR = PIN3_bm;	
	
	// Map port A to virtual port 3
	PORT_MapVirtualPort3( PORTCFG_VP3MAP_PORTA_gc );
	
	// set the projection buffer
	g_current_flash_addr = 0;
	g_current_polarity = 0;
	g_current_proj_buffer = g_flash_read_buffer_I;
	g_current_flash_buffer = g_flash_read_buffer_II;
	g_data_valid = 0;
}

/*****************************************************************
 *			M A I N    F U N C T I O N 
 *****************************************************************/
int main(void)
{
	SetClockFreq ( 16 );
	anibike_master_initialize_hardware( );
	
	initialize_hall_sensor(  );
	initialize_lighting_system(  );	
	run_row_control;
	dataflash_spi_init (  );
	
	swUART_ConfigureDevice ( 0 );
	swUART_SetRxInterruptLevel ( 3 );
	swUART_SetInterruptHandler ( rx_handler );
	set_hall_interrupt_handler( hall_sensor_handler );
	
	anibike_dl_initialize ( ANIBIKE_DL_MASTER );	
	
	sei ( );

	while (1)
	{
		// set the appropriate buffer to the projection system
		set_projection_buffer ( g_current_proj_buffer + g_current_polarity );
		
		// Read from the flash 96 bytes
		dataflash_read_vector( g_current_flash_addr, 
							   g_current_flash_buffer, 
							   96 );
										  
		// send appropriate 48 bytes to the secondary edge
		if (!g_current_polarity) anibike_hlcomm_send_data ( g_current_flash_buffer + 48 );
		else anibike_hlcomm_send_data ( g_current_flash_buffer );
		
		// idle until buffer not valid anymore
		while (g_data_valid) {}
	}
}
