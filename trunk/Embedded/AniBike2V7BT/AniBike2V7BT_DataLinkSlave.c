/*
 * AniBike2V7BT_DataLinkSlave.c
 *
 * Created: 10/22/2011 2:16:46 PM
 *  Author: David
 */ 
#include "AniBike2V7BT_Internal.h"

SPI_Slave_t spiSlaveC = {NULL, NULL};

void anibike_dl_slave_initialize ( void )
{	
	// map PORT C to virtual port 1
	PORT_MapVirtualPort1( PORTCFG_VP1MAP_PORTC_gc );
		
	// set clk out and data in
	DATALINK_PORT.DIRCLR = DATALINK_DATA_PIN|DATALINK_CLK_PIN|DATALINK_CS_PIN;
	
	// drive all to zero
	DATALINK_PORT.OUTCLR = DATALINK_DATA_PIN|DATALINK_CLK_PIN|DATALINK_CS_PIN;
		
	/* Initialize SPI slave on port C. */
	SPI_SlaveInit(&spiSlaveC,
	              &DATALINK_SPI,
	              &DATALINK_PORT,
	              false,
	              SPI_MODE_0_gc,
	              SPI_INTLVL_HI_gc);

		
	PORT_ConfigureInterrupt0( &DATALINK_PORT, PORT_INT0LVL_HI_gc, DATALINK_DATA_PIN );		
	PMIC.CTRL |= PMIC_LOLVLEN_bm|PMIC_MEDLVLEN_bm|PMIC_HILVLEN_bm;		
}

ISR(SPIC_INT_vect)
{
	/* Get received data. */
	uint8_t data = SPI_SlaveReadByte(&spiSlaveC);
}