/*
 * AniBike2V7BT_HighLevelComm.c
 *
 * Created: 10/17/2011 12:57:45 AM
 *  Author: David
 */ 

#include "AniBike2V7BT_Internal.h"

//__________________________________________________________________________________________________
#ifdef _ANIBIKE_MASTER	
int anibike_hlcomm_send_cal_data ( uint16_t red, uint16_t green, uint16_t blue )
{
	anibike_hlcomm_cal_msg msg;
	
	msg.header.dest = 1;
	msg.header.length = sizeof(msg)-sizeof(anibike_hlcomm_header);
	msg.header.opcode = ANIBIKE_HLCOMM_SET_CAL;
	msg.cal_red = red;
	msg.cal_green = green;
	msg.cal_blue = blue;
	
	return anibike_dl_send_data( (uint8_t*)((void*)(&msg)), sizeof(msg) );
}
#endif

//__________________________________________________________________________________________________
#ifdef _ANIBIKE_MASTER	
int anibike_hlcomm_light_led_req ( uint8_t row, uint8_t rgb_choose, uint16_t val )
{
	anibike_hlcomm_light_led_msg msg;
	
	msg.header.dest = 1;
	msg.header.length = sizeof(msg)-sizeof(anibike_hlcomm_header);
	msg.header.opcode = ANIBIKE_HLCOMM_LIGHT_LED;
	msg.row_num = row;
	msg.rgb_choose = rgb_choose;
	msg.val = val;
	
	return anibike_dl_send_data( (uint8_t*)((void*)(&msg)), sizeof(msg) );
}
#endif

//__________________________________________________________________________________________________
#ifdef _ANIBIKE_SLAVE
void anibike_hlcomm_setup_rx_system ( void )
{
		
}
#endif

//__________________________________________________________________________________________________
#ifdef _ANIBIKE_SLAVE
void anibike_hlcomm_handle_data ( void )
{
	if ( !rxDataReady ) return;
	rxDataReady = 0;	
	
	anibike_hlcomm_header *header = (anibike_hlcomm_header*)((void*)(rxBuffer));
	
	switch (header->opcode)
	{
		case (ANIBIKE_HLCOMM_IDN):
				
				break;
		case (ANIBIKE_HLCOMM_SET_DATA):
				
				break;		
		case (ANIBIKE_HLCOMM_SET_CLOCK):
				
				break;
		case (ANIBIKE_HLCOMM_SLEEP):
				
				break;
		case (ANIBIKE_HLCOMM_WAKEUP):
				
				break;
		case (ANIBIKE_HLCOMM_SETUP):
				
				break;
		case (ANIBIKE_HLCOMM_SET_CAL):
				{
					anibike_hlcomm_cal_msg *msg = (anibike_hlcomm_cal_msg *)((void*)(rxBuffer));
					write_period_calibrations ( msg->cal_red, msg->cal_green, msg->cal_blue );
				}				
				break;
		case (ANIBIKE_HLCOMM_LIGHT_LED):
				{
					anibike_hlcomm_light_led_msg *msg = (anibike_hlcomm_light_led_msg *)((void*)(rxBuffer));
					
					if (msg->rgb_choose&0x01)
					{
						set_row_color ( msg->row_num, 1, msg->val);
					}

					if (msg->rgb_choose&0x02)
					{
						set_row_color ( msg->row_num, 2, msg->val);
					}

					if (msg->rgb_choose&0x04)
					{
						set_row_color ( msg->row_num, 3, msg->val);
					}
				}							
				break;
		default:
				break;
	};
}
#endif