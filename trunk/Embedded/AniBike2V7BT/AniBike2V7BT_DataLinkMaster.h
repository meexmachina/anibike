/*
 * AniBike2V7BT_DataLinkMaster.h
 *
 * Created: 10/22/2011 10:01:05 PM
 *  Author: David
 */ 


#ifndef ANIBIKE2V7BT_DATALINKMASTER_H_
#define ANIBIKE2V7BT_DATALINKMASTER_H_

/*****************************************
 * header is define by 8 bits
 * bit 7-5: opcode (0x00:0x07)
 *		0x00 - new timing sync data
 *		0x01 - restart data batch
 *		0x02 - continue current batch
 *		0x03 - light LEDs debug
 *		0x04 - set address
 *		0x05 - go to sleep
 *		0x06 - reserved
 *		0x07 - reserved
 * bit 4-0: length (0-47)
 */
enum
{
	DL_NEW_TIMING_SYNC			= 0x00,
	DL_RESERVED1				= 0x20,
	DL_CONTINUE_DATA_BATCH		= 0x40,
	DL_LIGHT_LEDS_DEBUG			= 0x60,
	DL_SET_ADDRESS				= 0x80,
	DL_GO_TO_SLEEP				= 0xA0,
	DL_SET_CAL_VALUES			= 0xC0,
	DL_RESERVED2				= 0xE0
};

typedef uint8_t anibike_dl_slave_header;

void anibike_dl_master_initialize ( void );
int anibike_dl_master_check_connection ( void );
void anibike_dl_master_send_data ( uint8_t *data, uint8_t length );	
void anibike_dl_master_send_light_leds_debug ( uint8_t row, uint8_t rgb, uint8_t val );
void anibike_dl_master_send_timing_sync ( void );
void anibike_dl_master_send_address ( uint8_t address );
void anibike_dl_master_go_to_sleep ( void );
void anibike_dl_master_set_cal_values ( uint16_t r, uint16_t g, uint16_t b );


#endif /* ANIBIKE2V7BT_DATALINKMASTER_H_ */