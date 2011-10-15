#include "AniBike2V7BT_Internal.h"

SPI_Master_t spiMasterD;

//__________________________________________________________________________________________________
void dataflash_spi_init(void)
{
	/* Init SS pin as output with wired AND and pull-up. */
	PORTD.DIRSET = PIN4_bm;
	//PORTD.PIN4CTRL = PORT_OPC_WIREDANDPULL_gc;

	/* Set SS output to high. (No slave addressed). */
	//PORTD.OUTSET = PIN4_bm;
	
	/* Initialize SPI master on port DATAFLASH_PORT. */
	SPI_MasterInit(&spiMasterD,
	               &DATAFLASH_SPI,
	               &DATAFLASH_PORT,
	               false,
	               SPI_MODE_0_gc,
	               SPI_INTLVL_OFF_gc,
	               true,
	               SPI_PRESCALER_DIV4_gc, 1);
				   
	// Set CS
	CS_UP;
}




//__________________________________________________________________________________________________
void dataflash_spi_idn ( char *ManID, char* DevID1, char* DevID2, char* ExData )
{
	CS_DOWN;
	
	SPI_MasterTransceiveByte(&spiMasterD, IDN);			// Write IDN? command

	*ManID = SPI_MasterTransceiveByte(&spiMasterD, 0);
	*DevID1 = SPI_MasterTransceiveByte(&spiMasterD, 0);
	*DevID2 = SPI_MasterTransceiveByte(&spiMasterD, 0);
	*ExData = SPI_MasterTransceiveByte(&spiMasterD, 0);

	CS_UP;
}

//__________________________________________________________________________________________________
void dataflash_write( uint32_t addr,char Wrdat)
{
	unsigned char adrByte1, adrByte2, adrByte3;		// adrByte1 = MSB
	adrByte1 = (addr>>16)&0xff;
	adrByte2 = (addr>>8)&0xff;
	adrByte3 = addr&0xff;     
	      
	CS_DOWN;

	SPI_MasterTransceiveByte(&spiMasterD, WRITE_ENABLE);				// Write enable command

	CS_UP;
	
	CS_DOWN;

	SPI_MasterTransceiveByte(&spiMasterD, WRITE_BYTE_PAGE);	// Write command   

	SPI_MasterTransceiveByte(&spiMasterD, adrByte1);		// Send address - 24 bits
	SPI_MasterTransceiveByte(&spiMasterD, adrByte2);		// starting from MSB
	SPI_MasterTransceiveByte(&spiMasterD, adrByte3);
	SPI_MasterTransceiveByte(&spiMasterD, Wrdat);			// Send data byte

	CS_UP;      

	// Wait until finished 
	while (dataflash_read_status()&FLASH_READY)
	{
		//_delay_us(10);
	}
}

//__________________________________________________________________________________________________
char dataflash_read(uint32_t addr)
{
	unsigned char adrByte1, adrByte2, adrByte3;		// adrByte1 = MSB
	char data;	

	adrByte1 = (addr>>16)&0xff;
	adrByte2 = (addr>>8)&0xff;
	adrByte3 = addr&0xff;     
   
	CS_DOWN;
	SPI_MasterTransceiveByte(&spiMasterD, READ_ARRAY);		// Read command     

	SPI_MasterTransceiveByte(&spiMasterD, adrByte1);		// Send address - 24 bits
	SPI_MasterTransceiveByte(&spiMasterD, adrByte2);		// starting from MSB
	SPI_MasterTransceiveByte(&spiMasterD, adrByte3);

	data = SPI_MasterTransceiveByte( &spiMasterD, 0 );	
	CS_UP;      

	return data;
}


//__________________________________________________________________________________________________
void dataflash_unprotect_all (void)
{
	uint16_t i;

	for ( i=0; i<1024; i++ )
	{
		dataflash_unprotect_block4 (i);
	}
}

//__________________________________________________________________________________________________
void dataflash_protect_all ( void )
{
	uint16_t i;

	for ( i=0; i<1024; i++ )
	{
		dataflash_protect_block4 (i);
	}
}

//__________________________________________________________________________________________________
void dataflash_erase_all ( void )
{
	dataflash_unprotect_all ();

	CS_DOWN;
	SPI_MasterTransceiveByte(&spiMasterD, WRITE_ENABLE);	// Write enable command
	CS_UP;

	CS_DOWN;

	SPI_MasterTransceiveByte(&spiMasterD, CHIP_ERASE);     		// Write erase4 opcode

	CS_UP;

	// Wait until finished 
	while (dataflash_read_status()&FLASH_READY)
	{
		//_delay_ms(5);
	}
}

//__________________________________________________________________________________________________
void		dataflash_erase_block4		( uint16_t BlockNum )
{
	unsigned char adrByte1, adrByte2, adrByte3;		// adrByte1 = MSB
	uint32_t iAddress = (uint32_t)(FLASH_BLOCK4_SIZE) * BlockNum;

	CS_DOWN;
	SPI_MasterTransceiveByte(&spiMasterD, WRITE_ENABLE);			// Write enable command
	CS_UP;

	CS_DOWN;
	SPI_MasterTransceiveByte(&spiMasterD, BLOCK_ERASE_4K);			// Write erase4 opcode

	adrByte1 = (iAddress>>16)&0xff;
	adrByte2 = (iAddress>>8)&0xff;
	adrByte3 = iAddress&0xff;

	SPI_MasterTransceiveByte(&spiMasterD, adrByte1);		// Send address - 24 bits
	SPI_MasterTransceiveByte(&spiMasterD, adrByte2);		// starting from MSB
	SPI_MasterTransceiveByte(&spiMasterD, adrByte3);

	CS_UP;

	// Wait until finished 
	while (dataflash_read_status()&FLASH_READY)
	{
		//_delay_ms(5);
	}
}

//__________________________________________________________________________________________________
void		dataflash_unprotect_block4	( uint16_t BlockNum )
{
	unsigned char adrByte1, adrByte2, adrByte3;		// adrByte1 = MSB
	uint32_t iAddress = (uint32_t)(FLASH_BLOCK4_SIZE) * BlockNum;

	CS_DOWN;
	SPI_MasterTransceiveByte(&spiMasterD, WRITE_ENABLE);				// Write enable command
	CS_UP;

	CS_DOWN;
	SPI_MasterTransceiveByte(&spiMasterD, UNPROTECT_SECTOR);     		// Write opcode

	adrByte1 = (iAddress>>16)&0xff;
	adrByte2 = (iAddress>>8)&0xff;
	adrByte3 = iAddress&0xff;

	SPI_MasterTransceiveByte(&spiMasterD, adrByte1);		// Send address - 24 bits
	SPI_MasterTransceiveByte(&spiMasterD, adrByte2);		// starting from MSB
	SPI_MasterTransceiveByte(&spiMasterD, adrByte3);

	CS_UP;

	// Wait until finished 
	while (dataflash_read_status()&FLASH_READY)
	{
		//_delay_ms(5);
	}
}

//__________________________________________________________________________________________________
void		dataflash_protect_block4	( uint16_t BlockNum )
{
	unsigned char adrByte1, adrByte2, adrByte3;		// adrByte1 = MSB
	uint32_t iAddress = (uint32_t)(FLASH_BLOCK4_SIZE) * BlockNum;

	CS_DOWN;
	SPI_MasterTransceiveByte(&spiMasterD, WRITE_ENABLE);			// Write enable command
	CS_UP;

	CS_DOWN;
	SPI_MasterTransceiveByte(&spiMasterD, PROTECT_SECTOR);    		// Write erase64 opcode

	adrByte1 = (iAddress>>16)&0xff;
	adrByte2 = (iAddress>>8)&0xff;
	adrByte3 = iAddress&0xff;

	SPI_MasterTransceiveByte(&spiMasterD, adrByte1);		// Send address - 24 bits
	SPI_MasterTransceiveByte(&spiMasterD, adrByte2);		// starting from MSB
	SPI_MasterTransceiveByte(&spiMasterD, adrByte3);

	CS_UP;

	// Wait until finished 
	while (dataflash_read_status()&FLASH_READY)
	{
		//_delay_ms(5);
	}

}

//__________________________________________________________________________________________________
uint8_t dataflash_read_status ( void )
{
	uint8_t data;

	CS_DOWN;
	SPI_MasterTransceiveByte(&spiMasterD, READ_STATUS_REG);    		// Read status   

	data = SPI_MasterTransceiveByte(&spiMasterD, 0);				// Send data byte
	CS_UP;      

	return data;
}
