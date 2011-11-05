/*
 * AniBike2V7BT_DataLinkSlave.c
 *
 * Created: 10/22/2011 2:16:46 PM
 *  Author: David
 */ 
#include "AniBike2V7BT_Internal.h"

SPI_Slave_t spiSlaveC;
volatile	uint8_t		g_rx_data[DL_SLAVE_CIRC_BUFFER_SIZE] = {0};
volatile	uint8_t		g_data_counter = 0;
volatile	uint8_t		g_data_valid = 0;

volatile uint8_t g_buffer_I	[96] = {0};
volatile uint8_t g_buffer_II[96] = {0};
volatile uint8_t g_current_double_buffer = 0;		// 0=proj is I, receive=II
volatile uint8_t *g_receive_buffer;
volatile uint8_t *g_proj_buffer;
	
//__________________________________________________________________________________________________
void anibike_dl_slave_initialize ( void )
{	
	// map PORT C to virtual port 1
	PORT_MapVirtualPort1( PORTCFG_VP1MAP_PORTC_gc );
			
	// set clk out and data in
	DATALINK_PORT.DIRCLR = DATALINK_DATA_PIN|DATALINK_CLK_PIN|DATALINK_CS_PIN;
		
	PORT_ConfigurePins( &DATALINK_PORT,
						DATALINK_CLK_PIN|DATALINK_DATA_PIN,
						false,
						false,
						PORT_OPC_TOTEM_gc,
						PORT_ISC_BOTHEDGES_gc );
						
	PORT_ConfigurePins( &DATALINK_PORT,
						DATALINK_CS_PIN,
						false,
						false,
						PORT_OPC_PULLUP_gc,
						PORT_ISC_BOTHEDGES_gc );
	
		
	/* Initialize SPI slave on port C. */
	SPI_SlaveInit(&spiSlaveC,
	              &DATALINK_SPI,
	              &DATALINK_PORT,
	              false,
	              SPI_MODE_0_gc,
	              SPI_INTLVL_HI_gc);
				  
	PORT_ConfigureInterrupt0( &DATALINK_PORT, PORT_INT0LVL_HI_gc, DATALINK_CS_PIN );		

	PMIC.CTRL |= PMIC_LOLVLEN_bm|PMIC_MEDLVLEN_bm|PMIC_HILVLEN_bm;	
	
	DL_SLAVE_CIRC_BUFFER_START = 0;
	DL_SLAVE_CIRC_BUFFER_END = 0;	
	
	
	g_receive_buffer = g_buffer_II;
	g_proj_buffer = g_buffer_I;
	g_current_double_buffer = 0;
	set_projection_buffer ( g_proj_buffer );
}

//__________________________________________________________________________________________________
void anibike_dl_slave_set_receive_buffer ( uint8_t* buffer)
{
//	g_current_receive_buffer = buffer;
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
				DL_SLAVE_CIRC_BUFFER_FLUSH;
				run_row_control;
				// switch buffers
				if (g_current_double_buffer == 0)
				{
					g_proj_buffer = g_buffer_II;
					g_receive_buffer = g_buffer_I;
					g_current_double_buffer = 1;
				}	
				else
				{
					g_proj_buffer = g_buffer_I;	
					g_receive_buffer = g_buffer_II;
					g_current_double_buffer = 0;	
				}		
				
				set_projection_buffer ( g_proj_buffer );
				
				// re-init variables			
				// set data not valid
				g_data_valid = 0;
						
				// set data counter to zero
				g_data_counter = 0;
					
			}
			break;
		//_________________________________
		case DL_CONTINUE_DATA_BATCH:
			{
				uint8_t length = cur_byte&0x1F;
				
				/*if (g_data_valid)
				{
					DL_SLAVE_CIRC_BUFFER_THROW_MSG(length,g_rx_data);
				}	*/				
				while (length--)
				{
					uint8_t temp = DL_SLAVE_CIRC_BUFFER_POP(g_rx_data);
					g_receive_buffer[g_data_counter] = temp;
					g_data_counter++;
				}						
							
			}	
			break;
		//_________________________________
		case DL_LIGHT_LEDS_DEBUG:
				{
					uint8_t rownum;
					uint8_t rgb;
					uint8_t val;
					
					stop_row_control;
					
					if (val>0)
					{
						MUX_ENABLE;
					}						
					else
					{ 
						MUX_DISABLE;
					}						
					
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
				
				//printf_P(PSTR("%d %d %d"), (uint8_t)(r&0xff), (uint8_t)(g&0xff), (uint8_t)(b&0xff));
				
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
ISR(SPIC_INT_vect, ISR_BLOCK)
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

//__________________________________________________________________________________________________
ISR(PORTC_INT0_vect, ISR_BLOCK)
{
	DL_SLAVE_CIRC_BUFFER_FLUSH;
	/*
	if ( (DATALINK_PORT.IN&DATALINK_CS_PIN)!=0 )
	{
		NOP; NOP; NOP;
		g_debaunce_cs = (DATALINK_PORT.IN&DATALINK_CS_PIN)!=0; // can be either =0 or !=0
																// if =0 next time do flush
	}
	else
	{
		if (g_debaunce_cs)
			DL_SLAVE_CIRC_BUFFER_FLUSH;
		
		g_debaunce_cs = 1;
	}	*/
}