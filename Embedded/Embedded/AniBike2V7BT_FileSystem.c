/*
 * AniBike2V7BT_FileSystem.c
 *
 * Created: 8/22/2011 8:20:46 PM
 *  Author: David
 */ 
#include "AniBike2V7BT_Internal.h"

// =================================================================================================
// File System Variables
volatile uint16_t		iCurrentFileNum = 0;					// Point to the first file
volatile uint32_t		iCurrentAddress = ENTRY_TABLE_ADDR;		// Point to the first address
volatile uint16_t		iNumOfFiles = 0xFFFF; 					// 0xFFFF means uninitialized
volatile uint8_t		bFS_Empty = 0;							// Indicates whether file system
																// contains no files


//__________________________________________________________________________________________________
// Transfer two bytes to and from spi recipient
uint16_t transfer_word ( uint16_t w )
{
	uint8_t msb = 0;
	uint8_t lsb = 0;
	uint16_t ret_val = 0;

	if (w==0)
	{
		lsb = SPI_MasterTransceiveByte(&spiMasterD, 0);
		msb = SPI_MasterTransceiveByte(&spiMasterD, 0);
	}
	else
	{
		lsb = (uint8_t)(w & 0xff);
		msb = (uint8_t)(w >> 8);
		//spi_transfer ( msb );
		//spi_transfer ( lsb );
		SPI_MasterTransceiveByte(&spiMasterD, lsb);
		SPI_MasterTransceiveByte(&spiMasterD, msb);
	}

	ret_val = (uint16_t)(lsb) | ((uint16_t)(msb<<8));

	return ret_val;
}

//__________________________________________________________________________________________________
// Reads an entry from the FS table into pEntry - preallocated FileEntry_ST
// Returns success (1) or fail (0)
uint8_t FS_ReadEntry ( uint32_t *pAddress, FileEntry_ST* pEntry )
{
	unsigned char adrByte1, adrByte2, adrByte3;		// adrByte1 = MSB
	uint32_t Address = *pAddress;
	uint16_t iNumFrames16 = 0;
	uint8_t iNumFrames = 0;
	uint8_t itr = 0;

	CS_UP;
	// ======================================================================
	// Read next file entry
	adrByte1 = (Address>>16)&0xff;
	adrByte2 = (Address>>8)&0xff;
	adrByte3 = Address&0xff;

	CS_DOWN;
	SPI_MasterTransceiveByte(&spiMasterD, READ_ARRAY);      			// Read command
	SPI_MasterTransceiveByte(&spiMasterD, adrByte1);					// Send address - 24 bits
	SPI_MasterTransceiveByte(&spiMasterD, adrByte2);
	SPI_MasterTransceiveByte(&spiMasterD, adrByte3);					// starting from MSB

	// Read the number of frames
	iNumFrames16 = transfer_word ( 0 );
	iNumFrames = (uint8_t)(iNumFrames16&0xff);

	// Check if the list has finished (reading empty from flash
	// should be 0xffff word
	if (iNumFrames > FS_FILE_MAX_FRAMES)
	{
		// return fail
		CS_UP;
		return 0;
	}

	pEntry->iNumFrames = iNumFrames;

	// Read the name of the file
	for (itr = 0; itr<FS_FILENAME_LENGTH; itr ++)
		pEntry->sFileName[itr] = SPI_MasterTransceiveByte(&spiMasterD, 0);

	// Read the block addresses
	uint8_t *blockArray = (uint8_t*)(pEntry->iBlockList);
	for (itr = 0; itr<FS_FILE_MAX_FRAMES; itr ++)
	{
		*blockArray++ = SPI_MasterTransceiveByte(&spiMasterD, 0);
		*blockArray++ = SPI_MasterTransceiveByte(&spiMasterD, 0);
	}
	
	// Read other information
	pEntry->Reserved1 = SPI_MasterTransceiveByte(&spiMasterD, 0);
	pEntry->Reserved2 = SPI_MasterTransceiveByte(&spiMasterD, 0);

	CS_UP;

	// Advance the address
	*pAddress += (uint32_t)(ENTRY_TABLE_EL_SIZE);//16+2*iNumFrames;

	return 1;	// Success
}

//__________________________________________________________________________________________________
// Reads the next file entry and updates the global variables
uint8_t FS_ReadNextEntry ( FileEntry_ST* pEntry )
{
	if ( FS_ReadEntry ( (uint32_t*)(&iCurrentAddress), pEntry ) == 0 )	// if couldn't read anymore
	{
		// Start again from the first entry
		iCurrentAddress = ENTRY_TABLE_ADDR;

		if ( FS_ReadEntry ( (uint32_t*)(&iCurrentAddress), pEntry ) == 0 )	// Try again
		{
			bFS_Empty = 1;
			return 0;	// Fail
		}
		else
		{
			bFS_Empty = 0;
		}
	}
	return 1;		// Success
}

//__________________________________________________________________________________________________
// Read Out Frame Information
void FS_ReadFrame (uint16_t iBlock)
{
	unsigned char adrByte1, adrByte2, adrByte3;		// adrByte1 = MSB
	uint32_t				iAddress = 0;
	uint16_t				inner_itr = 0;

	CS_UP;

	// Calculate first address
	iAddress = FS_FRAME_CAP * (uint32_t)(iBlock);

	// Prepare transfer information
	adrByte1 = (iAddress>>16)&0xff;
	adrByte2 = (iAddress>>8)&0xff;
	adrByte3 = iAddress&0xff;

	CS_DOWN;
	SPI_MasterTransceiveByte(&spiMasterD, READ_ARRAY);      			// Read command
	SPI_MasterTransceiveByte(&spiMasterD, adrByte1);					// Send address - 24 bits
	SPI_MasterTransceiveByte(&spiMasterD, adrByte2);
	SPI_MasterTransceiveByte(&spiMasterD, adrByte3);					// starting from MSB

	for (inner_itr = 0; inner_itr < FS_FRAME_SIZE; inner_itr ++)
	{
		putchar( SPI_MasterTransceiveByte(&spiMasterD, 0) );	// Send next char
	}

	CS_UP;
}

//__________________________________________________________________________________________________
// Reads out the contents of a file. Finds the file by its name
uint8_t FS_ReadFileContents( char* FileName )
{
	FileEntry_ST			sFileEntry;
	uint16_t				itr = 0;

	// Search for this particular file
	if ( FS_SearchFile (FileName, &sFileEntry ) )	// Found
	{
		// Read out the information
		for (itr = 0; itr < sFileEntry.iNumFrames; itr ++)
		{
			printf_P( PSTR("\r\nFrame #%d Block #%d:\r\n"), itr, sFileEntry.iBlockList[itr]);

			FS_ReadFrame (sFileEntry.iBlockList[itr]);
		}
	}
	else		// Not Found
	{

	}

	return 1;		// Success
}


//__________________________________________________________________________________________________
// Reads an entry from the FS table into pEntry - Only if filename matches
// Returns success (1) or fail (0) of no match (2)
uint8_t FS_ReadEntry_FileMatch ( uint32_t *pAddress, FileEntry_ST* pEntry, char* FileName )
{
	unsigned char adrByte1, adrByte2, adrByte3;		// adrByte1 = MSB
	uint32_t Address = *pAddress;
	uint16_t iNumFrames16 = 0;
	uint8_t iNumFrames = 0;
	uint8_t itr = 0;

	CS_UP;

	// ======================================================================
	// Read next file entry
	adrByte1 = (Address>>16)&0xff;
	adrByte2 = (Address>>8)&0xff;
	adrByte3 = Address&0xff;

	CS_DOWN;
	SPI_MasterTransceiveByte(&spiMasterD, READ_ARRAY);      			// Read command
	SPI_MasterTransceiveByte(&spiMasterD, adrByte1);					// Send address - 24 bits
	SPI_MasterTransceiveByte(&spiMasterD, adrByte2);
	SPI_MasterTransceiveByte(&spiMasterD, adrByte3);					// starting from MSB

	// Read the number of frames
	iNumFrames16 = transfer_word ( 0 );
	iNumFrames = (uint8_t)(iNumFrames16 & 0xff);

	// Check if the list has finished (reading empty from flash
	// should be 0xffff word
	if (iNumFrames > FS_FILE_MAX_FRAMES)
	{
		// return fail
		CS_UP;
		return 0;
	}

	pEntry->iNumFrames = iNumFrames;

	// Read the name of the file
	for (itr = 0; itr<FS_FILENAME_LENGTH; itr ++)
		pEntry->sFileName[itr] = SPI_MasterTransceiveByte(&spiMasterD, 0); 

	if ( strcmp(pEntry->sFileName, FileName) != 0 )		// No match
	{
		CS_UP;
		*pAddress += (uint32_t)(ENTRY_TABLE_EL_SIZE);//16+2*iNumFrames;	// Advance to next entry
		return 2;
	}

	// Read the block addresses
	for (itr = 0; itr<iNumFrames; itr ++)
		pEntry->iBlockList[itr] = transfer_word (0);

	// Read other information
	pEntry->Reserved1 = SPI_MasterTransceiveByte(&spiMasterD, 0); 
	pEntry->Reserved2 = SPI_MasterTransceiveByte(&spiMasterD, 0); 

	CS_UP;

	// Advance the address
	*pAddress += (uint32_t)(ENTRY_TABLE_EL_SIZE);//16+2*iNumFrames;

	return 1;	// Success
}


//__________________________________________________________________________________________________
// File System - Show on uart the file system contents
void FS_ShowFileInformation ( void )
{
	uint32_t 				Address = ENTRY_TABLE_ADDR;
	FileEntry_ST			sFileEntry;
	uint16_t				iCount = 0;
	uint8_t					blockNum = 0;

	printf_P ( PSTR("\r\n#	Name			Block Num	Duration	Reserved\r\n"));

	Address = ENTRY_TABLE_ADDR;

	while ( FS_ReadEntry ( &Address, &sFileEntry ) )
	{
		iCount ++;
		printf_P(PSTR("%d	%s		%d		%d		%d\r\n"), 	iCount,
															sFileEntry.sFileName,
															sFileEntry.iBlockList[0],
															sFileEntry.Reserved1,
															sFileEntry.Reserved2 );


		if (sFileEntry.iNumFrames>1)
		{		
			printf_P ( PSTR("Blocks:\r\n"));
			for (blockNum = 0; blockNum < sFileEntry.iNumFrames; blockNum++ )
			{
				printf_P( PSTR("%d, "), sFileEntry.iBlockList[blockNum] );
			}
			printf_P ( PSTR("\r\n"));
		}

		iNumOfFiles = iCount;
	}

	if (iCount)
		bFS_Empty = 0;
	else
		bFS_Empty = 1;
}

//__________________________________________________________________________________________________
// File System - Search for a file by name
// Returns success (1) or fail (0)
uint8_t FS_SearchFile (char *FileName, FileEntry_ST	*sFileEntry )
{
	uint32_t 				Address = ENTRY_TABLE_ADDR;
	uint16_t				itr;
	uint8_t					bFound = 0;
	uint8_t					iSucc = 0;

	iSucc = FS_ReadEntry_FileMatch ( &Address, sFileEntry, FileName );
	while ( iSucc != 0 )
	{
		if ( iSucc == 1 )	// Success
		{
			printf_P(PSTR("Nm:%s\r\nOffs:%d\r\nNOFr:%d\r\n"),
																sFileEntry->sFileName,
																sFileEntry->Reserved1,
																sFileEntry->iNumFrames );
			printf_P ( PSTR("FrLst:") );

			for ( itr = 0; itr < sFileEntry->iNumFrames; itr ++)
			{
				printf_P(PSTR("%d "), sFileEntry->iBlockList[itr] );
			}

			printf_P ( PSTR("\r\n\r\n") );
			bFound = 1;
		}

		iSucc = FS_ReadEntry_FileMatch ( &Address, sFileEntry, FileName );
	}

	if (!bFound)
	{
		printf_P ( PSTR("\r\nNot Found\r\n") );
		return 0;	// Fail
	}

	return 1; 		// Success

}


/*void FS_StartReadingFrame ( uint16_t FileIndex, uint8_t iOffset )
{
	unsigned char adrByte1, adrByte2, adrByte3;		// adrByte1 = MSB
	uint32_t Address = FileIndex * FS_FRAME_CAP + iOffset;

	// Verify flash no selected and then select it
	CS_UP;
	CS_DOWN;

	adrByte1 = (Address>>16)&0xff;
	adrByte2 = (Address>>8)&0xff;
	adrByte3 = Address&0xff;     
  
  	// Specify opcode and starting address
	spi_transfer(READ_ARRAY);      			// Read command     
	spi_transfer(adrByte1);					// Send address - 24 bits
	spi_transfer(adrByte2);					// starting from MSB
	spi_transfer(adrByte3);

}*/

//__________________________________________________________________________________________________
// File System - Stop reading from memory
void FS_StopReadingFrame ( void )
{
	CS_UP;
}

//__________________________________________________________________________________________________
// File System - Get new entry table
void FS_WriteNewEntryTable ( void )
{

}


