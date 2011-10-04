#include <avr/io.h>
#include <compat/deprecated.h>

#ifndef __INTERNAL_H__
#define __INTERNAL_H__

#include "AniBike2_Usart_Comm.h"

//==============================================================================================
//
//  Defines & Types
//
//==============================================================================================


#define	ANIBIKE_VER					2
#define	ANIBIKE_MEMORY_SIZE			32		// Mbit
#define	ANIBIKE_CPU_SPEED			196		// MHz
#define	ANIBIKE_NUM_LEDS			32		// Single radius
#define ANIBIKE_NUM_LEDS_POWER		5
#define	ANIBIKE_NUM_LEDS_BYTES		4
#define	ANIBIKE_NUM_LEDS_BYTES_POWER 	2
#define	ANIBIKE_IS_RGB				1
#define ANIBIKE_COLOR_DEPTH			16
#define ANIBIKE_COLOR_DEPTH_POWER	4

#define UART_COMMAND_MAX_LENGTH 64
#define UART_RESPOND_MAX_LENGTH 64



#define ANGULAR_RESOLUTION		200
#define	INITIAL_RAD2_LINE_POS	100			// The initial offset of radius number 2
											// Attention! while changing "RADIAL_RESOLUTION"
											// we need to change also INITIAL_RAD2_LINE_POS by Eq:
											// INITIAL_RAD2_LINE_POS = INITIAL_RAD1_LINE_POS + RADIAL_RESOLUTION/2
#define INITIAL_RAD1_LINE_POS	0			// The offset of the hall sensor from led radius

#define IMAGE_SIZE				9600		// ANGULAR_RESOLUTION*ANIBIKE_NUM_LEDS*ANIBIKE_COLOR_DEPTH_POWER/8
											// We should change it for different parameters but for now
											// hardcoded couse we need to save flash memory


#define PORTB_MASK		(0b00001111);
#define PORTC_MASK		(0b00000000);
#define PORTD_MASK		(0b11000011);
#define PORTF_MASK		(0b00000000);

// Port Definitions
#define DD_MOSI				PB2
#define DD_MISO				PB3
#define DD_SCK				PB1
#define DD_SS				PB0
#define DD_PORT				PORTB
#define DDR_SPI 			DDRB

// Macros
#define TWO_INDEXES 	asm ( "mov r17, r16" );	asm ( "swap r16" );	asm ( "andi r16, 0xF" ); asm ( "andi r17, 0xF" );
#define CS_UP		DD_PORT |= _BV(DD_SS)
#define CS_DOWN		DD_PORT &= ~(_BV(DD_SS))

//___________________________________________
// Functions
void DataTransferMode ( void );
void ProcessData ( void );

//___________________________________________
// LED Port Defines & Macros
#define 	ROW1	PA0
#define 	ROW2	PA1
#define 	ROW3	PA2
#define 	ROW4	PA3
#define 	ROW5	PA4
#define 	ROW6	PA5
#define 	ROW7	PA6
#define 	ROW8	PA7


#define 	L1C1_R	PB4
#define 	L1C2_R	PB5
#define 	L1C3_R	PB6
#define 	L1C4_R	PB7

#define 	L1C1_G	PD2
#define 	L1C2_G	PD3
#define 	L1C3_G	PD4
#define 	L1C4_G	PD5

#define 	L1C1_B	PF0
#define 	L1C2_B	PF1
#define 	L1C3_B	PF2
#define 	L1C4_B	PF3



#define 	L2C1_R	PF4
#define 	L2C2_R	PF5
#define 	L2C3_R	PF6
#define 	L2C4_R	PF7

#define 	L2C1_G	PC0
#define 	L2C2_G	PC1
#define 	L2C3_G	PC2
#define 	L2C4_G	PC3

#define 	L2C1_B	PC4
#define 	L2C2_B	PC5
#define 	L2C3_B	PC6
#define 	L2C4_B	PC7

#define 	ROW_PORT	PORTA

#define		L1C_OFFSET	2
#define		L2C_OFFSET	4
#define		L1C_PORT	DDRC
#define		L2C_PORT	DDRD
#define		L1C_PORT_VAL	PORTC
#define		L2C_PORT_VAL	PORTD
#define		L1C_MASK	0b00111100
#define		L2C_MASK	0b11110000
#define		L1C_NMASK	0b11000011
#define		L2C_NMASK	0b00001111


#define		SET_BUFFERREADY		GPIOR0 |= 0b00100000;
#define		UNSET_BUFFERREADY	GPIOR0 &= 0b11011111;
#define		BUFFER_IS_READY		(GPIOR0 & 0b00100000)

#define		SET_HIGHSPEED		GPIOR0 |= 0b10000000;
#define		UNSET_HIGHSPEED		GPIOR0 &= 0b01111111;
#define		IS_HIGHSPEED		(GPIOR0 & 0b10000000)

#define		SET_FIRST_BUFFER	GPIOR0 &= 0b10111111;
#define		SET_SECOND_BUFFER	GPIOR0 |= 0b01000000;
#define		WHICH_BUFFER		(GPIOR0 & 0b01000000)

#define		SET_ROW(r)			GPIOR0 &= 0b11110000; GPIOR0 |= r;
#define		ADVANCE_ROW			GPIOR0 = (GPIOR0 + 1)&(0b11100111); GPIOR2 = 0; GPIOR2 = (1<<(GPIOR0 & 0b00000111));
#define		ROW_NUM				(GPIOR0 & 0b00000111)

#define		PWM_COUNTER			GPIOR1
#define		ADVANCE_COUNTER		GPIOR1 = (GPIOR1 + 1)&(0b00001111);

#define		ROW_PATTERN			GPIOR2

#define		READ_FLASH_BYTE		{SPDR = 0; while (! (SPSR & (1<<SPIF))){}}

// MSB 4 is L2
// LSB 4 is L1
#define		UNIFY8TO44 ( L1, L2 )	((L2)<<4)|((L1)&0b00001111)

//___________________________________________
// Other Ports Defines & Macros
#define		HALL_SENSOR_PORT	PORTD
#define		HALL_SENSOR			PD2

#define		UART_INT_PORT		PORTD
#define		UART_INT			PD3


//___________________________________________
// Types
typedef struct
{	
	uint8_t Blue_L2[16];
	uint8_t Green_L2[16];
	uint8_t Red_L2[16];	
	uint8_t Blue_L1[16];
	uint8_t Green_L1[16];
	uint8_t Red_L1[16];
} Data_Line_ST;


//___________________________________________
// Code Control
#define __ENABLE_EXIT_UART
//#define __ENABLE_FILE_WRITE_UART
#define __ENABLE_DIRECT_WRITE_UART
#define __ENABLE_READ_UART
#define __ENABLE_IDN_UART
//#define __ENABLE_ECHO_UART
#define	__ENABLE_ECHO_DATA_UART
#define __ENABLE_READ_PARAM_UART
//#define __ENABLE_READ_IMAGE_LINE_UART


#define __ENABLE_POWER_SAFE


//#define __NOSLEEP_NOSPI_DEBUG

#endif
