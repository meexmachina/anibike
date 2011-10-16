/*
 * AniBike2V7BT_DataLink.h
 *
 * Created: 8/22/2011 8:39:50 PM
 *  Author: David
 */ 


#ifndef ANIBIKE2V7BT_DATALINK_H_
#define ANIBIKE2V7BT_DATALINK_H_

/*
 * Explanation:
 * ===========
 * The system has 32 LEDs on each bar. The master bar is gathering information from the flash
 * non-volatile memory - for the next cycle (!!) - and puts it in its buffers. Then sends the
 * information that is needed by the second bar (slave) in the next cycle. The amount of information
 * that is read from the flash is:
 *   RGB = 3
 *   bits_per_color = 4
 *   num_of_leds = 32
 *   TOTAL = 2 * #bits_per_led * num_of_leds = RGB*bits_per_color*num_of_leds = 2*384bits = 2*48 bytes
 * So the amount of info is 48 bytes to send.
 *
 * The init for the two (or more) nodes:
 *   Master: Sets itself to wired-or			Slave: sets itself to wired-or
 *   Master: Driving data line high (clk input).Slave: driving clk line high. data input.
 *   Master: idle.                              Slave: wait for interrupt on data (going low).
 * 
 * Transaction process start:
 *   Master: set data low, wait to clk low.     Slave: catches interrupt on data (low), activates
 *													   interrupt handler, sets clk input (which
 *													   makes the line low because its pulled-down). 
 *   Master: gets clk low, sets it output.      Slave: waiting for data.
 *
 * Transaction process for message after start:
 *   Master: set both lines totem-pole
 *   Master: Send length						Slave: read and store data length
 *   Master: Send L bytes of info				Slave: read out received L bytes
 *   Master: Send checksum						Slave: read out checksum and calculate + compare
 *
 * Transaction process verify:
 *   Master: Set both lines wired-or.
 *   Master: Set lines clk as IN, data out low  Slave: Set line clk as out low, data in
 *   Master: check if clk becomes high   		Slave: ACK := clk high; NACK := clk stays low
 *   Master: Wait till got high clock or timeout		Slave: wait until data becomes high
 *
 * Transaction ending:
 *   Master: data is high, clk input			Slave: sees that data high, sets clk high
 *   Master: wait till clk high					Slave: idle and waiting for the next interrupt				
 *
 * Master understands he is alone if the clk line is low.
 * If the slave stopped working during transaction, the master will still send everything, but
 * at the end of the transaction he will see that the line stays low after data was set high.
 * For a big number of slaves connected to the same master, when the data goes high all of them
 * set the clk as input. all of them listen. all of them read out the transaction data. The one 
 * that recognizes its address drives the clk line high.
 * Once in a second the master will send IDN command to the slave. The slave will return ACK
 *
 */


/*
 * Opcodes definition
 ***************************************************************************************/
#define ANIBIKE_DL_ACK_PIN		DATALINK_DATA_PIN
#define ANIBIKE_DL_NACK_PIN		DATALINK_CLK_PIN


/*
 * Other definitions
 ***************************************************************************************/
#define ANIBIKE_DL_SYNC_BYTE			0x81	//  1  0  0  0  0  0  0  1
												// |--|--|--|--|--|--|--|--
												//  ^-- interrupt, waiting for some time.
												// all the other bytes come after this one.

#define ANIBIKE_DL_RX_TIMEOUT			0x80	// number of clock cycles to receive data

typedef enum
{
	ANIBIKE_DL_MASTER = 0,					// the master is the one that initiates conversation
	ANIBIKE_DL_SLAVE  = 1					// the slave has no initiative
} ANIBIKE_DL_TYPE_EN;
	

/*
 * Macro definitions
 ***************************************************************************************/					
#define anibike_dl_send_byte(spi,b)		{SPI_MasterTransceiveByte(&(spi), (b));}

/*
 * Functions definitions
 ***************************************************************************************/		
void anibike_dl_initialize		( ANIBIKE_DL_TYPE_EN enNodeType );	
uint8_t anibike_dl_send_data	( uint8_t *aData, uint8_t iLength );
void anibike_dl_receive_data	( void );
uint8_t anibike_dl_receive_byte ( void ) __attribute__((naked));
void anibike_dl_flush			( void );

#endif	//ANIBIKE2V7BT_DATALINK_H_