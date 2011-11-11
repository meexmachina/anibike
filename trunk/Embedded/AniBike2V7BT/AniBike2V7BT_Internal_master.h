/*
 * AniBike2V7BT_Internal.h
 *
 * Created: 6/15/2011 11:09:03 PM
 *  Author: David
 */ 

#ifndef ANIBIKE2V7BT_INTERNAL_MASTER_H_
#define ANIBIKE2V7BT_INTERNAL_MASTER_H_

/*****************************************************************
 *			H E A D E R    F I L E S
 *****************************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <util/crc16.h>
#include <stdio.h>

#include "swUART.h"
#include "port_driver.h"
#include "spi_driver.h"
#include "tc_driver.h"
#include "rtc_driver.h"
#include "eeprom_driver.h"
#include "clksys_driver.h"
#include "DataFlash.h"

#include "AniBike2V7BT_FileSystem.h"
#include "AniBike2V7BT_TransferProtocol.h"
#include "AniBike2V7BT_LightingSystem.h"
#include "AniBike2V7BT_HallSensor.h"
#include "AniBike2V7BT_DataLinkMaster.h"


/*****************************************************************
 *			C O N S T A N T    D E F I N I T I O N S
 *****************************************************************/

//#define UART_PORT_IS_PORTA
#define UART_PORT_IS_PORTB
#define UART_PORT_OUT	0x11		//VPORT0_OUT
#define UART_PORT_IN	0x12		//VPORT0_IN
#define UART_PORT_DIR	VPORT0_DIR
#define UART_RX_PIN		2
#define UART_TX_PIN		3

#ifdef UART_PORT_IS_PORTA
	#define UART_PORT		PORTA
#else
	#ifdef UART_PORT_IS_PORTB
		#define UART_PORT		PORTB
	#else
		#ifdef UART_PORT_IS_PORTC
			#define UART_PORT		PORTC
		#else
			// and so on...
		#endif
	#endif
#endif

#define	ANIBIKE_VER				2
#define ANIBIKE_SUBVER			7
#define ANIBIKE_MEMORY_SIZE		32
#define ANIBIKE_CLOCK_SPEED		32
#define ANIBIKE_NUM_LEDS		32
#define ANIBIKE_IS_RGB			1
#define ANIBIKE_COLOR_DEPTH		4
#define ANIBIKE_NUM_OF_RADS		256


/*****************************************************************
 *			M A C R O    D E F I N I T I O N S
 *****************************************************************/
#define DATAFLASH_PORT		PORTD
#define DATAFLASH_SPI		SPID
#define CS_UP				DATAFLASH_PORT.OUTSET = PIN4_bm;
#define CS_DOWN				DATAFLASH_PORT.OUTCLR = PIN4_bm;

#define DATALINK_SPI		SPIC
#define DATALINK_DATA_PIN	PIN5_bm
#define DATALINK_DATA_PINN	5
#define DATALINK_CLK_PIN	PIN7_bm
#define DATALINK_CLK_PINN	7
#define DATALINK_CS_PIN		PIN4_bm
#define DATALINK_CS_PINN	4
#define DATALINK_PORT		PORTC
#define DATALINK_DATA_CTRL	DATALINK_PORT.PIN5CTRL
#define DATALINK_CLK_CTRL	DATALINK_PORT.PIN7CTRL
#define DATALINK_PORT_OUT	0x15		//VPORT1_OUT
#define DATALINK_PORT_IN	0x16		//VPORT1_IN

/*
 * GPIO definitions:
 *	GPIO0 - stores current row number for lighting system
 *  GPIO1 - stores different flags - flash data valid, datalink finished
 *  GPIO2 - stores current column (angle 0:255) counter
 *  GPIO3 - stores SW UART delay register that can change
 */
//#define FLASH_DATA_IS_VALID		(GPIO_GPIO1&0x01)
//#define SET_FLASH_DATA_VALID	{GPIO_GPIO1 |= 0x01;}	
//#define CLR_FLASH_DATA_VALID	{GPIO_GPIO1 &= 0xFE;}
//#define DL_SEND_IS_FINISHED	(GPIO_GPIO1&0x02)
//#define SET_DL_SEND_FINISHED	{GPIO_GPIO1 |= 0x02;}
//#define CLR_DL_SEND_FINISHED	{GPIO_GPIO1 &= 0xFD;}
#define ELAPSED_ANGLE			GPIO_GPIO1
#define CURRENT_ANGLE			GPIO_GPIO2


#define COLUMN_TIMER_CTRL		TCC1

#define NOP					{asm volatile ("nop\n\t");}
#define DELAY120NS			{NOP;NOP;NOP;NOP;}

/*****************************************************************
 *			T Y P E S    D E F I N I T I O N S
 *****************************************************************/
extern SPI_Master_t spiMasterD;
extern SPI_Master_t spiMasterC;

/*****************************************************************
 *			F U N C T I O N    D E F I N I T I O N S
 *****************************************************************/
void	SetClockFreq		( uint8_t freq );
void	hall_sensor_handler ( void );



#endif /* ANIBIKE2V7BT_INTERNAL_MASTER_H_ */