/*
 * AniBike2V7BT_LightingSystem.c
 *
 * Created: 8/22/2011 5:13:59 PM
 *  Author: David
 */ 
#include "AniBike2V7BT_Internal.h"

volatile uint16_t	g_iRedCalibrationPeriod = 280;			// a little bit more dimmed
volatile uint16_t	g_iGreenCalibrationPeriod = 235;		// maximum 235
volatile uint16_t	g_iBlueCalibrationPeriod = 235;			// maximum 235
//volatile uint8_t	g_current_row = 0;
//volatile uint8_t	g_current_row_place = 0;

volatile uint8_t CIE_Gamma_4bit[] = {0,2,4,7,12,18,27,38,51,67,86,108,134,163,197,235};
	
/*****************************************************************
 *			BUFFERS
 *****************************************************************/
volatile uint8_t g_flash_read_buffer_I	[48] = {0};
volatile uint8_t g_flash_read_buffer_II	[48] = {0};
volatile uint8_t *g_current_flash_buffer = NULL;

//__________________________________________________________________________________________________
void initialize_lighting_system ( void )
{
	// Row MUX control init
	MUX_CONTROL_PORT_DIR |= MUX_CONTROL_PINS;
	MUX_DISABLE;
	MUX_SET_ROW(0);
	
	// Column ports init
	GREEN_PORT.DIRSET = LED_COLUMN_PINS;
	RED_PORT.DIRSET = LED_COLUMN_PINS;
	BLUE_PORT.DIRSET = LED_COLUMN_PINS;
	
	PORT_ConfigurePins( &RED_PORT,  LED_COLUMN_PINS, 1, 1, PORT_OPC_WIREDANDPULL_gc, PORT_ISC_INPUT_DISABLE_gc );
	PORT_ConfigurePins( &GREEN_PORT,  LED_COLUMN_PINS, 1, 1, PORT_OPC_WIREDANDPULL_gc, PORT_ISC_INPUT_DISABLE_gc );
	PORT_ConfigurePins( &BLUE_PORT,  LED_COLUMN_PINS, 1, 1, PORT_OPC_WIREDANDPULL_gc, PORT_ISC_INPUT_DISABLE_gc );
	
	GREEN_PWM_CTRL.CTRLB |= TC0_CCAEN_bm;
	GREEN_PWM_CTRL.CTRLB |= TC0_CCBEN_bm;
	GREEN_PWM_CTRL.CTRLB |= TC0_CCCEN_bm;
	GREEN_PWM_CTRL.CTRLB |= TC0_CCDEN_bm;
	GREEN_PWM_CTRL.CTRLB |= TC_WGMODE_SS_gc;
	
	RED_PWM_CTRL.CTRLB |= TC0_CCAEN_bm;
	RED_PWM_CTRL.CTRLB |= TC0_CCBEN_bm;
	RED_PWM_CTRL.CTRLB |= TC0_CCCEN_bm;
	RED_PWM_CTRL.CTRLB |= TC0_CCDEN_bm;
	RED_PWM_CTRL.CTRLB |= TC_WGMODE_SS_gc;
	
	BLUE_PWM_CTRL.CTRLB |= TC0_CCAEN_bm;
	BLUE_PWM_CTRL.CTRLB |= TC0_CCBEN_bm;
	BLUE_PWM_CTRL.CTRLB |= TC0_CCCEN_bm;
	BLUE_PWM_CTRL.CTRLB |= TC0_CCDEN_bm;
	BLUE_PWM_CTRL.CTRLB |= TC_WGMODE_SS_gc;
	
	TC0_ConfigClockSource(&RED_PWM_CTRL, TC_CLKSEL_DIV1_gc);
	TC0_ConfigClockSource(&GREEN_PWM_CTRL, TC_CLKSEL_DIV1_gc);
	TC0_ConfigClockSource(&BLUE_PWM_CTRL, TC_CLKSEL_DIV1_gc);
	
	read_period_calibrations ( &g_iRedCalibrationPeriod, &g_iGreenCalibrationPeriod, &g_iBlueCalibrationPeriod );
	
	TC_SetPeriod(&GREEN_PWM_CTRL, g_iGreenCalibrationPeriod);
	TC_SetPeriod(&RED_PWM_CTRL, g_iRedCalibrationPeriod);
	TC_SetPeriod(&BLUE_PWM_CTRL, g_iBlueCalibrationPeriod);
	
	RED_PWM_CTRL.CNT = 0;
	GREEN_PWM_CTRL.CNT = 0;
	BLUE_PWM_CTRL.CNT = 0;
	
	
	// setup row control time on TCC1A
	ROW_TIMER_CTRL.CTRLA |= TC1_CCAEN_bm;
	TC1_ConfigClockSource(&ROW_TIMER_CTRL, TC_CLKSEL_DIV64_gc);	// we need it every 64 microseconds
	ROW_TIMER_CTRL.CTRLB |= TC1_WGMODE0_bm|TC1_WGMODE1_bm;
	ROW_TIMER_CTRL.PER = 0x20;
	ROW_TIMER_CTRL.CCA = 0x20;
	TC1_SetCCAIntLevel(&ROW_TIMER_CTRL, TC_CCAINTLVL_LO_gc );
	ROW_TIMER_CTRL.CNT = 0;
}

//__________________________________________________________________________________________________
void read_period_calibrations ( uint16_t *r, uint16_t *g, uint16_t *b )
{
	EEPROM_DisableMapping();
	*r = EEPROM_ReadByte	(EEPROM_LED_CONFIG_PAGE, EEPROM_R_CONFIG_WORD*2);
	*r |= (EEPROM_ReadByte	(EEPROM_LED_CONFIG_PAGE, EEPROM_R_CONFIG_WORD*2+1))<<8;
	
	*g = EEPROM_ReadByte	(EEPROM_LED_CONFIG_PAGE, EEPROM_G_CONFIG_WORD*2);
	*g |= (EEPROM_ReadByte	(EEPROM_LED_CONFIG_PAGE, EEPROM_G_CONFIG_WORD*2+1))<<8;
	
	*b = EEPROM_ReadByte	(EEPROM_LED_CONFIG_PAGE, EEPROM_B_CONFIG_WORD*2);
	*b |= (EEPROM_ReadByte	(EEPROM_LED_CONFIG_PAGE, EEPROM_B_CONFIG_WORD*2+1))<<8;
}

//__________________________________________________________________________________________________
void write_period_calibrations ( uint16_t r, uint16_t g, uint16_t b )
{
	EEPROM_FlushBuffer();
	EEPROM_DisableMapping();
	EEPROM_ErasePage( EEPROM_LED_CONFIG_PAGE );
	EEPROM_WaitForNVM(  );
	
	EEPROM_WriteByte(EEPROM_LED_CONFIG_PAGE, EEPROM_R_CONFIG_WORD*2, r&0xFF);
	EEPROM_WriteByte(EEPROM_LED_CONFIG_PAGE, EEPROM_R_CONFIG_WORD*2+1, (r>>8)&0xFF);
	
	EEPROM_WriteByte(EEPROM_LED_CONFIG_PAGE, EEPROM_G_CONFIG_WORD*2, g&0xFF);
	EEPROM_WriteByte(EEPROM_LED_CONFIG_PAGE, EEPROM_G_CONFIG_WORD*2+1, (g>>8)&0xFF);
	
	EEPROM_WriteByte(EEPROM_LED_CONFIG_PAGE, EEPROM_B_CONFIG_WORD*2, b&0xFF);
	EEPROM_WriteByte(EEPROM_LED_CONFIG_PAGE, EEPROM_B_CONFIG_WORD*2+1, (b>>8)&0xFF);
	
	TC_SetPeriod(&GREEN_PWM_CTRL, g);
	TC_SetPeriod(&RED_PWM_CTRL, r);
	TC_SetPeriod(&BLUE_PWM_CTRL, b);
}

//__________________________________________________________________________________________________
void set_row_color ( uint8_t row_num, uint8_t color, uint8_t color4bit)	// color = 1(RED), 2(GREEN), 3(BLUE)
{
	uint8_t val = CIE_Gamma_4bit[color4bit];
	MUX_ENABLE;
	// Set the row num
	MUX_SET_ROW (row_num);
					
	if (color==1)
	{
		RED_PWM_CTRL.CCABUF = val;        
		RED_PWM_CTRL.CCBBUF = val;
		RED_PWM_CTRL.CCCBUF = val;
		RED_PWM_CTRL.CCDBUF = val;	
	}	
	else if (color==2)
	{
		GREEN_PWM_CTRL.CCABUF = val;        
		GREEN_PWM_CTRL.CCBBUF = val;
		GREEN_PWM_CTRL.CCCBUF = val;
		GREEN_PWM_CTRL.CCDBUF = val;
	}					
	else if (color==3)
	{
		BLUE_PWM_CTRL.CCABUF = val;        
		BLUE_PWM_CTRL.CCBBUF = val;
		BLUE_PWM_CTRL.CCCBUF = val;
		BLUE_PWM_CTRL.CCDBUF = val;
	}
}


// TBD - REWRITE!!!
//__________________________________________________________________________________________________
void set_projection_state ( uint8_t *data )
{
	uint8_t *place = data+(uint8_t)(CURR_ROW*6);
	uint8_t col = 0;
		
	col = ((*place)&0xf0)>>4;
	col *= col;
	RED1 = col;
	col = ((*place)&0x0f);  
	col *= col;             
	GREEN1 = col;
	col = ((*(place+1))&0xf0)>>4;  
	col *= col;           
	BLUE1 = col;        
	col = ((*(place+1))&0x0f);
	col *= col;
	RED2 = col;
	col = ((*(place+2))&0xf0)>>4;
	col *= col;
	GREEN2 = col;
	col = ((*(place+2))&0x0f);
	col *= col;
	BLUE2 = col;
	col = ((*(place+3))&0xf0)>>4;
	col *= col;
	RED3 = col;        
	col = ((*(place+3))&0x0f);
	col *= col;
	GREEN3 = col;        
	col = ((*(place+4))&0xf0)>>4;
	col *= col;
	BLUE3 = col;        
	col = ((*(place+4))&0x0f);
	col *= col;
	RED4 = col;
	col = ((*(place+5))&0xf0)>>4;
	col *= col;
	GREEN4 = col;
	col = ((*(place+5))&0x0f);
	col *= col;
	BLUE4 = col;
}

//__________________________________________________________________________________________________
ISR(TCC1_CCA_vect)
{
	CURR_ROW ++;						// 3cc
	CURR_ROW &= 0x07;					// 3cc
	MUX_SET_ROW (CURR_ROW);

	set_projection_state ( g_flash_read_buffer_I );
}