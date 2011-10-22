/*
 * AniBike2V7BT_DataLinkMaster.h
 *
 * Created: 10/22/2011 10:01:05 PM
 *  Author: David
 */ 


#ifndef ANIBIKE2V7BT_DATALINKMASTER_H_
#define ANIBIKE2V7BT_DATALINKMASTER_H_


void anibike_dl_master_initialize ( void );
int anibike_dl_master_check_connection ( void );
void anibike_dl_master_send_data ( uint8_t *data, uint8_t length );	


#endif /* ANIBIKE2V7BT_DATALINKMASTER_H_ */