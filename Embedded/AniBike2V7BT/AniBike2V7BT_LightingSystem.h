/*
 * AniBike2V7BT_LightingSystem.h
 *
 * Created: 8/22/2011 5:13:47 PM
 *  Author: David
 */ 


#ifndef ANIBIKE2V7BT_LIGHTINGSYSTEM_H_
#define ANIBIKE2V7BT_LIGHTINGSYSTEM_H_


/*****************************************************************
 *			C O N S T A N T    D E F I N I T I O N S
 *****************************************************************/
#define MUX_CONTROL_PORT	PORTA
#define MUX_CONTROL_PINS	(PIN4_bm|PIN5_bm|PIN6_bm|PIN7_bm)
#define MUX_ENABLE_PIN		PIN7_bm
#define MUX_SET_ROW(r)		{PORTA_OUTCLR=0x70; PORTA_OUTSET=((r)<<4);}	// make it a virtual port 4!
#define MUX_DISABLE			{PORTA_OUTSET=MUX_ENABLE_PIN;}
#define MUX_ENABLE			{PORTA_OUTCLR=MUX_ENABLE_PIN;}

#define LED_COLUMN_PINS		(PIN0_bm|PIN1_bm|PIN2_bm|PIN3_bm)

#define GREEN_PORT				PORTC
#define GREEN_PWM_CTRL			TCC0
#define RED_PORT				PORTD
#define RED_PWM_CTRL			TCD0
#define BLUE_PORT				PORTE
#define BLUE_PWM_CTRL			TCE0

#define ROW_TIMER_CTRL			TCC1

#define EEPROM_LED_CONFIG_PAGE	0
#define EEPROM_R_CONFIG_WORD	0
#define EEPROM_G_CONFIG_WORD	1
#define EEPROM_B_CONFIG_WORD	2

#define RED1	(*((uint8_t*)(0x938)))
#define RED2	(*((uint8_t*)(0x93A)))
#define RED3	(*((uint8_t*)(0x93C)))
#define RED4	(*((uint8_t*)(0x93E)))
#define GREEN1	(*((uint8_t*)(0x838)))
#define GREEN2	(*((uint8_t*)(0x83A)))
#define GREEN3	(*((uint8_t*)(0x83C)))
#define GREEN4	(*((uint8_t*)(0x83E)))
#define BLUE1	(*((uint8_t*)(0xA38)))
#define BLUE2	(*((uint8_t*)(0xA3A)))
#define BLUE3	(*((uint8_t*)(0xA3C)))
#define BLUE4	(*((uint8_t*)(0xA3E)))


/*****************************************************************
 *			M A C R O    D E F I N I T I O N S
 *****************************************************************/
#define run_row_control			{TC1_SetCCAIntLevel(&ROW_TIMER_CTRL, TC_CCAINTLVL_LO_gc );}
#define stop_row_control		{TC1_SetCCAIntLevel(&ROW_TIMER_CTRL, TC_CCAINTLVL_OFF_gc );}


/*****************************************************************
 *			F U N C T I O N    D E F I N I T I O N S
 *****************************************************************/
void initialize_lighting_system ( void );
void read_period_calibrations ( uint16_t *r, uint16_t *g, uint16_t *b );
void write_period_calibrations ( uint16_t r, uint16_t g, uint16_t b );
void set_row_color ( uint8_t row_num, uint8_t color, uint8_t color4bit);	// color = 1(RED), 2(GREEN), 3(BLUE)
void set_projection_state ( uint8_t *data );

#endif /* ANIBIKE2V7BT_LIGHTINGSYSTEM_H_ */