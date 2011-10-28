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
#define MUX_CONTROL_PORT		PORTA//VPORT3_OUT
#define MUX_CONTROL_PORT_DIR	PORTA_DIR//VPORT3_DIR
#define MUX_CONTROL_PINS	(PIN4_bm|PIN5_bm|PIN6_bm|PIN7_bm)
#define MUX_ENABLE_PIN		0x80
#define MUX_ENABLE_PIN_N	0x7F
#define MUX_SET_ROW(r)		{MUX_CONTROL_PORT.OUTCLR = MUX_CONTROL_PINS; MUX_CONTROL_PORT.OUTSET = ((r)<<4);}//{MUX_CONTROL_PORT&=0x8F; MUX_CONTROL_PORT|=((r)<<4);}	// make it a virtual port 4!
#define MUX_DISABLE			{MUX_CONTROL_PORT.OUTCLR = MUX_ENABLE_PIN; }//{MUX_CONTROL_PORT&=MUX_ENABLE_PIN_N;}
#define MUX_ENABLE			{MUX_CONTROL_PORT.OUTSET = MUX_ENABLE_PIN; }//{MUX_CONTROL_PORT|=MUX_ENABLE_PIN;}
#define CURR_ROW			GPIO_GPIO0

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

/*
#define RED1	_SFR_MEM8(0x938)
#define RED2	_SFR_MEM8(0x93A)
#define RED3	_SFR_MEM8(0x93C)
#define RED4	_SFR_MEM8(0x93E)
#define GREEN1	_SFR_MEM8(0x838)
#define GREEN2	_SFR_MEM8(0x83A)
#define GREEN3	_SFR_MEM8(0x83C)
#define GREEN4	_SFR_MEM8(0x83E)
#define BLUE1	_SFR_MEM8(0xA38)
#define BLUE2	_SFR_MEM8(0xA3A)
#define BLUE3	_SFR_MEM8(0xA3C)
#define BLUE4	_SFR_MEM8(0xA3E)
*/

#define RED1	(*((uint8_t*)(0x928))) 
#define RED2	(*((uint8_t*)(0x92A)))
#define RED3	(*((uint8_t*)(0x92C)))
#define RED4	(*((uint8_t*)(0x92E)))
#define GREEN1	(*((uint8_t*)(0x828)))
#define GREEN2	(*((uint8_t*)(0x82A)))
#define GREEN3	(*((uint8_t*)(0x82C)))
#define GREEN4	(*((uint8_t*)(0x82E)))
#define BLUE1	(*((uint8_t*)(0xA28)))
#define BLUE2	(*((uint8_t*)(0xA2A)))
#define BLUE3	(*((uint8_t*)(0xA2C)))
#define BLUE4	(*((uint8_t*)(0xA2E)))


/*****************************************************************
 *			M A C R O    D E F I N I T I O N S
 *****************************************************************/
#define run_row_control			{TC1_SetCCAIntLevel(&ROW_TIMER_CTRL, TC_CCAINTLVL_LO_gc );}
#define stop_row_control		{TC1_SetCCAIntLevel(&ROW_TIMER_CTRL, TC_CCAINTLVL_OFF_gc );}
#define turn_on_projection		MUX_ENABLE
#define turn_off_projection		MUX_DISABLE

/*****************************************************************
 *			F U N C T I O N    D E F I N I T I O N S
 *****************************************************************/
void initialize_lighting_system ( void );
void read_period_calibrations ( uint16_t *r, uint16_t *g, uint16_t *b );
void write_period_calibrations ( uint16_t r, uint16_t g, uint16_t b );
void set_row_color ( uint8_t row_num, uint8_t color, uint8_t color4bit);	// color = 1(RED), 2(GREEN), 3(BLUE)
void switch_projection_state ( void );
void set_projection_buffer ( uint8_t *buffer );

#endif /* ANIBIKE2V7BT_LIGHTINGSYSTEM_H_ */