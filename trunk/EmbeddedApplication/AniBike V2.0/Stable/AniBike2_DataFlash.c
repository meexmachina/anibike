#include "Anibike_Internal.h"
#include "AniBike2_DataFlash.h"


//__________________________________________________________________________________________________
void dataflash_spi_init(void)
{
	// Set SS,MOSI and SCK output, MISO input
	DDR_SPI |= (1<<DD_MOSI)|(1<<DD_SCK)|(1<<DD_SS);
	DD_PORT |= (1<<DD_MISO);		// Set internal pullup on MISO

	// Set CS
	CS_UP;

	/* Enable SPI, Master, set clock rate fck/4 */
	SPCR = (1<<SPE)|(1<<MSTR);
	SPSR = (1<<SPIF)|(1<<SPI2X);
}




//__________________________________________________________________________________________________
// Transfer the data to the data-flash using SPI
uint8_t spi_transfer (uint8_t c)
{
#ifndef __NOSLEEP_NOSPI_DEBUG
	SPDR = c;
	while (! (SPSR & (1<<SPIF))) 
	{
	}
	return SPDR;
#endif
}

//__________________________________________________________________________________________________
void dataflash_spi_idn ( char *ManID, char* DevID1, char* DevID2, char* ExData )
{
	CS_DOWN;

	spi_transfer(IDN);				// Write IDN? command

	*ManID = spi_transfer(0);
	*DevID1 = spi_transfer(0);
	*DevID2 = spi_transfer(0);
	*ExData = spi_transfer(0);

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

	spi_transfer(WRITE_ENABLE);				// Write enable command

	CS_UP;
	CS_DOWN;

	spi_transfer(WRITE_BYTE_PAGE);     		// Write command   

	spi_transfer(adrByte1);					// Send address - 24 bits
	spi_transfer(adrByte2);					// starting from MSB
	spi_transfer(adrByte3);
	spi_transfer(Wrdat);   					// Send data byte

	CS_UP;      

	// Wait until finished 
	while (dataflash_read_status()&FLASH_READY)
	{
		_delay_us(10);
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
	spi_transfer(READ_ARRAY);      			// Read command     

	spi_transfer(adrByte1);					// Send address - 24 bits
	spi_transfer(adrByte2);					// starting from MSB
	spi_transfer(adrByte3);

	data = spi_transfer(0);	
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
	spi_transfer(WRITE_ENABLE);				// Write enable command
	CS_UP;

	CS_DOWN;

	spi_transfer(CHIP_ERASE);     		// Write erase4 opcode

	CS_UP;

	// Wait until finished 
	while (dataflash_read_status()&FLASH_READY)
	{
		_delay_ms(5);
	}
}

//__________________________________________________________________________________________________
void		dataflash_erase_block4		( uint16_t BlockNum )
{
	unsigned char adrByte1, adrByte2, adrByte3;		// adrByte1 = MSB
	uint32_t iAddress = (uint32_t)(FLASH_BLOCK4_SIZE) * BlockNum;

	CS_DOWN;
	spi_transfer(WRITE_ENABLE);				// Write enable command
	CS_UP;

	CS_DOWN;

	spi_transfer(BLOCK_ERASE_4K);     		// Write erase4 opcode

	adrByte1 = (iAddress>>16)&0xff;
	adrByte2 = (iAddress>>8)&0xff;
	adrByte3 = iAddress&0xff;

	spi_transfer(adrByte1);					// Send address - 24 bits
	spi_transfer(adrByte2);					// starting from MSB
	spi_transfer(adrByte3);

	CS_UP;

#ifndef __NOSLEEP_NOSPI_DEBUG
	_delay_ms(5);
#endif

	// Wait until finished 
	while (dataflash_read_status()&FLASH_READY)
	{
		_delay_ms(5);
	}
}

//__________________________________________________________________________________________________
void		dataflash_unprotect_block4	( uint16_t BlockNum )
{
	unsigned char adrByte1, adrByte2, adrByte3;		// adrByte1 = MSB
	uint32_t iAddress = (uint32_t)(FLASH_BLOCK4_SIZE) * BlockNum;

	CS_DOWN;
	spi_transfer(WRITE_ENABLE);				// Write enable command
	CS_UP;

	CS_DOWN;

	spi_transfer(UNPROTECT_SECTOR);     		// Write opcode

	adrByte1 = (iAddress>>16)&0xff;
	adrByte2 = (iAddress>>8)&0xff;
	adrByte3 = iAddress&0xff;

	spi_transfer(adrByte1);					// Send address - 24 bits
	spi_transfer(adrByte2);					// starting from MSB
	spi_transfer(adrByte3);

	CS_UP;
#ifndef __NOSLEEP_NOSPI_DEBUG
	_delay_ms(10);
#endif

	// Wait until finished 
	while (dataflash_read_status()&FLASH_READY)
	{
		_delay_ms(5);
	}
}

//__________________________________________________________________________________________________
void		dataflash_protect_block4	( uint16_t BlockNum )
{
	unsigned char adrByte1, adrByte2, adrByte3;		// adrByte1 = MSB
	uint32_t iAddress = (uint32_t)(FLASH_BLOCK4_SIZE) * BlockNum;

	CS_DOWN;
	spi_transfer(WRITE_ENABLE);				// Write enable command
	CS_UP;

	CS_DOWN;

	spi_transfer(PROTECT_SECTOR);     		// Write erase64 opcode

	adrByte1 = (iAddress>>16)&0xff;
	adrByte2 = (iAddress>>8)&0xff;
	adrByte3 = iAddress&0xff;

	spi_transfer(adrByte1);					// Send address - 24 bits
	spi_transfer(adrByte2);					// starting from MSB
	spi_transfer(adrByte3);

	CS_UP;

#ifndef __NOSLEEP_NOSPI_DEBUG
	_delay_ms(5);
#endif

	// Wait until finished 
	while (dataflash_read_status()&FLASH_READY)
	{
		_delay_ms(5);
	}

}

//__________________________________________________________________________________________________
uint8_t dataflash_read_status ( void )
{
	uint8_t data;

	CS_DOWN;
	spi_transfer(READ_STATUS_REG);     		// Read status   

#ifndef __NOSLEEP_NOSPI_DEBUG
	_delay_us(100);
#endif

	data = spi_transfer(0); 				// Send data byte
	CS_UP;      

	return data;
}
