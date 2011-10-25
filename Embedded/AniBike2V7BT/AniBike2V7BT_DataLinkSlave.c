/*
 * AniBike2V7BT_DataLinkSlave.c
 *
 * Created: 10/22/2011 2:16:46 PM
 *  Author: David
 */ 
#include "AniBike2V7BT_Internal.h"

SPI_Slave_t spiSlaveC;
volatile	uint8_t		g_rx_data[DL_SLAVE_CIRC_BUFFER_SIZE] = {0};
volatile	uint8_t		*g_current_receive_buffer = NULL;
volatile	uint8_t		g_data_valid = 0;
	
//__________________________________________________________________________________________________
void anibike_dl_slave_initialize ( void )
{	
	// map PORT C to virtual port 1
	PORT_MapVirtualPort1( PORTCFG_VP1MAP_PORTC_gc );
			
	// set clk out and data in
	DATALINK_PORT.DIRCLR = DATALINK_DATA_PIN|DATALINK_CLK_PIN|DATALINK_CS_PIN;
		
	PORT_ConfigurePins( &DATALINK_PORT,
						DATALINK_CLK_PIN|DATALINK_DATA_PIN|DATALINK_CS_PIN,
						false,
						false,
						PORT_OPC_TOTEM_gc,
						PORT_ISC_LEVEL_gc );
	
		
	/* Initialize SPI slave on port C. */
	SPI_SlaveInit(&spiSlaveC,
	              &DATALINK_SPI,
	              &DATALINK_PORT,
	              false,
	              SPI_MODE_0_gc,
	              SPI_INTLVL_HI_gc);

	PMIC.CTRL |= PMIC_LOLVLEN_bm|PMIC_MEDLVLEN_bm|PMIC_HILVLEN_bm;	
	
	DL_SLAVE_CIRC_BUFFER_START = 0;
	DL_SLAVE_CIRC_BUFFER_END = 0;	
}

//__________________________________________________________________________________________________
void anibike_dl_slave_set_receive_buffer ( uint8_t* buffer)
{
	g_current_receive_buffer = buffer;
}

//__________________________________________________________________________________________________
void anibike_dl_slave_handle_data ( void )
{
	uint8_t opcode;
	uint8_t cur_byte = 0;
	
	//if (!DATA_IS_VALID)
	//	return;
	
	// check if the fifo has grown
	if (DL_SLAVE_CIRC_BUFFER_LENGTH == 0)
	{
		return;
	}		
	
	cur_byte = DL_SLAVE_CIRC_BUFFER_TOP(g_rx_data);
	
	// not all the data has been gotten yet
	if (DL_SLAVE_CIRC_BUFFER_LENGTH <= (cur_byte&0x1F))
	{
		return;
	}		
			
	cur_byte = DL_SLAVE_CIRC_BUFFER_POP(g_rx_data);
	opcode = cur_byte&0xE0;
	
	switch (opcode)
	{
		//_________________________________
		case DL_NEW_TIMING_SYNC:
			{
				// read the new timing
				uint8_t temp1 = DL_SLAVE_CIRC_BUFFER_POP(g_rx_data);
				uint8_t temp2 = DL_SLAVE_CIRC_BUFFER_POP(g_rx_data);
				
				// switch buffers
			
				// re-init variables				
			}
			break;
		//_________________________________
		case DL_RESTART_DATA_BATCH:
			// set data not valid
			
			// set data counter to zero
			break;
		//_________________________________
		case DL_CONTINUE_DATA_BATCH:
			{
				uint8_t length = cur_byte&0x1F;
				uint8_t temp;
				
				while (length--)
				{
					temp = DL_SLAVE_CIRC_BUFFER_POP(g_rx_data);
				}					
						
				// copy data to current data receive buffer
					
				// progress counter
				
			}	
			break;
		//_________________________________
		case DL_LIGHT_LEDS_DEBUG:
				{
					uint8_t rownum;
					uint8_t rgb;
					uint8_t val;
					
					rownum = DL_SLAVE_CIRC_BUFFER_POP(g_rx_data);
					rgb = DL_SLAVE_CIRC_BUFFER_POP(g_rx_data);
					val = DL_SLAVE_CIRC_BUFFER_POP(g_rx_data);
					
					if (rgb&0x01)
					{
						set_row_color (rownum, 1, val);
					}

					if (rgb&0x02)
					{
						set_row_color ( rownum, 2, val);
					}

					if (rgb&0x04)
					{
						set_row_color ( rownum, 3, val);
					}
				}			
			break;
		//_________________________________
		case DL_SET_ADDRESS:
			{
				uint8_t address = DL_SLAVE_CIRC_BUFFER_POP(g_rx_data);
				
				// do something with the address
			}
			break;
		//_________________________________
		case DL_GO_TO_SLEEP:
			{
				// do something to go to sleep
			}
			break;
		//_________________________________	
		case DL_SET_CAL_VALUES:
			{
				uint8_t temp1 = DL_SLAVE_CIRC_BUFFER_POP(g_rx_data);	// lsb
				uint8_t temp2 = DL_SLAVE_CIRC_BUFFER_POP(g_rx_data);	// msb
				uint16_t r = ((uint16_t)(temp1))|(((uint16_t)(temp2))<<8);
				temp1 = DL_SLAVE_CIRC_BUFFER_POP(g_rx_data);
				temp2 = DL_SLAVE_CIRC_BUFFER_POP(g_rx_data);				
				uint16_t g = ((uint16_t)(temp1))|(((uint16_t)(temp2))<<8);
				DL_SLAVE_CIRC_BUFFER_POP(g_rx_data);
				DL_SLAVE_CIRC_BUFFER_POP(g_rx_data);				
				uint16_t b = ((uint16_t)(temp1))|(((uint16_t)(temp2))<<8);
				
				printf_P(PSTR("%d %d %d"), (uint8_t)(r&0xff), (uint8_t)(g&0xff), (uint8_t)(b&0xff));
				
				//write_period_calibrations ( r, g, b );
			}				
			break;
		//_________________________________	
		default:
			DL_SLAVE_CIRC_BUFFER_FLUSH;
			break;
	};
}

//__________________________________________________________________________________________________
ISR(SPIC_INT_vect)
{
	uint8_t temp;
	uint8_t length;
	/* Get received data. */
	
	temp = SPI_SlaveReadByte(&spiSlaveC);
	DL_SLAVE_CIRC_BUFFER_ADD (g_rx_data, temp);
	length = temp&0x1F;	// extract the length
	
	while (length--)
	{
		while (!SPI_SlaveDataAvailable(&spiSlaveC)) {}
		DL_SLAVE_CIRC_BUFFER_ADD (g_rx_data, SPI_SlaveReadByte(&spiSlaveC));
	}
	
	g_data_valid=1;
}