/*
 * AniBike2V7BT_HallSensor.c
 *
 * Created: 8/22/2011 8:37:56 PM
 *  Author: David
 */ 
#include "AniBike2V7BT_Internal.h"

void (*hall_handler)( void ) = NULL;

void initialize_hall_sensor ( void )
{
	// Set pin2 to be input
	HALL_SENSOR_PORT.DIRCLR = HALL_SENSOR_PIN;
	
	// Setup asynchronous interrupt
	PORT_ConfigurePins( &HALL_SENSOR_PORT,
						HALL_SENSOR_PIN,
						false,
						false,
						PORT_OPC_PULLUP_gc,
						PORT_ISC_RISING_gc );
						
	PORT_ConfigureInterrupt0( &HALL_SENSOR_PORT, PORT_INT0LVL_MED_gc, HALL_SENSOR_PIN );		
}

void	set_hall_interrupt_handler ( void (*Handler)( void ) )
{
	hall_handler = Handler;
}

ISR(PORTA_INT0_vect)
{	
	if (hall_handler!=NULL)
		hall_handler ( );
}
