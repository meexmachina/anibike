/*
 * AniBike2V7BT_LightingSystem.c
 *
 * Created: 8/22/2011 5:13:59 PM
 *  Author: David
 */ 
#include "AniBike2V7BT_Internal.h"

volatile uint16_t	g_iRedCalibrationPeriod = 255;			// a little bit more dimmed
volatile uint16_t	g_iGreenCalibrationPeriod = 235;		// maximum 235
volatile uint16_t	g_iBlueCalibrationPeriod = 235;			// maximum 235

prog_uint8_t CIE_Gamma_4bit[] PROGMEM = {0,2,4,7,12,18,27,38,51,67,86,108,134,163,197,235};

//__________________________________________________________________________________________________
void initialize_lighting_system ( void )
{
	// Row MUX control init
	MUX_CONTROL_PORT.DIRSET = MUX_CONTROL_PINS;
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