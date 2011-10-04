#include "AniBike_Internal.h"
#include "AniBike2_DataFlash.h"
#include "AniBike2_Usart_Comm.h"
#include "AniBike2_FileSystem.h"
#include <avr/wdt.h>

//__________________________________________________________________________________________________
// File System - Stop reading from memory
void TM_EchoCommand ( char *sIncomingCommand )
{
	#ifdef  __ENABLE_ECHO_UART
	USARTWriteString ( sIncomingCommand );
	#endif
}

//__________________________________________________________________________________________________
// File System - Stop reading from memory
void TM_Identify ( void )
{
	char ManID, DevID1, DevID2, ExData;
	char sRespondData[UART_RESPOND_MAX_LENGTH];

	sprintf ( sRespondData, "ANIBIKE VER:%03d.00;"	// The project
							"MSZ:%05d MBITS;"		// Memory Size
							"CPS:%03d MHZ;"			// CPU SIZE
							"NLED:%03d;"			// number of leds
							"RGB:%d;\r\n", 			// RGB?
							ANIBIKE_VER,
							ANIBIKE_MEMORY_SIZE,
							ANIBIKE_CPU_SPEED,
							ANIBIKE_NUM_LEDS,
							ANIBIKE_IS_RGB );
	USARTWriteString ( sRespondData );

	USARTWriteString ( (char*)"\r\nFlashInfo:\r\n" );
	dataflash_spi_idn ( &ManID, &DevID1, &DevID2, &ExData );

	sprintf ( sRespondData, "ManID = 0x%x; DevID1 = 0x%x; DevID2 = 0x%x; ExData = 0x%x\r\n",
					ManID, DevID1, DevID2, ExData );
	USARTWriteString ( sRespondData );
	USARTWriteString ( (char*)"\r\n" );
}

//__________________________________________________________________________________________________
// File System - Show Parameters
void TM_ShowParameters ( void )
{
	char sRespondData[UART_RESPOND_MAX_LENGTH];

	sprintf ( sRespondData, "CDEP:%03d;"		//color depth
							"ANG:%03d;\r\n", 		//angular res
							ANIBIKE_COLOR_DEPTH,
							ANGULAR_RESOLUTION );
	USARTWriteString ( sRespondData );
}

//__________________________________________________________________________________________________
// File System - Read Memory
void TM_ReadMemory ( char *sIncomingCommand )
{
	char sRespondData[UART_RESPOND_MAX_LENGTH];
	char* cTemp;
	char HexaRead[50];
	uint32_t iIterator;
	uint32_t iStart;
	uint32_t iLength;

	cTemp = strtok ( sIncomingCommand, " " ); // The first one is "READ"
	cTemp = strtok ( NULL, " " );
	iStart = atol (cTemp);
	cTemp = strtok ( NULL, " " );
	iLength = atol (cTemp);

	if (cTemp!=NULL)
	{
		sprintf ( sRespondData, "Reading %lu to %lu;\r\n",
								iStart, iStart+iLength );
		USARTWriteString ( sRespondData );

		for (iIterator=0; iIterator<=iLength; iIterator ++ )
		{
			sprintf(HexaRead, "A%lu:   0x%x\r\n", (iStart+iIterator), dataflash_read (iStart+iIterator) );
			USARTWriteString ( HexaRead );
		}

		USARTWriteString ( (char*)"\r\n" );
	}
	else USARTWriteString ( (char*)"USAGE:   READ [START ADDRESS] [BYTE COUNT]\r\n" );
}


//__________________________________________________________________________________________________
// File System - Write Memory
void TM_WriteMemory ( char *sIncomingCommand )
{
	uint32_t iIterator;
	char* cTemp;
	uint32_t iStart;
	uint32_t iLength;

	cTemp = strtok ( sIncomingCommand, " " ); // The first one is "WRIT"
	cTemp = strtok ( NULL, " " );
	iStart = atol (cTemp);
	cTemp = strtok ( NULL, " " );
	iLength = atol (cTemp);

	if (cTemp!=NULL)
	{
		//sprintf ( sRespondData, "Writing %d-%d",
		//						iStart, iStart+iLength );
		//USARTWriteString ( sRespondData );

		for (iIterator=0; iIterator<=iLength; iIterator ++ )
		{
			dataflash_write ( iStart+iIterator, USARTReadChar() );
		}
	}
	else USARTWriteString ( (char*)"USAGE:   WRIT [START ADDRESS] [BYTE COUNT]\r\n" );
}

//__________________________________________________________________________________________________
// File System - Spi Read/Write
void TM_Spi ( char *sIncomingCommand )
{
	char* cTemp;
	uint8_t Spi_t;
	char HexaRead[40];
	char data;

	cTemp = strtok ( sIncomingCommand, " " ); // The first one is "SPI"
	cTemp = strtok ( NULL, " " );
	Spi_t = atoi (cTemp);						// This is the data


	if (cTemp!=NULL)
	{
		sprintf(HexaRead, "\r\nTransferring 0x%x", Spi_t );
		USARTWriteString ( HexaRead );

		data = spi_transfer(Spi_t);

		sprintf(HexaRead, "\r\nRecieved 0x%x\r\n", data );
		USARTWriteString ( HexaRead );

	}
	else USARTWriteString ( (char*)"USAGE:   SPI [DATA_8BIT]\r\n" );
}

//__________________________________________________________________________________________________
// File System - Read flash status
void TM_FlashStatus ( char *sIncomingCommand )
{
	uint8_t stat_flash = dataflash_read_status ( );

	if (stat_flash&FLASH_READY)
		USARTWriteString ( (char*)"\r\n#0 NOT ready (1)" );
	else
		USARTWriteString ( (char*)"\r\n#0 ready (0)" );

	if (stat_flash&FLASH_WE)
		USARTWriteString ( (char*)"\r\n#1 write enabled (1)" );
	else
		USARTWriteString ( (char*)"\r\n#1 NOT write enabled (0)" );


	if ((stat_flash&FLASH_SOFTWARE_PROTECT)==0x1100)
		USARTWriteString ( (char*)"\r\n#23 write protected (11)" );
	else if ((stat_flash&FLASH_SOFTWARE_PROTECT)==0x0100)
		USARTWriteString ( (char*)"\r\n#23 partly write protected (01)" );
	else if ((stat_flash&FLASH_SOFTWARE_PROTECT)==0x0000)
		USARTWriteString ( (char*)"\r\n#23 NOT write protected (00)" );
	else
		USARTWriteString ( (char*)"\r\n#23 protection unknown (10 reserved)" );

	if (stat_flash&FLASH_WP_PIN)
		USARTWriteString ( (char*)"\r\n#4 NOT write protected (1)" );
	else
		USARTWriteString ( (char*)"\r\n#4 write protected (0)" );

	if (stat_flash&FLASH_ERASE_WRITE_ERR)
		USARTWriteString ( (char*)"\r\n#5 failed program (erase) (1)" );
	else
		USARTWriteString ( (char*)"\r\n#5 program (erase) was successfull (0)" );

	if (stat_flash&FLASH_SEC_PROTECT_VEC_LOCK)
		USARTWriteString ( (char*)"\r\n#7 sector protection registers LOCKED (1)\r\n\r\n" );
	else
		USARTWriteString ( (char*)"\r\n#7 sector protection registers UNLOCKED (0)\r\n\r\n" );

}

//__________________________________________________________________________________________________
// File System - Fetch New Command
void TM_FetchCommand ( char *sIncomingCommand )
{
	int iPlace;
	char data;

	USARTWriteString ( (char*)">>" );

	// _____________________________________________________________
	// Fetch New Command
	iPlace = 0;
	data = toupper ( USARTReadChar() );
	USARTWriteChar ( data );
	while (data != ':' && iPlace != UART_COMMAND_MAX_LENGTH-1)
	{
		sIncomingCommand[iPlace++] = data;
		data=toupper ( USARTReadChar() );
		USARTWriteChar ( data );
	}

	sIncomingCommand [iPlace] = '\0';
	USARTWriteString ( (char*)"\r\n" );

}

//__________________________________________________________________________________________________
// File System - Write blocks to the flash memory
// This function gets a command as shown below:
// 		WRBL [Starting Block Number - uint16] [number of data-bytes to write]
// The starting address is calculated from the Starting Block. The channel
// is open to reading according to the Number of data-bytes length
void TM_WriteBlocks ( char *sIncomingCommand )
{
	uint32_t iIterator;
	char *cTemp;
	uint32_t iStartBlockNumber;
	uint32_t iLengthInBytes;
	uint32_t iStartAddress = 0;
//	char Temp;
//	uint32_t iTemp;
	
/////
//char HexaRead[40];

	cTemp = strtok ( sIncomingCommand, " " ); // The first one is "WRBL"
	cTemp = strtok ( NULL, " " );
	iStartBlockNumber = atol (cTemp);
	cTemp = strtok ( NULL, " " );
	iLengthInBytes = atol (cTemp);

	USARTWriteString ( (char*)"WRBL\r\n" );

	if (cTemp!=NULL)
	{
		// Calculate the address - every block is defined as 3 Flash Blocks
		// Which means 3*4KByte*iStart
		iStartAddress = (uint32_t)(FS_FRAME_CAP) * iStartBlockNumber;

/////
//sprintf(HexaRead, "A%lu\r\n", iStartAddress );
//USARTWriteString ( HexaRead );

		// Unprotect the appropriate blocks (3 blocks)
		dataflash_unprotect_block4 ( iStartBlockNumber * 3 );
		_delay_ms(100);
		dataflash_unprotect_block4 ( iStartBlockNumber * 3 + 1 );
		_delay_ms(100);
		dataflash_unprotect_block4 ( iStartBlockNumber * 3 + 2 );
		_delay_ms(100);

		// Erase the appropriate blocks (3 blocks)
		dataflash_erase_block4 ( iStartBlockNumber * 3 );
		_delay_ms(100);
		dataflash_erase_block4 ( iStartBlockNumber * 3 + 1 );
		_delay_ms(100);
		dataflash_erase_block4 ( iStartBlockNumber * 3 + 2 );
		_delay_ms(100);



		for (iIterator=0; iIterator<=iLengthInBytes; iIterator ++ )
		{
			dataflash_write ( iStartAddress+iIterator, USARTReadChar() );
		}
	}
	else USARTWriteString ( (char*)"USAGE:   WRBL [START BLOCK] [BYTE COUNT]\r\n" );

}

//__________________________________________________________________________________________________
// File System - Unprotect bocks
void TM_unprotect ( char *sIncomingCommand )
{
	char* cTemp;
	uint32_t iBlock;
	uint32_t iAddress;
//	char HexaRead[40];

	cTemp = strtok ( sIncomingCommand, " " ); // The first one is the command
	cTemp = strtok ( NULL, " " );

	if ( cTemp != NULL )
	{
		if ( !strcmp (cTemp, "ALL") )
		{
			dataflash_unprotect_all (  );
		}
		else
		{
			iBlock = atol (cTemp);						// This is the data

			// Calculate address
			iAddress = iBlock * (uint32_t)(FLASH_BLOCK4_SIZE);

//			sprintf( HexaRead, "Uprotecting addr:%lu\r\n", iAddress );
//			USARTWriteString ( HexaRead );

			dataflash_unprotect_block4 ( iAddress );

		}
	}
	else USARTWriteString ( (char*)"USAGE:   UPRO [BLOCK# or ALL]\r\n" );


}

//__________________________________________________________________________________________________
// File System - Protect Blocks
void TM_protect ( char *sIncomingCommand )
{
	char* cTemp;
	uint32_t iBlock;
	uint32_t iAddress;

	cTemp = strtok ( sIncomingCommand, " " ); // The first one is the command
	cTemp = strtok ( NULL, " " );

	if ( cTemp != NULL )
	{
		if ( !strcmp (cTemp, "ALL") )
		{
			dataflash_protect_all (  );
		}
		else
		{
			iBlock = atol (cTemp);						// This is the data

			// Calculate address
			iAddress = iBlock * (uint32_t)(FLASH_BLOCK4_SIZE);

			dataflash_protect_block4 ( iAddress );

		}
	}
	else USARTWriteString ( (char*)"USAGE:   PROT [BLOCK# or ALL]\r\n" );

}

//__________________________________________________________________________________________________
// File System - Erase Blocks
void TM_erase ( char *sIncomingCommand )
{
	char* cTemp;
	uint32_t iBlock;

	cTemp = strtok ( sIncomingCommand, " " ); // The first one is the command
	cTemp = strtok ( NULL, " " );

	if ( cTemp != NULL )
	{
		if ( !strcmp (cTemp, "ALL") )
		{
			dataflash_erase_all ( );
		}
		else
		{
			iBlock = atol (cTemp);						// This is the data
			
			dataflash_unprotect_block4 ( (uint16_t)(iBlock) );

			dataflash_erase_block4 ( (uint16_t)(iBlock) );
		}
	}
	else USARTWriteString ( (char*)"USAGE:   FLER [BLOCK# or ALL]\r\n" );

}

//__________________________________________________________________________________________________
// Testing LEDs
void TM_TestLEDs (  char *sIncomingCommand )
{
	uint8_t iRowNum = 0;
	uint8_t iLastRowPort = ROW_PORT;
	uint8_t i;
	char* cTemp;

	cTemp = strtok ( sIncomingCommand, " " ); // The first one is "WRIT"
	cTemp = strtok ( NULL, " " );
	iRowNum = atoi (cTemp);
	cTemp = strtok ( NULL, " " );
	
	// Now the cTemp contains "RGB" or "RG" or "R" or "G" and so on...

	// Set the row num
	ROW_PORT = (1<<iRowNum);
	
	for (i = 0; i<strlen(cTemp); i++)
	{
		if (cTemp[i]=='R')
		{
			DDRB |= 0xF0;
			DDRF |= 0xF0;
		}

		if (cTemp[i]=='G')
		{
			DDRD |= 0b00111100;
			DDRC |= 0x0F;
		}

		if (cTemp[i]=='B')
		{
			DDRF |= 0x0F;
			DDRC |= 0xF0;
		}
	}

	_delay_ms(1500);

	DDRB &= 0x0F;
	DDRC = 0;
	DDRD &= 0b11000011;
	DDRF = 0;
	
	// Cleanup
	ROW_PORT = iLastRowPort;
}

//__________________________________________________________________________________________________
// File System - Perform memory check
void TM_MemoryTest ( void )
{
	char Msg[70];
	uint32_t ErrCounter = 0;
	uint32_t pos = 0;	
	uint8_t retVal = 0, Val = 0;

	// Erase memory
	USARTWriteString ( (char*)"Delete\r\n" );
	
	dataflash_erase_all ( );

	USARTWriteString ( (char*)"Unprotect\r\n" );

	dataflash_unprotect_all ( );

#ifndef __NOSLEEP_NOSPI_DEBUG
	_delay_ms(4000);
#endif

	USARTWriteString ( (char*)"Writing\r\n" );
	for (pos = 0; pos < FS_MEMORY_SIZE; pos ++)
	{
		Val = pos&0xff;

		if ( (pos & 0xFFF) == 0)	// Every 4096
		{
			sprintf ( Msg, "%lu ", pos);
			USARTWriteString ( Msg );
		}

		dataflash_write	( pos, Val );
	}

#ifndef __NOSLEEP_NOSPI_DEBUG
	_delay_ms(15000);
#endif
	USARTWriteString ( (char*)"Reading\r\n" );

	for (pos = 0; pos < FS_MEMORY_SIZE; pos ++)
	{
		retVal = dataflash_read	( pos );
		Val = pos&0xff;

		if ( (pos & 0xFFF) == 0)	// Every 4096
		{
			sprintf ( Msg, "%lu ", pos);
			USARTWriteString ( Msg );
		}

		if (retVal!=Val)
			ErrCounter ++;
	}

	sprintf (Msg, "#Errors: %lu\r\n", ErrCounter);
	USARTWriteString (Msg);
	
}

//__________________________________________________________________________________________________
// File System - Write blocks to the flash memory
void TM_TransferFileSystem ( void )
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
			USARTWriteChar( cEntry[itr] );		
	}

	if ( itr == 0 )		// nothing was read then FS empty
	{
		USARTWriteString ( (char*)"EMPTY\r\n" );
	}
}

//==============================================================================================
// This function is invoked by the USART RX interrupt
void DataTransferMode ( )
{
	char sIncomingCommand[UART_COMMAND_MAX_LENGTH];

	USARTWriteString ( (char*)"ANIBIKE INC. VER. 2.00.\r\n");
	
	while (1)
	{
		// Fetch new command from command prompt
		TM_FetchCommand ( sIncomingCommand );
		// Echo command
		TM_EchoCommand ( sIncomingCommand );

		// _____________________________________________________________
		// PARSE COMMANDS
		// _____________________________________________________________


		// _____________________________________________________________
		// Read identification data
		#ifdef  __ENABLE_IDN_UART
		// IDENTIFICATION AND INFORMATION
		if ( strcmp (sIncomingCommand, "IDN?") == 0 )
			TM_Identify ( );
		#endif

		// _____________________________________________________________
		// READ PARAMETERS
		#ifdef  __ENABLE_READ_PARAM_UART
		// READ THE PARAMETERS FROM MEMORY
		if ( strcmp (sIncomingCommand, "REPA") == 0 )
			TM_ShowParameters (  );
		#endif

		// _____________________________________________________________
		// READ MEMORY
		#ifdef  __ENABLE_READ_UART
		if ( sIncomingCommand[0] == 'R' && sIncomingCommand[1] == 'E' &&
			 sIncomingCommand[2] == 'A' && sIncomingCommand[3] == 'D' )
			TM_ReadMemory ( sIncomingCommand );
		#endif
		
		#ifdef  __ENABLE_DIRECT_WRITE_UART		
		// _____________________________________________________________		
		// WRITE MEMORY
		if ( sIncomingCommand[0] == 'W' && sIncomingCommand[1] == 'R' &&
			 sIncomingCommand[2] == 'I' && sIncomingCommand[3] == 'T' )
			TM_WriteMemory ( sIncomingCommand );
		#endif

		// _____________________________________________________________
		// WRITE BLOCKS
		if ( sIncomingCommand[0] == 'W' && sIncomingCommand[1] == 'R' &&
			 sIncomingCommand[2] == 'B' && sIncomingCommand[3] == 'L' )
			//USARTWriteString ( (char*)"WRBL\r\n" );
			TM_WriteBlocks ( sIncomingCommand );

		// _____________________________________________________________
		// CS LOW
		if ( strcmp (sIncomingCommand, "CSLO") == 0 )
		{	CS_DOWN;	}

		// _____________________________________________________________
		// CS HIGH
		if ( strcmp (sIncomingCommand, "CSHI") == 0 )
		{	CS_UP;		}

		// _____________________________________________________________
		// SPI Transfer
		if ( sIncomingCommand[0] == 'S' && sIncomingCommand[1] == 'P' &&
			 sIncomingCommand[2] == 'I' )
			TM_Spi ( sIncomingCommand );


		// _____________________________________________________________
		// Unprotect flash
		if ( sIncomingCommand[0] == 'U' && sIncomingCommand[1] == 'P' &&
			 sIncomingCommand[2] == 'R' && sIncomingCommand[3] == 'O')
			TM_unprotect ( sIncomingCommand );


		// _____________________________________________________________
		// Protect flash
		if ( sIncomingCommand[0] == 'P' && sIncomingCommand[1] == 'R' &&
			 sIncomingCommand[2] == 'O' && sIncomingCommand[3] == 'T')
			TM_protect ( sIncomingCommand );

		// _____________________________________________________________
		// Erase flash
		if ( sIncomingCommand[0] == 'F' && sIncomingCommand[1] == 'L' &&
			 sIncomingCommand[2] == 'E' && sIncomingCommand[3] == 'R')
			TM_erase ( sIncomingCommand );


		// _____________________________________________________________
		// Show flash status
		if ( sIncomingCommand[0] == 'F' && sIncomingCommand[1] == 'L' &&
			 sIncomingCommand[2] == 'S' && sIncomingCommand[3] == 'T')
			TM_FlashStatus ( sIncomingCommand );



		
		// _____________________________________________________________
		// DIR 
		if ( strcmp (sIncomingCommand, "DIR") == 0 )
		{	FS_ShowFileInformation ( );		}

		
		// _____________________________________________________________
		// GET FS 
		if ( strcmp (sIncomingCommand, "GTFS") == 0 )
		{	TM_TransferFileSystem ( );		}


		// _____________________________________________________________
		// LEDS TEST
		if ( sIncomingCommand[0] == 'T' && sIncomingCommand[1] == 'L' &&
			 sIncomingCommand[2] == 'E' && sIncomingCommand[3] == 'D' )
			TM_TestLEDs ( sIncomingCommand );


		// _____________________________________________________________
		// MTST 
		if ( strcmp (sIncomingCommand, "MTST") == 0 )
		{	
			USARTWriteString ( (char*)"Testing Flash Memory: \r\n" );
			TM_MemoryTest ( );		
		}
		


		#ifdef  __ENABLE_EXIT_UART
		// _____________________________________________________________
		// EXIT TRANSFER MODE
		if ( strcmp (sIncomingCommand, "EXIT") == 0 )
		{ 
			USARTWriteString ( (char*)"ANIBIKE EXITING TRANSFER MODE\r\n" );	
			break;
		}
		#endif

		if ( strcmp (sIncomingCommand, "RST") == 0 ) 
		{
			cli(); //irq's off 
			wdt_enable(WDTO_15MS); //wd on,15ms 
			while(1); //loop
		}

	}

	return;
}
