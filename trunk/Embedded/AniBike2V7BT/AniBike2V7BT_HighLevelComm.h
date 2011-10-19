/*
 * AniBike2V7BT_HighLevelComm.h
 *
 * Created: 10/17/2011 12:57:54 AM
 *  Author: David
 */ 


#ifndef ANIBIKE2V7BT_HIGHLEVELCOMM_H_
#define ANIBIKE2V7BT_HIGHLEVELCOMM_H_

/* EXTERN VARS
 * *****************/
extern volatile uint8_t		rxBuffer[128];
extern volatile uint8_t		rxLength;
extern volatile uint8_t		rxDataReady;


/* OPCODES
 * *****************/
#define ANIBIKE_HLCOMM_IDN			0x01
#define ANIBIKE_HLCOMM_SET_DATA		0x02
#define ANIBIKE_HLCOMM_SET_CLOCK	0x05
#define ANIBIKE_HLCOMM_SLEEP		0x06
#define ANIBIKE_HLCOMM_WAKEUP		0x07
#define ANIBIKE_HLCOMM_SETUP		0x08
#define ANIBIKE_HLCOMM_SET_CAL		0x09
#define ANIBIKE_HLCOMM_LIGHT_LED	0x10


/* HEADER
 * *****************/
typedef struct 
{
	uint8_t		opcode;
	uint8_t		length;
	uint8_t		dest;
} anibike_hlcomm_header;

/* Calibration Msg
 * *****************/
typedef struct  
{
	anibike_hlcomm_header	header;
	uint16_t				cal_red;
	uint16_t				cal_green;
	uint16_t				cal_blue;
} anibike_hlcomm_cal_msg;

/* Sleep/Wakeup Msg
 * *****************/
typedef struct  
{
	anibike_hlcomm_header	header;
} anibike_hlcomm_sleep_msg;

typedef struct  
{
	anibike_hlcomm_header	header;
} anibike_hlcomm_wakeup_msg;

/* Data Msg
 * *****************/
typedef struct  
{
	anibike_hlcomm_header	header;
	uint8_t					data[48];
} anibike_hlcomm_set_data_msg;

/* Light LED Msg
 * *****************/
typedef struct  
{
	anibike_hlcomm_header	header;
	uint8_t					row_num;
	uint8_t					rgb_choose;
	uint16_t				val;
} anibike_hlcomm_light_led_msg;

/*
 * High-level function definitions
 ***************************************************************************************/	

// MASTER SIDE
#ifdef _ANIBIKE_MASTER
	int anibike_hlcomm_send_cal_data ( uint16_t red, uint16_t green, uint16_t blue );
	int anibike_hlcomm_light_led_req ( uint8_t row, uint8_t rgb_choose, uint16_t val );
#endif

// SLAVE SIDE
#ifdef _ANIBIKE_SLAVE
	void anibike_hlcomm_setup_rx_system ( void );
	void anibike_hlcomm_handle_data ( void );
#endif

#endif /* ANIBIKE2V7BT_HIGHLEVELCOMM_H_ */