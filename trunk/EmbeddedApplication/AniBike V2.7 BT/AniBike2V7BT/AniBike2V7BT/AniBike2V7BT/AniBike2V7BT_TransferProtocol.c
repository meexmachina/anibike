#include "AniBike2V7BT_Internal.h"

extern uint8_t g_iCurrentFrameCycle;
extern uint16_t	g_iRedCalibrationPeriod;
extern uint16_t	g_iGreenCalibrationPeriod;
extern uint16_t	g_iBlueCalibrationPeriod;

static U8 msg[MAX_MSG_SIZE];
static U8 last_msg[MAX_MSG_SIZE];
static U8 *msg_ptr;

volatile uint8_t 	g_leftDataToGet 	= 0;
volatile uint32_t 	g_dataStartAddress 	= 0;  
volatile uint8_t	g_EchoOnOff = 1;
volatile uint8_t	g_isOperating = 0;

static cmd_t cmd_tbl[] = 
{
	{"idn?",			tm_cmd_idn,					""					},
	{"repa",			tm_cmd_repa,				""					},
	{"read",			tm_cmd_read,				"[st] [len]"		},
    {"write",			tm_cmd_write,				"[st] [len]"		},
	{"write_blk",		tm_cmd_write_block,			"[blk#] [qnt] [len]"},
	{"read_blk",		tm_cmd_read_block,			"[blk#]"			},	
	{"cs_low",			tm_cmd_cs_low,				""					},
	{"cs_high",			tm_cmd_cs_high,				""					},
	{"spi_trans",		tm_cmd_spi_transfer,		"[u8]"				},
	{"unprot_flash",	tm_cmd_unprotect_flash,		"[blk#/all]"		},
	{"prot_flash",		tm_cmd_protect_flash,		"[blk#/all]"		},
	{"erase_flash",		tm_cmd_erase_flash,			"[blk#/all]"		},
	{"flash_stat",		tm_cmd_flash_status,		""					},
	{"dir",				tm_cmd_dir,					""					},
	{"get_fs",			tm_cmd_get_fs,				""					},
	{"test_led",		tm_cmd_test_led,			"[row#] [rgb]"		},
	{"mem_test",		tm_cmd_test_memory,			""					},
	{"help",			tm_cmd_help,				"[cmd]"				},
	{"sw_reset",		tm_cmd_software_reset,		""					},
	{"eof",				tm_cmd_echo_off,			""					},
	{"eon",				tm_cmd_echo_on,				""					},
	{"stop_all", 		tm_cmd_stop_all,			""					},
	{"start_all",		tm_cmd_start_all, 			""					},
	{"configure_bt",	tm_cmd_configure_bt,		""					},	
	{"read_cal",		tm_cmd_read_led_cal,		""					},		
	{"write_cal",		tm_cmd_write_led_cal,		"r16 g16 b16"		},
	{"write_dl",		tm_cmd_write_dl,			"str"				},	
	{"exit",			NULL,						""					},		
	{NULL,      		NULL,						NULL				}
};

/**************************************************************************/
/*!
    Reading a line from the instream
*/
/**************************************************************************/
/*uint8_t tm_get_new_line ( char *buffer )
{
	uint8_t cnt = 0;
	char c;
	
	do
	{
		c = swUART_ReadChar (  );
		buffer[cnt++]=c;
	} while (c != '\r' && c != '\n');
	
	buffer[cnt] = '\0';
	return cnt;
}*/


/**************************************************************************/
/*!
    This is the rx handler function. Collecting transmitted (rx) command
	and echoing it back. As the cmd is finished transmitting, it sends it
	to the parser
*/
/**************************************************************************/
void rx_handler ( void )
{
	char c;
	bool bInLoop = true;
	
	printf_P( PSTR("ANIBIKE INC. VER. 2.7 BT.\r\n") );
	cmd_menu ( );
	
	while (bInLoop)
	{
	    c = swUART_ReadChar( );

	    switch (c)
	    {
		case ':':
	    case '\r':
	        // terminate the msg and reset the msg ptr. then send
	        // it to the handler for processing.
			if (g_isOperating != 0)
			{
				tm_cmd_stop_all(0, NULL);
			}

	        *msg_ptr = '\0';

			if (g_EchoOnOff)
			{
		        printf_P( PSTR("\r\n") );
			}				
	               
			if ( strcmp ((char*)msg, "exit") == 0 )
			{
				bInLoop = false;
				printf_P( PSTR("Goodbye...\r\n"));
				break;
			}									   
			
			strcpy (last_msg, msg);	    
			cmd_parse((char *)msg);
	        msg_ptr = msg;
	        break;
            
	    case '\b':
	        if (msg_ptr > msg)
	        {
	            msg_ptr--;
				swUART_PutChar ( c );
				swUART_PutChar ( ' ' );
				swUART_PutChar ( c );
			}
	        break;
			
           
	    default:
			if (g_EchoOnOff)
				swUART_PutChar ( c );

	        *msg_ptr++ = c;
	        break;
	    }
	}		
}


/**************************************************************************/
/*!
	This is the command prompt shell line printer command    
*/
/**************************************************************************/
void cmd_menu( void )
{
	if ( !g_leftDataToGet && g_EchoOnOff==1)
	{
    	printf_P( PSTR("\r\n"));
    	printf_P( PSTR(COMMAND_PROMPT));
	}
}

/**************************************************************************/
/*!
	echo is off    
*/
/**************************************************************************/
void tm_cmd_echo_off(U8 argc, char **argv)
{
	g_EchoOnOff = 0;
	printf_P (PSTR("OK\r\n"));
}

/**************************************************************************/
/*!
	Entered the transfer mode so stop all the other stuff
*/
/**************************************************************************/
void tm_cmd_stop_all(U8 argc, char **argv)
{
	g_isOperating = 0;

//	TIMSK1 = 0;	// Compare Interrupt
//	TIMSK2 = 0; // Compare interrupt

//	ROW_PORT = 0x00;

	CS_UP;

	printf_P (PSTR("OK\r\n"));
}

/**************************************************************************/
/*!
	Exited the transfer mode so start up all the other stuff
*/
/**************************************************************************/
void tm_cmd_start_all(U8 argc, char **argv)
{
	g_isOperating = 1;

//	TIMSK1 = (1<<OCIE1A);	// Compare Interrupt
//	TIMSK2 = (1<<OCIE2A); 	// Compare interrupt
//	SET_ROW(0);

	CS_UP;
//	g_iCurrentFrameCycle = 0;

	
	printf_P (PSTR("OK\r\n"));
}



/**************************************************************************/
/*!
	echo is on
*/
/**************************************************************************/
void tm_cmd_echo_on(U8 argc, char **argv)
{
	g_EchoOnOff = 1;
	printf_P (PSTR("OK\r\n"));
}

/**************************************************************************/
/*!
	This function parses the command (recieved in void rx()) and tries
	to find a suitable command line in the cmd_table. Then invokes the
	appropriate function given in the cmd_table.    
*/
/**************************************************************************/
void cmd_parse(char *cmd)
{
    U8 argc, i = 0;
    char *argv[30];

    fflush(stdout);

    argv[i] = strtok(cmd, " ");
    do
    {
        argv[++i] = strtok(NULL, " ");
    } while ((i < 30) && (argv[i] != NULL));
    
    argc = i;
    for (i=0; cmd_tbl[i].cmd != NULL; i++)
    {
        if (!strcmp(argv[0], cmd_tbl[i].cmd))
        {
            cmd_tbl[i].func(argc, argv);
            cmd_menu();
            return;
        }
    }
    printf_P( PSTR("CMD: Command not recognized.\r\n"));

    cmd_menu();
}


/**************************************************************************/
/*!
	Print out an identification information   
*/
/**************************************************************************/
void tm_cmd_idn				(U8 argc, char **argv)
{
	char ManID, DevID1, DevID2, ExData;

	printf_P ( PSTR("ANIBIKE VERSION:%03d.%d;"			// The project
					"MEM_SIZE:%05d MBITS;"				// Memory Size
					"CLOCK:%03d MHZ;"					// CPU SIZE
					"#LEDS:%03d;"						// number of leds
					"RGB:%d;" 							// RGB?
					"MEMBLK:%02d KB"					// Flash minimal block
					"\r\n"),
					ANIBIKE_VER,
					ANIBIKE_SUBVER,
					ANIBIKE_MEMORY_SIZE,
					ANIBIKE_CLOCK_SPEED,
					ANIBIKE_NUM_LEDS,
					ANIBIKE_IS_RGB,
					FLASH_BLOCK4_SIZE );

	printf_P ( PSTR("\r\nFlashInfo:\r\n") );
	dataflash_spi_idn ( &ManID, &DevID1, &DevID2, &ExData );

	printf_P ( PSTR("ManID = 0x%x; DevID1 = 0x%x; DevID2 = 0x%x; ExData = 0x%x\r\n"),
					ManID, DevID1, DevID2, ExData );

}

/**************************************************************************/
/*!
	Print-out the soft parameters of the system   
*/
/**************************************************************************/
void tm_cmd_repa			(U8 argc, char **argv)
{
	printf_P ( PSTR("COLOR_DEPTH:%03d;"
					"FILE#BLKS:%02d;"
					"FN_LEN:%03d;"
					"MAX_FRAMES:%03d;\n"), 
				ANIBIKE_COLOR_DEPTH,
				FS_FRAME_BLOCKS,
				FS_FILENAME_LENGTH,
				FS_FILE_MAX_FRAMES);
}

/**************************************************************************/
/*!
	read out data from the flash chip   
*/
/**************************************************************************/
void tm_cmd_read			(U8 argc, char **argv)
{
	if ( argc != 3 )
	{
		printf_P( PSTR("usage: read [start_address] [length]\r\n"));
		return;	
	}
	else
	{
		uint32_t iIterator;
		uint32_t iStart = atol (argv[1]);
		uint32_t iLength = atol (argv[2]);

		printf_P (PSTR("reading: %lu to %lu;\r\n"), iStart, iStart+iLength);

		for (iIterator=0; iIterator<=iLength; iIterator ++ )
			printf_P( PSTR("A%lu:   0x%x\r\n"), (iStart+iIterator), 
									dataflash_read (iStart+iIterator) );
	}
}

/**************************************************************************/
/*!
	write stuff to the flash   
*/
/**************************************************************************/
void tm_cmd_write			(U8 argc, char **argv)
{
	if ( argc != 3 )
	{
		printf_P (PSTR("usage: write [32bit start_address] [8bit length]\r\n"));
		return;	
	}
	else
	{
		g_dataStartAddress = atol(argv[1]);
		g_leftDataToGet = atoi(argv[2]);

		if ( g_leftDataToGet > 15 ) g_leftDataToGet = 15;	// Don't allow more then 15 bytes

		printf_P (PSTR("writing: %lu to %lu;\r\n"), g_dataStartAddress, 
											g_dataStartAddress+g_leftDataToGet);

//		for (iIterator=0; iIterator<=iLength; iIterator ++ )
//			dataflash_write ( iStart+iIterator, swUART_ReadChar( ) );
	}
}


/**************************************************************************/
/*!
	write a whole (or part) of a block in the flash   
*/
/**************************************************************************/
void tm_cmd_write_block		(U8 argc, char **argv)
{
	if ( argc != 4 )
	{
		printf_P( PSTR("usage: write_block [start_block] [quant] [length]\r\n"));
		return;	
	}
	else
	{
		unsigned char rawData[64];	// max number of bytes inside one quant
		unsigned char *dataIt = NULL;
		uint8_t count, i;
		uint32_t msgCount;
		uint8_t currQuant;
		uint32_t iPlace;
		uint16_t crc16 = 0xffff;
		
		uint32_t iStartAddress = atol(argv[1])*FLASH_BLOCK4_SIZE;
		uint8_t iQuant = atoi(argv[2]);
		uint32_t iLength = atol(argv[3]);
		iPlace = iStartAddress;
		
		msgCount = iLength;
		
		// Send the OK that says we are ready to get data chunks
		printf_P (PSTR("OK\r\n"));
		
		while (msgCount)
		{
			currQuant = (iQuant>msgCount)?msgCount:iQuant;
			count = currQuant + 2;		// added 2 bytes of crc16
			dataIt = rawData;
			
			while (count--) *dataIt++=getchar( );
			
			// calculate the crc16 of the just received data
			crc16 = 0xffff;
			for (i=0; i<currQuant; i++)
				crc16 = _crc16_update(crc16, rawData[i]);
			
			// check success	
			if ((crc16&0xff)==rawData[currQuant] && ((crc16>>8)&0xff)==rawData[currQuant+1])
			{
				// write those stuff to the flash memory and post success message
				for (i=0; i<currQuant; i++)	dataflash_write	( iPlace+i, rawData[i] );
				iPlace += iQuant;
				msgCount -= iQuant;
				printf_P (PSTR("OK\r\n"));
			}
			else
			{
				printf_P (PSTR("FAIL\r\n"));
			}
		}
	}
}

/**************************************************************************/
/*!
	read the whole block from the flash   
*/
/**************************************************************************/
void tm_cmd_read_block		(U8 argc, char **argv)
{
	if ( argc != 2 )
	{
		printf_P( PSTR("usage: read_block [start_block]\r\n"));
		return;	
	}
	else
	{
		uint32_t iIterator = 0;
		
		g_dataStartAddress = atol(argv[1])*FLASH_BLOCK4_SIZE;
	
		for (iIterator=0; iIterator<=FLASH_BLOCK4_SIZE; iIterator ++ )
			putchar(dataflash_read ( g_dataStartAddress+iIterator ));
	}
}


/**************************************************************************/
/*!
	Pull down CS pin   
*/
/**************************************************************************/
void tm_cmd_cs_low			(U8 argc, char **argv)
{
	CS_DOWN;
	printf_P (PSTR("OK\r\n"));
}

/**************************************************************************/
/*!
	Pull up CS pin   
*/
/**************************************************************************/
void tm_cmd_cs_high			(U8 argc, char **argv)
{
	CS_UP;
	printf_P (PSTR("OK\r\n"));
}

/**************************************************************************/
/*!
	Send an spi commant to the flash memory   
*/
/**************************************************************************/
void tm_cmd_spi_transfer	(U8 argc, char **argv)
{
	if ( argc != 2)
	{
		printf_P( PSTR("usage: spi_trans [byte]\r\n"));
		return;		
	}
	else
	{
		U8 byte_sent;
		U8 byte_rec;

		byte_sent = atoi (argv[1]);	

		printf_P( PSTR("\r\nSending 0x%x"), byte_sent );
		byte_rec = SPI_MasterTransceiveByte(&spiMasterD, byte_sent);

		printf_P( PSTR("\r\nRecieved 0x%x\r\n"), byte_rec );
	}
}

/**************************************************************************/
/*!
	Write-UnProtect the flash memory    
*/
/**************************************************************************/
void tm_cmd_unprotect_flash	(U8 argc, char **argv)
{
	if ( argc != 2 )
	{
		printf_P( PSTR("usage: unprot_flash [<block number> or <ALL>]\r\n"));
		return;		
	}
	else
	{
		if ( strcmp(argv[1], "ALL") == 0)
		{
			dataflash_unprotect_all ( );
		}
		else
		{
			uint32_t iBlock;
			uint32_t iAddress;

			iBlock = atol (argv[1]);						// This is the data

			// Calculate address
			iAddress = iBlock * (uint32_t)(FLASH_BLOCK4_SIZE);

			dataflash_unprotect_block4 ( iAddress );
		}
	}
	printf_P (PSTR("OK\r\n"));
}


/**************************************************************************/
/*!
	Write-Protect the flash memory   
*/
/**************************************************************************/
void tm_cmd_protect_flash	(U8 argc, char **argv)
{
	if ( argc != 2 )
	{
		printf_P( PSTR("usage: prot_flash [<block number> or <ALL>]\r\n"));
		return;		
	}
	else
	{
		if ( strcmp(argv[1], "ALL") == 0)
		{
			dataflash_protect_all ( );
		}
		else
		{
			uint32_t iBlock;
			uint32_t iAddress;

			iBlock = atol (argv[1]);						// This is the data

			// Calculate address
			iAddress = iBlock * (uint32_t)(FLASH_BLOCK4_SIZE);

			dataflash_protect_block4 ( iAddress );
		}
	}
	printf_P (PSTR("OK\r\n"));
}

/**************************************************************************/
/*!
	Erase the flash chip - per block of ALL   
*/
/**************************************************************************/
void tm_cmd_erase_flash		(U8 argc, char **argv)
{
	if ( argc != 2 )
	{
		printf_P( PSTR("usage: erase_flash [<block number> or <ALL>]\r\n"));
		return;		
	}
	else
	{
		if ( strcmp(argv[1], "ALL") == 0)
		{
			dataflash_erase_all ( );
		}
		else
		{
			uint32_t iBlock;

			iBlock = atol (argv[1]);						// This is the data
			
			dataflash_unprotect_block4 ( (uint16_t)(iBlock) );

			dataflash_erase_block4 ( (uint16_t)(iBlock) );
		}
	}
	printf_P (PSTR("OK\r\n"));
}

/**************************************************************************/
/*!
	Retrieve the flash statuses and print out on the shell   
*/
/**************************************************************************/
void tm_cmd_flash_status	(U8 argc, char **argv)
{
	uint8_t stat_flash = dataflash_read_status ( );

	if (stat_flash&FLASH_READY)
		printf_P( PSTR("\r\n#0 NOT ready (1)") );
	else
		printf_P( PSTR("\r\n#0 ready (0)") );

	if (stat_flash&FLASH_WE)
		printf_P( PSTR("\r\n#1 write enabled (1)") );
	else
		printf_P( PSTR("\r\n#1 NOT write enabled (0)") );


	if ((stat_flash&FLASH_SOFTWARE_PROTECT)==0x1100)
		printf_P( PSTR("\r\n#23 write protected (11)") );
	else if ((stat_flash&FLASH_SOFTWARE_PROTECT)==0x0100)
		printf_P( PSTR("\r\n#23 partly write protected (01)") );
	else if ((stat_flash&FLASH_SOFTWARE_PROTECT)==0x0000)
		printf_P( PSTR("\r\n#23 NOT write protected (00)") );
	else
		printf_P( PSTR("\r\n#23 protection unknown (10 reserved)") );

	if (stat_flash&FLASH_WP_PIN)
		printf_P( PSTR("\r\n#4 NOT write protected (1)") );
	else
		printf_P( PSTR("\r\n#4 write protected (0)") );

	if (stat_flash&FLASH_ERASE_WRITE_ERR)
		printf_P( PSTR("\r\n#5 failed program (erase) (1)") );
	else
		printf_P( PSTR("\r\n#5 program (erase) was successfull (0)") );

	if (stat_flash&FLASH_SEC_PROTECT_VEC_LOCK)
		printf_P( PSTR("\r\n#7 sector protection registers LOCKED (1)\r\n\r\n") );
	else
		printf_P(PSTR("\r\n#7 sector protection registers UNLOCKED (0)\r\n\r\n") );
}

/**************************************************************************/
/*!
	Show file list in the shell   
*/
/**************************************************************************/
void tm_cmd_dir				(U8 argc, char **argv)
{
	FS_ShowFileInformation ( );
}

/**************************************************************************/
/*!
	Send out the whole files-system boot table
*/
/**************************************************************************/
void tm_cmd_get_fs			(U8 argc, char **argv)
{
	FileEntry_ST sCurEntry;
	char *cEntry = NULL;
	uint32_t iAddress = ENTRY_TABLE_ADDR;
	uint8_t	itr = 0;

	// Try reading entries until the end
	while ( FS_ReadEntry ( &iAddress, &sCurEntry ) )
	{
		cEntry = (char*)(&sCurEntry);
		
		for ( itr = 0; itr < ENTRY_TABLE_EL_SIZE; itr ++ )
			putchar( cEntry[itr] );		
	}

	if ( itr == 0 )		// nothing was read then FS empty
	{
		printf_P(PSTR("EMPTY\r\n") );
	}
}

/**************************************************************************/
/*!
	Test some LEDs with brightness values   
*/
/**************************************************************************/
void tm_cmd_test_led		(U8 argc, char **argv)
{
	if ( argc < 2 )
	{
		printf_P(PSTR("usage: test_led [row#] [RGB] [val]\n"));
		return;	
	}
	else
	{
		uint8_t i;
		uint16_t val = 0;
		
		MUX_ENABLE;
		
		// Set the row num
		MUX_SET_ROW (atoi(argv[1]));
		
		val = atol (argv[3]);
			
		for (i = 0; i<strlen(argv[2]); i++)
		{
			if (argv[2][i]=='R')
			{
				RED_PWM_CTRL.CCABUF = val;        
				RED_PWM_CTRL.CCBBUF = val;
				RED_PWM_CTRL.CCCBUF = val;
				RED_PWM_CTRL.CCDBUF = val;
			}

			if (argv[2][i]=='G')
			{
				GREEN_PWM_CTRL.CCABUF = val;        
				GREEN_PWM_CTRL.CCBBUF = val;
				GREEN_PWM_CTRL.CCCBUF = val;
				GREEN_PWM_CTRL.CCDBUF = val;
			}

			if (argv[2][i]=='B')
			{
				BLUE_PWM_CTRL.CCABUF = val;        
				BLUE_PWM_CTRL.CCBBUF = val;
				BLUE_PWM_CTRL.CCCBUF = val;
				BLUE_PWM_CTRL.CCDBUF = val;
			}
		}
	} 
}

/**************************************************************************/
/*!
	Test the flash memory integrity (chip and communication)   
*/
/**************************************************************************/
void tm_cmd_test_memory		(U8 argc, char **argv)
{
	uint32_t ErrCounter = 0;
	uint32_t pos = 0;	
	uint8_t retVal = 0, Val = 0;

	// Erase memory
	printf_P ( PSTR("Deleting all information from flash...\r\n") );
	
	dataflash_erase_all ( );

	printf_P ( PSTR("Unprotecting entire flash...\r\n") );

	dataflash_unprotect_all ( );

#ifndef __NOSLEEP_NOSPI_DEBUG
	_delay_ms(4000);
#endif

	printf_P ( PSTR("Writing testing sequence...\r\n") );
	for (pos = 0; pos < FS_MEMORY_SIZE; pos ++)
	{
		Val = pos&0xff;

		if ( (pos & 0xFFF) == 0)	// Every 4096
			printf_P ( PSTR("%lu "), pos);

		dataflash_write	( pos, Val );
	}

#ifndef __NOSLEEP_NOSPI_DEBUG
	_delay_ms(10000);
#endif
	printf_P ( PSTR("Reading the testing sequence..\r\n") );

	for (pos = 0; pos < FS_MEMORY_SIZE; pos ++)
	{
		retVal = dataflash_read	( pos );
		Val = pos&0xff;

		if ( (pos & 0xFFF) == 0)	// Every 4096
			printf_P ( PSTR("%lu "), pos);
		
		if (retVal!=Val)
			ErrCounter ++;
	}

	printf_P(PSTR("Number of errors: %lu\r\n"), ErrCounter);
}

/**************************************************************************/
/*!
	This function shows the help menu to the user
*/
/**************************************************************************/
void tm_cmd_help			(U8 argc, char **argv)
{
	if (argc > 2)
	{
		printf_P( PSTR("usage: help [cmd]\r\n"));
		return;	
	}
	else if (argc == 2)
	{
		U8 i = 0;
		while (	cmd_tbl[i].cmd != NULL && 
				strcmp(cmd_tbl[i].cmd, argv[1]) != 0 ) 
			{ i++; }
		
		if ( cmd_tbl[i].cmd != NULL )
		{
			printf_P( PSTR("usage: %s %s\r\n"), cmd_tbl[i].cmd, 
					((strlen(cmd_tbl[i].usage)==0)?"no parameters":cmd_tbl[i].usage));	
		}	
	}
	else
	{
		U8 i = 0;
		while (	cmd_tbl[i].cmd != NULL ) 
		{ 
			printf_P( PSTR("%s: %s\r\n"), cmd_tbl[i].cmd, 
				((strlen(cmd_tbl[i].usage)==0)?"no parameters":cmd_tbl[i].usage));
			i++; 
		}
	}
}

/**************************************************************************/
/*!
	Resets the microcontroller   
*/
/**************************************************************************/
void tm_cmd_software_reset	(U8 argc, char **argv)
{
	CPU_CCP=CCP_IOREG_gc;
	RST.CTRL=RST_SWRST_bm;
}

/**************************************************************************/
/*!
	Send some AT command to the BT module   
*/
/**************************************************************************/
/*void tm_cmd_send_AT_command_bt (U8 argc, char **argv)
{
	if ( argc != 2 )
	{
		printf_P( PSTR("usage: at_command [command - no spaces]\r\n"));
		return;		
	}
	else
	{
		printf_P( PSTR("Enterring AT mode...\r\n"));
		PORTA.DIRSET = PIN3_bm;
		PORTA.OUTSET = PIN3_bm;
		_delay_ms(1000);
		
		printf_P( PSTR("%s\r\n"), argv[1]);
		if (tm_get_new_line ( sTemp ))
		{
			if ( strstr(sTemp, "OK") )
				iTestCommandPass = 1;
		}
		
		PORTA.OUTCLR = PIN3_bm;	
		printf_P( PSTR("Exiting AT mode. Result:\r\n"0);
	}
	printf_P (PSTR("OK\r\n"));	
}*/


/**************************************************************************/
/*!
	Configure the BT module for ANIBIKE  
*/
/**************************************************************************/
void tm_cmd_configure_bt (U8 argc, char **argv)
{
	uint8_t iTestCommandPass = 0;
	uint8_t iNameChanged = 0;
	uint8_t iUARTChanged = 0;
	char sVersion[16] = {0};
	char sTemp[32] = {0};
	char *cTok = NULL;
	
	printf_P( PSTR("Enterring AT mode...\r\n"));
	PORTA.DIRSET = PIN3_bm;
	PORTA.OUTSET = PIN3_bm;
	
	_delay_ms(1000);
	
	printf_P( PSTR("AT\r\n"));
	if ( gets(sTemp) )
	{
		if ( strstr(sTemp, "OK") )
			iTestCommandPass = 1;
	}
	
	printf_P( PSTR("AT+VERSION?\r\n"));
	if ( gets(sTemp) )
	{
		cTok = strtok ( sTemp, ":\n" );
		cTok = strtok ( NULL, ":\n" );
		strcpy (sVersion, cTok);
	}
	
	printf_P( PSTR("AT+NAME=ANIBIKE_2V7_BT\r\n"));
	if ( gets(sTemp) )
	{
		if ( strstr(sTemp, "OK") )
			iNameChanged = 1;
	}	
	
/*	printf_P( PSTR("AT+UART=460800,0,0\r\n"));
	if ( gets(sTemp) )
	{
		if ( strstr(sTemp, "OK") )
			iUARTChanged = 1;
	}	*/
	
	PORTA.OUTCLR = PIN3_bm;	
	printf_P( PSTR("Exiting AT mode... Configuration complete.\r\n"));
	printf_P( PSTR("Bluetooth info:\r\n  Status: %d\r\n  Version: %s\r\n  Name change: %d\r\n"), iTestCommandPass, sVersion, iNameChanged);
}

/**************************************************************************/
/*!
	Read the calibration values for the LEDs (max counter number)  
*/
/**************************************************************************/
void tm_cmd_read_led_cal	(U8 argc, char **argv)
{
	read_period_calibrations ( &g_iRedCalibrationPeriod, &g_iGreenCalibrationPeriod, &g_iBlueCalibrationPeriod );
	
	printf_P (PSTR("Red: %u;  Green: %u;  Blue: %u; \r\n"), g_iRedCalibrationPeriod,
															g_iGreenCalibrationPeriod,
															g_iBlueCalibrationPeriod);
}

/**************************************************************************/
/*!
	Store the calibration values for the LEDs (max counter number)  
*/
/**************************************************************************/
void tm_cmd_write_led_cal (U8 argc, char **argv)
{
	if (argc != 4)
	{
		printf_P( PSTR("usage: write_cal red16 green16 blue16\r\n"));
		return;	
	}
	g_iRedCalibrationPeriod = atol(argv[1]);
	g_iGreenCalibrationPeriod = atol(argv[2]);
	g_iBlueCalibrationPeriod = atol(argv[3]);
	
	write_period_calibrations ( g_iRedCalibrationPeriod, g_iGreenCalibrationPeriod, g_iBlueCalibrationPeriod );
	
	TC_SetPeriod(&GREEN_PWM_CTRL, g_iGreenCalibrationPeriod);
	TC_SetPeriod(&RED_PWM_CTRL, g_iRedCalibrationPeriod);
	TC_SetPeriod(&BLUE_PWM_CTRL, g_iBlueCalibrationPeriod);
}

/**************************************************************************/
/*!
	Send data to Secondary ANIBIKE Board and get the response  
*/
/**************************************************************************/
void tm_cmd_write_dl		(U8 argc, char **argv)
{
	uint8_t result = anibike_dl_send_data ((uint8_t*)((void*)(argv[1])), strlen(argv[1]));	
	
	switch (result)
	{
		case 0: printf_P( PSTR("transaction successful (ACKed)\r\n")); break;
		case 1: printf_P( PSTR("no slave detected\r\n")); break;
		case 2: printf_P( PSTR("slave does not respond\r\n")); break;
		case 3: printf_P( PSTR("received NACK\r\n")); break;
		default: break;
	};
}