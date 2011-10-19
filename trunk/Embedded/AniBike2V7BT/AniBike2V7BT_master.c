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
volatile uint8_t *g_current_flash_buffer = NULL;

/*****************************************************************
 *			HALL SENSOR HANDLE
 *****************************************************************/
void hall_sensor_handler ( void )
{
	printf_P ( PSTR("Hall Sensor\r\n"));
}

/*****************************************************************
 *			GENERAL SYSTEM CONFIGURATION
 *****************************************************************/
void anibike_master_initialize_hardware ( void )
{
	// Set the bt configuration mode to uart
	PORTA.DIRSET = PIN3_bm;
	PORTA.OUTCLR = PIN3_bm;	
	
	g_current_flash_buffer = g_flash_read_buffer_I;
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
	run_row_control ( 1 );
	dataflash_spi_init (  );
	
	swUART_ConfigureDevice ( 0 );
	swUART_SetRxInterruptLevel ( 3 );
	swUART_SetInterruptHandler ( rx_handler );
	set_hall_interrupt_handler( hall_sensor_handler );
	
	anibike_dl_initialize ( ANIBIKE_DL_MASTER );	
	
	sei ( );

	while (1)
	{
	}
}
