/*
 * AniBike2V7BT.c
 *
 * Created: 4/8/2011 9:45:05 PM
 *  Author: David
 */ 

#include "AniBike2V7BT_Internal.h"

void hall_sensor_handler ( void )
{
	printf_P ( PSTR("Hall Sensor\r\n"));
}

/*****************************************************************
 *			M A I N    F U N C T I O N 
 *****************************************************************/
int main(void)
{
	SetClockFreq ( 16 );
	
	initialize_hall_sensor(  );
	initialize_lighting_system(  );	
	dataflash_spi_init (  );
	
	swUART_ConfigureDevice ( 0 );
	swUART_SetRxInterruptLevel ( 3 );
	swUART_SetInterruptHandler ( rx_handler );
	set_hall_interrupt_handler( hall_sensor_handler );
	
	anibike_dl_initialize ( ANIBIKE_DL_MASTER );	
	
	sei ( );
	
	//anibike_dl_send_data		( "david", 5 );
/*	MUX_ENABLE;
	MUX_SET_ROW (1);
	write_period_calibrations ( 1200, 300, 255 );

	RED_PWM_CTRL.CCABUF = 20;        
	RED_PWM_CTRL.CCBBUF = 20;
	RED_PWM_CTRL.CCCBUF = 20;
	RED_PWM_CTRL.CCDBUF = 20;
				
	GREEN_PWM_CTRL.CCABUF = 20;        
	GREEN_PWM_CTRL.CCBBUF = 20;
	GREEN_PWM_CTRL.CCCBUF = 20;
	GREEN_PWM_CTRL.CCDBUF = 20;
				
	BLUE_PWM_CTRL.CCABUF = 20;        
	BLUE_PWM_CTRL.CCBBUF = 20;
	BLUE_PWM_CTRL.CCCBUF = 20;
	BLUE_PWM_CTRL.CCDBUF = 20;
*/
	
	while (1)
	{
//		i += 1;
//		i = i%8;
//		MUX_SET_ROW (i);	
	}
}
