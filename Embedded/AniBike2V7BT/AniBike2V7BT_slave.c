/*
 * AniBike2V7BT.c
 *
 * Created: 4/8/2011 9:45:05 PM
 *  Author: David
 */ 
#include "AniBike2V7BT_Internal.h"


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
	anibike_dl_initialize ( ANIBIKE_DL_SLAVE );
	anibike_hlcomm_setup_rx_system (  );
	
	sei ( );
	
	while (1)
	{
		anibike_hlcomm_handle_data (  );
	}
}