/*
 * AniBike2V7BT_HighLevelComm.c
 *
 * Created: 10/17/2011 12:57:45 AM
 *  Author: David
 */ 

#include "AniBike2V7BT_Internal.h"

//__________________________________________________________________________________________________
void anibike_dl_send_cal_data ( uint16_t red, uint16_t green, uint16_t blue )
{
	
}

//__________________________________________________________________________________________________
void anibike_dl_light_led_req ( uint8_t row, uint8_t rgb_choose, uint16_t val )
{
	
}

//__________________________________________________________________________________________________
void anibike_hlcomm_setup_rx_system ( void )
{
		
}

//__________________________________________________________________________________________________
void anibike_dl_handle_data ( void )
{
	if ( !rxDataReady ) return;
	
	rxDataReady = 0;	
}