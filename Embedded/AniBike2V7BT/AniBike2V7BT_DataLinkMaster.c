/*
 * AniBike2V7BT_DataLinkMaster.c
 *
 * Created: 10/22/2011 10:00:56 PM
 *  Author: David
 */ 

#include "AniBike2V7BT_Internal.h"

SPI_Master_t		spiMasterC;

void anibike_dl_master_initialize ( void )
{
	// Set pull-down and wired-or so that there will be no problems
	PORT_ConfigurePins( &DATALINK_PORT,
						DATALINK_CLK_PIN|DATALINK_DATA_PIN|DATALINK_CS_PIN,
						false,
						false,
						PORT_OPC_TOTEM_gc,
						PORT_ISC_LEVEL_gc );
							
	/* Initialize SPI master on port DATAFLASH_PORT. */
	SPI_MasterInit(&spiMasterC,
	            &DATALINK_SPI,
	            &DATALINK_PORT,
	            false,
	            SPI_MODE_0_gc,
	            SPI_INTLVL_OFF_gc,
	            false,
	            SPI_PRESCALER_DIV4_gc,0);
				   
	// Enable the spi interface
	SPI_MasterEnable (&spiMasterC, 1);
		
	// Set directions - data out, clk in
	DATALINK_PORT.DIRSET = DATALINK_DATA_PIN|DATALINK_CLK_PIN|DATALINK_CS_PIN;
			
	// drive data to high
	DATALINK_PORT.OUTSET = DATALINK_CS_PIN;	
}

int anibike_dl_master_check_connection ( void )
{
	
}

void anibike_dl_master_send_data ( uint8_t *data, uint8_t length )
{
	
}