/*
 * AniBike2V7BT_DataLinkMaster.c
 *
 * Created: 10/22/2011 10:00:56 PM
 *  Author: David
 */ 

#include "AniBike2V7BT_Internal.h"

SPI_Master_t		spiMasterC;

//__________________________________________________________________________________________________
void anibike_dl_master_initialize ( void )
{
	// Set pull-down and wired-or so that there will be no problems
	/*PORT_ConfigurePins( &DATALINK_PORT,
						DATALINK_CLK_PIN|DATALINK_DATA_PIN|DATALINK_CS_PIN,
						false,
						false,
						PORT_OPC_TOTEM_gc,
						PORT_ISC_INPUT_DISABLE_gc );
	*/
	DATALINK_PORT.DIRSET = DATALINK_CLK_PIN|DATALINK_DATA_PIN|DATALINK_CS_PIN;
	DATALINK_PORT.OUTSET = DATALINK_CS_PIN;
							
	/* Initialize SPI master on port DATAFLASH_PORT. */
	SPI_MasterInit(&spiMasterC,
	            &DATALINK_SPI,
	            &DATALINK_PORT,
	            false,
	            SPI_MODE_0_gc,
	            SPI_INTLVL_OFF_gc,
	            false,
	            SPI_PRESCALER_DIV4_gc,1);
					
	// drive cs to high
	DATALINK_PORT.OUTSET = DATALINK_CS_PIN;	
}

//__________________________________________________________________________________________________
int anibike_dl_master_check_connection ( void )
{
	
}

//__________________________________________________________________________________________________
void anibike_dl_master_send_data ( uint8_t *data, uint8_t length )		// length smaller then 32
{
	uint8_t header;
	header = DL_CONTINUE_DATA_BATCH | length;
	
	SPI_MasterSSLow (&DATALINK_PORT, DATALINK_CS_PIN);
	SPI_MasterTransceiveByte(&spiMasterC, (uint8_t)(header));
	
	while (length--)
	{
		//NOP;NOP;NOP;
		uint8_t d = *data++;
		SPI_MasterTransceiveByte(&spiMasterC, d);
	}		
	
	SPI_MasterSSHigh (&DATALINK_PORT, DATALINK_CS_PIN);
}

//__________________________________________________________________________________________________
void anibike_dl_master_send_batch_start ( void )
{
	uint8_t header;
	header = DL_RESTART_DATA_BATCH;
	
	SPI_MasterSSLow (&DATALINK_PORT, DATALINK_CS_PIN);
	SPI_MasterTransceiveByte(&spiMasterC, header);
	SPI_MasterSSHigh (&DATALINK_PORT, DATALINK_CS_PIN);
}

//__________________________________________________________________________________________________
void anibike_dl_master_send_light_leds_debug ( uint8_t row, uint8_t rgb, uint8_t val )
{
	uint8_t header;
	header = DL_LIGHT_LEDS_DEBUG|3;
	
	SPI_MasterSSLow (&DATALINK_PORT, DATALINK_CS_PIN);
	
	//printf_P(PSTR("sending %d %d %d %d\r\n"), (uint8_t)(header), row, rgb, val);
	
	SPI_MasterTransceiveByte(&spiMasterC, header);	
	//DELAY120NS
	SPI_MasterTransceiveByte(&spiMasterC, row);	
	//DELAY120NS
	SPI_MasterTransceiveByte(&spiMasterC, rgb);	
	//DELAY120NS
	SPI_MasterTransceiveByte(&spiMasterC, val);
	
	SPI_MasterSSHigh (&DATALINK_PORT, DATALINK_CS_PIN);
}

//__________________________________________________________________________________________________
void anibike_dl_master_send_timing_sync ( void )
{
	uint8_t header;
	header = DL_NEW_TIMING_SYNC;
	
	SPI_MasterSSLow (&DATALINK_PORT, DATALINK_CS_PIN);
	SPI_MasterTransceiveByte(&spiMasterC, header);
	SPI_MasterSSHigh (&DATALINK_PORT, DATALINK_CS_PIN);
}

//__________________________________________________________________________________________________
void anibike_dl_master_send_address ( uint8_t address )
{
	uint8_t header;
	header = DL_SET_ADDRESS|1;
	SPI_MasterSSLow (&DATALINK_PORT, DATALINK_CS_PIN);
	SPI_MasterTransceiveByte(&spiMasterC, header);
	//DELAY120NS
	SPI_MasterTransceiveByte(&spiMasterC, address );		
	SPI_MasterSSHigh (&DATALINK_PORT, DATALINK_CS_PIN);	
}

//__________________________________________________________________________________________________
void anibike_dl_master_go_to_sleep ( void )
{
	uint8_t header;
	header = DL_GO_TO_SLEEP;
	SPI_MasterSSLow (&DATALINK_PORT, DATALINK_CS_PIN);
	SPI_MasterTransceiveByte(&spiMasterC, header);
	SPI_MasterSSHigh (&DATALINK_PORT, DATALINK_CS_PIN);	
}

//__________________________________________________________________________________________________
void anibike_dl_master_set_cal_values ( uint16_t r, uint16_t g, uint16_t b )
{
	uint8_t header;
	header = DL_SET_CAL_VALUES|6;
	SPI_MasterSSLow (&DATALINK_PORT, DATALINK_CS_PIN);
	SPI_MasterTransceiveByte(&spiMasterC, header);
	//DELAY120NS
	SPI_MasterTransceiveByte(&spiMasterC, r&0xFF);			//LSB
	//DELAY120NS
	SPI_MasterTransceiveByte(&spiMasterC, (r>>8)&0xFF);		//MSB
	//DELAY120NS
	SPI_MasterTransceiveByte(&spiMasterC, g&0xFF);			//LSB
	//DELAY120NS
	SPI_MasterTransceiveByte(&spiMasterC, (g>>8)&0xFF);		//MSB
	//DELAY120NS
	SPI_MasterTransceiveByte(&spiMasterC, b&0xFF);			//LSB
	//DELAY120NS
	SPI_MasterTransceiveByte(&spiMasterC, (b>>8)&0xFF);		//MSB
	SPI_MasterSSHigh (&DATALINK_PORT, DATALINK_CS_PIN);		
}