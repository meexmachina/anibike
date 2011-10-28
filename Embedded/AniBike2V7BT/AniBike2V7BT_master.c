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
extern	 uint8_t		g_cpu_speed;
volatile uint8_t		g_flash_read_buffer_I	[96] = {0};
volatile uint8_t		g_flash_read_buffer_II	[96] = {0};
volatile uint8_t*		g_current_flash_buffer;
volatile uint8_t*		g_current_proj_buffer;
volatile uint32_t		g_current_flash_addr = 0;
volatile FileEntry_ST	g_currentEntry;
volatile uint8_t		g_currentFrameInFile = 0;
volatile uint8_t		g_currentDuration = 255;
volatile uint8_t		g_current_polarity = 0;
volatile uint8_t		g_current_data_counter = 0;

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
	
	
	// setup column interchanging timer
//	COLUMN_TIMER_CTRL.CTRLA |= TC1_CCBEN_bm;								// enable timer B
//	COLUMN_TIMER_CTRL.CCB = 0x2710;										// init to 600 usec
//	TC1_SetCCBIntLevel(&COLUMN_TIMER_CTRL, TC_CCBINTLVL_LO_gc );
	
	
	// set the projection buffer
	g_current_flash_addr = 0;
	g_current_polarity = 0;
	g_current_proj_buffer = g_flash_read_buffer_I;
	g_current_flash_buffer = g_flash_read_buffer_II;
	g_current_data_counter = 0;
	CLR_FLASH_DATA_VALID;
	CLR_DL_SEND_FINISHED;
	CURRENT_ANGLE = 0;
}

/*****************************************************************
 *			M A I N    F U N C T I O N 
 *****************************************************************/
int main(void)
{
	SetClockFreq ( 32 );
	anibike_master_initialize_hardware( );
	
	initialize_hall_sensor(  );
	initialize_lighting_system(  );	
	run_row_control;
	dataflash_spi_init (  );
	
	swUART_ConfigureDevice ( 0 );
	swUART_SetRxInterruptLevel ( 4 );
	swUART_SetInterruptHandler ( rx_handler );
	swUART_SetBaudRate ( 115200 );
	
	set_hall_interrupt_handler( hall_sensor_handler );
	anibike_dl_master_initialize ( );
	
	sei ( );

	while (1)
	{	
		
		// set the appropriate buffer to the projection system
		set_projection_buffer ( g_current_proj_buffer + g_current_polarity );
		
		// Read from the flash 96 bytes
		dataflash_read_vector( g_current_flash_addr, 
							   g_current_flash_buffer, 
							   FS_COLUMN_SIZE );
		
		// mark to the sending mechanism to start transactions
		g_current_data_counter = 0;
		SET_FLASH_DATA_VALID;		
		
		while (g_current_data_counter<48)
		{
			anibike_dl_master_send_data( g_current_flash_buffer + g_current_data_counter, 3);
			g_current_data_counter+=3;
		}			
										  
		// idle until buffer not valid anymore
		while (FLASH_DATA_IS_VALID) {}
	}
}


/*****************************************************************
 *			I N T E R R U P T   H A N D L  E R S
 *****************************************************************/
ISR(TCC1_CCB_vect)
{
	COLUMN_TIMER_CTRL.CNT = 0;
	g_current_data_counter = 0;
	CLR_DL_SEND_FINISHED;
	
	// update the next angle
	CURRENT_ANGLE += 1;				// if its 255 it will wrap around to 0 
	
	// update the next polarity if needed
	if (CURRENT_ANGLE&0x80)
	{
		g_current_polarity = 48;
	}
	else
	{
		g_current_polarity = 0;
	}
	
	// update the next flash address
	if (CURRENT_ANGLE==0x80)
	{
		g_current_flash_addr = FRAME_OFFSET_ADDR(g_currentEntry.iBlockList[g_currentFrameInFile]);
	}		
	else
	{
		g_current_flash_addr += FS_COLUMN_SIZE;
	}		
			
	// switch between flash read buffers
	if (g_current_flash_buffer ==  g_flash_read_buffer_I)
	{
		g_current_flash_buffer = g_flash_read_buffer_II;
		g_current_proj_buffer = g_flash_read_buffer_I;
	}
	else
	{
		g_current_flash_buffer = g_flash_read_buffer_I;
		g_current_proj_buffer = g_flash_read_buffer_II;		
	}
	
	set_projection_buffer ( g_current_proj_buffer );
	
	// send to all the slaves "start new batch massage"
	anibike_dl_master_send_timing_sync( 600 );
	anibike_dl_master_send_batch_start(  );

	CLR_FLASH_DATA_VALID;
}

/*****************************************************************
 *			HALL SENSOR HANDLE
 *****************************************************************/
void hall_sensor_handler ( void )
{
	//printf_P ( PSTR("Hall Sensor\r\n"));
	
	// init the current angle to zero
	CURRENT_ANGLE = 0;
	
	// re-init other flags and variables
	g_current_data_counter = 0;
	CLR_DL_SEND_FINISHED;
	CLR_FLASH_DATA_VALID;
	
	// if finished current file, read the next entry to g_currentEntry;
	// and set g_currentFrameInFile to 0
	
	
	// if didn't finish yet, switch to the next frame
	
}