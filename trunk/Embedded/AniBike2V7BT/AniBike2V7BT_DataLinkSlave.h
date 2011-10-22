/*
 * AniBike2V7BT_DataLinkSlave.h
 *
 * Created: 10/22/2011 2:17:00 PM
 *  Author: David
 */ 


#ifndef ANIBIKE2V7BT_DATALINKSLAVE_H_
#define ANIBIKE2V7BT_DATALINKSLAVE_H_

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
typedef uint8_t anibike_dl_slave_header;

void anibike_dl_slave_initialize ( void );



#endif /* ANIBIKE2V7BT_DATALINKSLAVE_H_ */