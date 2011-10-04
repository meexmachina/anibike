/*
 * AniBike2V7BT_Internal.h
 *
 * Created: 6/15/2011 11:09:03 PM
 *  Author: David
 */ 

#ifndef ANIBIKE2V7BT_INTERNAL_SLAVE_H_
#define ANIBIKE2V7BT_INTERNAL_SLAVE_H_

/*****************************************************************
 *			H E A D E R    F I L E S
 *****************************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "AniBike2V7BT_LightingSystem.h"
#include "clksys_driver.h"
#include "AniBike2V7BT_DataLink.h"

#include "swUART.h"
#include "port_driver.h"
#include "spi_driver.h"
#include "tc_driver.h"
#include "eeprom_driver.h"


/*****************************************************************
 *			C O N S T A N T    D E F I N I T I O N S
 *****************************************************************/

#define	ANIBIKE_VER				2
#define ANIBIKE_SUBVER			7
#define ANIBIKE_MEMORY_SIZE		32
#define ANIBIKE_CLOCK_SPEED		32
#define ANIBIKE_NUM_LEDS		32
#define ANIBIKE_IS_RGB			1

/*****************************************************************
 *			M A C R O    D E F I N I T I O N S
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

#define DATALINK_SPI		SPIC
#define DATALINK_DATA_PIN	PIN5_bm
#define DATALINK_DATA_PINN	5
#define DATALINK_CLK_PIN	PIN7_bm
#define DATALINK_CLK_PINN	7
#define DATALINK_PORT		PORTC
#define DATALINK_DATA_CTRL	DATALINK_PORT.PIN5CTRL
#define DATALINK_CLK_CTRL	DATALINK_PORT.PIN7CTRL
#define DATALINK_PORT_OUT	0x15		//VPORT1_OUT
#define DATALINK_PORT_IN	0x16		//VPORT1_IN


/*****************************************************************
 *			T Y P E S    D E F I N I T I O N S
 *****************************************************************/
extern SPI_Master_t spiMasterC;

/*****************************************************************
 *			F U N C T I O N    D E F I N I T I O N S
 *****************************************************************/
void	SetClockFreq ( uint8_t freq );



#endif /* ANIBIKE2V7BT_INTERNAL_SLAVE_H_ */
