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
volatile uint8_t*		g_current_flash_buffer = NULL;
volatile uint8_t*		g_current_proj_buffer = NULL;
volatile uint8_t		g_flash_data_valid = 0;
volatile uint8_t		g_current_dl_finished = 0;
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
	ELAPSED_ANGLE = 75;
	
	// set the projection buffer
	g_current_flash_addr = 0;
	g_current_polarity = 0;
	g_current_proj_buffer = g_flash_read_buffer_I;
	g_current_flash_buffer = g_flash_read_buffer_II;
	g_current_data_counter = 0;
	g_flash_data_valid = 0;
	g_current_dl_finished = 0;
	CURRENT_ANGLE = 0;
}

void anibike_master_initialize_software ( void )
{
	uint8_t o = 0;
	
	for (uint8_t i=(0+o); i<(16+o); i++)
	{
		uint8_t k = i%16;
		uint8_t v = 4+(k/2);
		// blue l1
		g_current_proj_buffer[k] = 0;//(i)|((i)<<4);
		
		// green l1
		g_current_proj_buffer[16+k] = (v)|((v)<<4);
			
		// red l1
		g_current_proj_buffer[32+k] = 0;//(i)|((i)<<4);
		
		// blue l2
		g_current_proj_buffer[48+k] = 0;
		
		// green l2
		g_current_proj_buffer[64+k] = 0;
		
		// red l2
		g_current_proj_buffer[80+k] = 0;
	}
	set_projection_buffer ( g_current_proj_buffer );
	
	
	// read the first entry from the filesystem	
	if (FS_ReadNextEntry ( &g_currentEntry )==1)	// means that the filesystem is empty
	{
		printf_P(PSTR("anibike 2v8bt: %s, %d, %d\r\n"), g_currentEntry.sFileName, g_currentEntry.iNumFrames, g_currentEntry.iBlockList[0]);		
		g_current_flash_addr = ((uint32_t)(g_currentEntry.iBlockList[0]))*FS_FRAME_SIZE;
		g_currentFrameInFile = 0; 
		MUX_ENABLE;
		run_row_control;		
	}
	else
	{
		printf_P(PSTR("anibike 2v8bt: file-system empty."));
		MUX_DISABLE;
		stop_row_control;
	}
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
	stop_row_control;
	dataflash_spi_init (  );
	
	swUART_ConfigureDevice ( 0 );
	swUART_SetRxInterruptLevel ( 4 );
	swUART_SetInterruptHandler ( rx_handler );
	swUART_SetBaudRate ( 115200 );
	
	set_hall_interrupt_handler( hall_sensor_handler );
	anibike_dl_master_initialize ( );
	anibike_master_initialize_software ( );
	
	sei ( );
	
	while (1)
	{	
		//if (MUX_IS_DISABLED) continue;
		
		// Read from the flash 96 bytes
		dataflash_read_vector( g_current_flash_addr, 
							   g_current_flash_buffer, 
							   FS_COLUMN_SIZE );
		
		// mark to the sending mechanism to start transactions
		if (g_current_polarity==0)
		{
			g_current_data_counter = 0;
			g_flash_data_valid = 1;		
		
		
			while (g_current_data_counter<48)
			{
				anibike_dl_master_send_data( g_current_flash_buffer + g_current_data_counter, 3);
				g_current_data_counter+=3;
			}	
		}
		else
		{
			g_current_data_counter = 48;
			g_flash_data_valid = 1;		
		
		
			while (g_current_data_counter<96)
			{
				anibike_dl_master_send_data( g_current_flash_buffer + g_current_data_counter, 3);
				g_current_data_counter+=3;
			}				
		}	
		
		anibike_dl_master_end_transactions;		
													  
		// idle until buffer not valid anymore
		while (ELAPSED_ANGLE) {	 }
				
		switch_angle_signal ( );
	}
}


/*****************************************************************
 *			I N T E R R U P T   H A N D L  E R S
 *****************************************************************/
void switch_angle_signal ( void )
{
	g_current_dl_finished = 0;
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
	if (CURRENT_ANGLE==0x80 || CURRENT_ANGLE==0x00)
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
	
	// set the appropriate buffer to the projection system
	set_projection_buffer ( g_current_proj_buffer + g_current_polarity );
	
	// send to all the slaves "start new batch massage"
	anibike_dl_master_send_timing_sync(  );

	g_flash_data_valid = 0;
	
	ELAPSED_ANGLE = 75;
}

/*****************************************************************
 *			HALL SENSOR HANDLE
 *****************************************************************/
void hall_sensor_handler ( void )
{
	//printf_P ( PSTR("Hall Sensor\r\n"));
	
	// init the current angle to zero
//	CURRENT_ANGLE = 0;
	
	// re-init other flags and variables
//	g_current_data_counter = 0;
//	CLR_DL_SEND_FINISHED;
//	CLR_FLASH_DATA_VALID;
	
	// if finished current file, read the next entry to g_currentEntry;
	// and set g_currentFrameInFile to 0
	
	
	// if didn't finish yet, switch to the next frame
	
}