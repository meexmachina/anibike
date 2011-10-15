/*
 * AniBike2V7BT_HallSensor.h
 *
 * Created: 8/22/2011 8:37:38 PM
 *  Author: David
 */ 


#ifndef ANIBIKE2V7BT_HALLSENSOR_H_
#define ANIBIKE2V7BT_HALLSENSOR_H_


/*****************************************************************
 *			C O N S T A N T    D E F I N I T I O N S
 *****************************************************************/
#define HALL_SENSOR_PORT	PORTA
#define HALL_SENSOR_PIN		PIN2_bm

/*****************************************************************
 *			F U N C T I O N    D E F I N I T I O N S
 *****************************************************************/
void initialize_hall_sensor ( void );
void set_hall_interrupt_handler ( void (*Handler)( void ) );

#endif /* ANIBIKE2V7BT_HALLSENSOR_H_ */