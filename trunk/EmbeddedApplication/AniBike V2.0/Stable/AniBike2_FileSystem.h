#include <avr/io.h>
#include <compat/deprecated.h>
#include "Anibike_Internal.h"
#include <avr/interrupt.h>
#include <ctype.h>
#include <stdlib.h>


#define FS_FRAME_SIZE		9600		// Bytes
#define FS_HALF_FRAME_SIZE	(uint32_t)(48)		// Bytes
#define	FS_FRAME_CAP		(uint32_t)(12288)		// Bytes
#define FS_NUM_4K_BLOCKS	1024
#define FS_FRAME_BLOCKS		3			// Number of blocks per file
#define FS_FILENAME_LENGTH	12			// The length of filename string
#define FS_FILE_MAX_FRAMES	32			// The maximal number of frames in file
#define	FS_MEMORY_SIZE		(uint32_t)(4194304)		// Bytes
#define NUM_OF_FILES		340			// FS_MEMORY_SIZE div FS_FILE_SIZE = 341.333...
#define ENTRY_TABLE_ADDR	0x3FC000	// The start of last four blocks
#define ENTRY_TABLE_EL_SIZE 80
#define ENTRY_TABLE_BLOCK	NUM_OF_FILES


enum
{
	FS_FILE_TYPE_FRAME 		= 0,		// Regular picture frame
	FS_FILE_TYPE_SPEED 		= 1,		// Speedometer
	FS_FILE_TYPE_EFFECT1	= 2,		// Fire, fractal...
	FS_FILE_TYPE_EFFECT2	= 3
};


typedef struct {
	uint16_t 	iNumFrames;							// Number of frames
	char		sFileName[FS_FILENAME_LENGTH];		// File name string
	uint16_t 	iBlockList[FS_FILE_MAX_FRAMES];		// Frame block list

	uint8_t		iAngleOffset;		// Angle of the hall effect
	uint8_t		iFileType;			// Enum describes different filetypes
} FileEntry_ST;



void 		FS_ShowFileInformation 	( void );
void 		FS_StopReadingFrame 	( void );
uint8_t 	FS_SearchFile 			( char *FileName, FileEntry_ST	*sFileEntry );
uint8_t 	FS_ReadEntry_FileMatch 	( uint32_t *pAddress, FileEntry_ST* pEntry, char* FileName );
uint8_t 	FS_ReadFileContents		( char* FileName );
uint8_t 	FS_ReadNextEntry 		( FileEntry_ST* pEntry );
uint8_t 	FS_ReadEntry 			( uint32_t *pAddress, FileEntry_ST* pEntry );
uint16_t 	transfer_word 			( uint16_t w );

//void 		FS_StartReadingFrame 	( uint16_t FileIndex );

