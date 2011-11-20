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
volatile uint8_t		g_delta_angle = 255;

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
	ELAPSED_ANGLE = g_delta_angle;
	
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

/*****************************************************************
 *			SETUP REAL-TIME-COUNTER FOR HALL SENSOR
 *****************************************************************/
void anibike_master_setup_realtime_counter ( void )
{
	/* Turn on internal 32kHz. */
	OSC.CTRL |= OSC_RC32KEN_bm;

	do {
		/* Wait for the 32kHz oscillator to stabilize. */
	} while ( ( OSC.STATUS & OSC_RC32KRDY_bm ) == 0);
	
	/* Set internal 32kHz oscillator as clock source for RTC. */
	CLK.RTCCTRL = CLK_RTCSRC_RCOSC_gc | CLK_RTCEN_bm;
	
	do {
		/* Wait until RTC is not busy. */
	} while ( RTC_Busy() );
	
	// the distance between ticks is going to be 1 msec if there was no hall sensor for 2 seconds
	// the system will shut down the main current consumers
	RTC.PER = 1000;			// 1 msec
	RTC.CNT = 0;
	RTC.COMP = 1000;		// 1 msec
	RTC.CTRL = ( RTC.CTRL & ~RTC_PRESCALER_gm ) | RTC_PRESCALER_DIV1_gc;	
	RTC_SetCompareIntLevel( RTC_COMPINTLVL_LO_gc  );
}

/*****************************************************************
 *			GENERAL SOFTWARE CONFIGURATION
 *****************************************************************/
void anibike_master_initialize_software ( void )
{
	uint8_t o = 0;
	
	for (uint8_t i=(0+o); i<(16+o); i++)
	{
		uint8_t k = i%16;
		uint8_t v = 4+(k/2);
		g_current_proj_buffer[   k] = 0;				// blue l1
		g_current_proj_buffer[16+k] = (v)|((v)<<4);		// green l1
		g_current_proj_buffer[32+k] = 0;				// red l1	
		g_current_proj_buffer[48+k] = 0;				// blue l2
		g_current_proj_buffer[64+k] = 0;				// green l2
		g_current_proj_buffer[80+k] = 0;				// red l2
	}
	set_projection_buffer ( g_current_proj_buffer );
	
	
	// read the first entry from the filesystem	
	if (FS_ReadNextEntry ( &g_currentEntry )==1)	// means that the filesystem is empty
	{
		//printf_P(PSTR("anibike 2v8bt: %s, %d, %d\r\n"), g_currentEntry.sFileName, g_currentEntry.iNumFrames, g_currentEntry.iBlockList[0]);		
		g_current_flash_addr = ((uint32_t)(g_currentEntry.iBlockList[0]))*FS_FRAME_SIZE;
		g_currentFrameInFile = 0; 
		g_currentDuration = g_currentEntry.iDuration;
		MUX_ENABLE;
		run_row_control;		
	}
	else
	{
		//printf_P(PSTR("anibike 2v8bt: file-system empty."));
		MUX_DISABLE;
		stop_row_control;
	}
}

/*****************************************************************
 *			M A I N    F U N C T I O N 
 *****************************************************************/
int main(void)
{
	// Sleep a little bit until the power supply is stabilized
	_delay_ms(100);
	
	SetClockFreq ( 32 );
	anibike_master_setup_realtime_counter (  );
	anibike_master_initialize_hardware(  );
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
		if (g_current_polarity==1)
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
	anibike_dl_master_start_transactions	
	
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
	
	ELAPSED_ANGLE = g_delta_angle;
}

/*****************************************************************
 *			HALL SENSOR HANDLE
 *****************************************************************/
void hall_sensor_handler ( void )
{
	uint16_t temp;
	anibike_dl_master_end_transactions
	
	//printf_P ( PSTR("H"));
	if ((RTC.CNT)<100) 
	{
		stop_row_control;
		MUX_DISABLE;
		anibike_dl_master_go_to_sleep (  );
		return;
	}		
	
	// init the current angle to zero
	CURRENT_ANGLE = 0;
	
	// re-init other flags and variables
	g_current_data_counter = 0;
	g_flash_data_valid = 0;
	g_current_dl_finished = 0;
	
	anibike_dl_master_start_transactions	
	
	// calculate the new delta-angle time
	// the rtc ticks every 1 ms. row changes every 16 usec.
	// so given T msec, multiply by 1000/255 will give number of usec for single rotation
	// then divide by 16 (row change time).
	// so given T msec we multiply with 0.2437=3.9/16. which is 0.2437~1/4-1/128
	temp = (RTC.CNT);
	//printf_P ( PSTR("%d"), temp);
	g_delta_angle = (uint8_t)(((temp/4)-(temp/128))&0xff);
	ELAPSED_ANGLE = g_delta_angle;
	RTC.CNT = 0;						// re-init real-time counter
	//printf_P ( PSTR(",%d,"), g_delta_angle);
	
	
	
	if (g_currentDuration == 0)
	{
		// if finished current file, read the next entry to g_currentEntry;
		if (FS_ReadNextEntry ( &g_currentEntry )==1)
		{
			g_current_flash_addr = ((uint32_t)(g_currentEntry.iBlockList[0]))*FS_FRAME_SIZE;
			g_currentDuration = g_currentEntry.iDuration;
			g_currentFrameInFile = 0;
			g_current_polarity = 0;
			g_current_flash_buffer = g_flash_read_buffer_I;
			g_current_proj_buffer = g_flash_read_buffer_II;	
			set_projection_buffer ( g_current_proj_buffer );
			anibike_dl_master_send_timing_sync(  );
		}		
		else
		{
			stop_row_control;
			MUX_DISABLE;
			anibike_dl_master_go_to_sleep (  );
		}
	}
	else
	{
		// if didn't finish yet, switch to the next frame
		g_currentFrameInFile = (g_currentFrameInFile+1)%g_currentEntry.iNumFrames;
		g_current_flash_addr = ((uint32_t)(g_currentEntry.iBlockList[g_currentFrameInFile]))*FS_FRAME_SIZE;
		g_currentDuration --;		
		g_current_flash_buffer = g_flash_read_buffer_I;
		g_current_proj_buffer = g_flash_read_buffer_II;		
		g_current_polarity = 0;
		set_projection_buffer ( g_current_proj_buffer );
		anibike_dl_master_send_timing_sync(  );
	}
	
	MUX_ENABLE;
	run_row_control;		
}

/*****************************************************************
 *			LONG IDLE INTERRUPT
 *****************************************************************/
ISR(RTC_COMP_vect)
{
	// stop all projection
	stop_row_control;
	MUX_DISABLE;
		
	// send the secondary boards to stop all projection
	anibike_dl_master_go_to_sleep (  );
}