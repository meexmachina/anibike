/*
 * AniBike2V7BT_TransferProtocol.h
 *
 * Created: 8/20/2011 12:45:50 PM
 *  Author: David
 */ 


#ifndef ANIBIKE2V7BT_TRANSFERPROTOCOL_H_
#define ANIBIKE2V7BT_TRANSFERPROTOCOL_H_

/*****************************************************************
 *			H E A D E R    F I L E S
 *****************************************************************/


/*****************************************************************
 *			C O N S T A N T    D E F I N I T I O N S
 *****************************************************************/
#define COMMAND_PROMPT		"ANIBIKE_2V7BT >> "
#define MAX_MSG_SIZE    	60
#define	FS_MEMORY_SIZE		(uint32_t)(4194304)		// Bytes

/*****************************************************************
 *			T Y P E S    D E F I N I T I O N S
 *****************************************************************/
typedef uint8_t U8;
typedef struct
{
    char *cmd;
    void (*func)(U8 argc, char **argv);
	char *usage;
} cmd_t;



/*****************************************************************
 *			F U N C T I O N    D E F I N I T I O N S
 *****************************************************************/
void rx_handler( void );
void cmd_parse(char *cmd);
void cmd_menu( void );

void tm_cmd_echo_off		(U8 argc, char **argv);
void tm_cmd_echo_on			(U8 argc, char **argv);
void tm_cmd_stop_all		(U8 argc, char **argv);
void tm_cmd_start_all		(U8 argc, char **argv);
void tm_cmd_idn				(U8 argc, char **argv);
void tm_cmd_repa			(U8 argc, char **argv);
void tm_cmd_read			(U8 argc, char **argv);
void tm_cmd_write			(U8 argc, char **argv);
void tm_cmd_write_block		(U8 argc, char **argv);
void tm_cmd_read_block		(U8 argc, char **argv);
void tm_cmd_cs_low			(U8 argc, char **argv);
void tm_cmd_cs_high			(U8 argc, char **argv);
void tm_cmd_spi_transfer	(U8 argc, char **argv);
void tm_cmd_unprotect_flash	(U8 argc, char **argv);
void tm_cmd_protect_flash	(U8 argc, char **argv);
void tm_cmd_erase_flash		(U8 argc, char **argv);
void tm_cmd_flash_status	(U8 argc, char **argv);
void tm_cmd_dir				(U8 argc, char **argv);
void tm_cmd_get_fs			(U8 argc, char **argv);
void tm_cmd_test_led		(U8 argc, char **argv);
void tm_cmd_test_memory		(U8 argc, char **argv);
void tm_cmd_help			(U8 argc, char **argv);
void tm_cmd_software_reset	(U8 argc, char **argv);
void tm_cmd_configure_bt	(U8 argc, char **argv);
void tm_cmd_read_led_cal	(U8 argc, char **argv);
void tm_cmd_write_led_cal	(U8 argc, char **argv);
void tm_cmd_write_dl		(U8 argc, char **argv);


#endif /* ANIBIKE2V7BT_TRANSFERPROTOCOL_H_ */