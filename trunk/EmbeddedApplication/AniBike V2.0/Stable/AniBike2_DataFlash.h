//============================================================
//
//   DATAFLASH Macros & Defines
// 
//============================================================


// Opcodes
#define WRITE_STATUS_REG	0x01
#define WRITE_BYTE_PAGE		0x02
#define READ_ARRAY			0x03
#define WRITE_DISABLE		0x04
#define READ_STATUS_REG		0x05
#define WRITE_ENABLE		0x06
#define BLOCK_ERASE_4K		0x20
#define BLOCK_ERASE_32K		0x52
#define BLOCK_ERASE_64K		0xd8
#define CHIP_ERASE			0x60
#define PROTECT_SECTOR		0x36
#define UNPROTECT_SECTOR	0x39
#define READ_SECTOR_PROTECT 0x3c
#define DEEP_POWER_DOWN		0xb9
#define RESUME_POWER_DOWN	0xab
#define	IDN					0x9f

// Status Register Positions
#define FLASH_READY					0x01
#define FLASH_WE					0x02
#define FLASH_SOFTWARE_PROTECT		0x0c
#define FLASH_WP_PIN				0x10
#define FLASH_ERASE_WRITE_ERR		0x20
#define FLASH_SEC_PROTECT_VEC_LOCK 	0x80

#define FLASH_BLOCK4_SIZE			4096


//============================================================
//
//   DATAFLASH Functions
// 
//============================================================
uint8_t 	spi_transfer (uint8_t c);

void 		dataflash_spi_init		( void );
void 		dataflash_write			( uint32_t addr,
									  char Wrdat );
char 		dataflash_read			( uint32_t addr );
void 		dataflash_spi_idn 		( char *ManID, 
									  char* DevID1, 
									  char* DevID2, 
									  char* ExData );
void 		dataflash_unprotect_all 	( void );
void 		dataflash_protect_all 		( void );
void		dataflash_erase_block4		( uint16_t BlockNum );
void		dataflash_unprotect_block4	( uint16_t BlockNum );
void		dataflash_protect_block4	( uint16_t BlockNum );
void 		dataflash_erase_all 		( void );
uint8_t 	dataflash_read_status 		( void );
