/*
 * AniBike2V7BT_FileSystem.h
 *
 * Created: 8/22/2011 8:20:28 PM
 *  Author: David
 */ 


#ifndef ANIBIKE2V7BT_FILESYSTEM_H_
#define ANIBIKE2V7BT_FILESYSTEM_H_


#define FS_FRAME_SIZE		9600				// Bytes
#define FS_HALF_FRAME_SIZE	(uint32_t)(48)		// Bytes
#define	FS_FRAME_CAP		(uint32_t)(12288)	// Bytes
#define FS_NUM_4K_BLOCKS	1024
#define FS_FRAME_BLOCKS		3						// Number of blocks per frame
#define FS_FILENAME_LENGTH	12						// The length of filename string
#define FS_FILE_MAX_FRAMES	32						// The maximal number of frames in file
#define	FS_MEMORY_SIZE		(uint32_t)(4194304)		// Bytes
#define NUM_OF_FILES		339						// FS_MEMORY_SIZE div FS_FILE_SIZE = ... The entry table starts at block 1017
#define ENTRY_TABLE_ADDR	0x3F9000				// The start of last seven blocks
													// HEX: 0x400000 - 0x7 * 0x1000
													// DEC: 4194304 - 7 * 4096
#define ENTRY_TABLE_EL_SIZE (2+(FS_FILENAME_LENGTH)+((FS_FILE_MAX_FRAMES)*2)+1+1)
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

	uint8_t		Reserved1;			// Angle of the hall effect
									// Or Duration in Moussavi
	uint8_t		Reserved2;			// Enum describes different filetypes
									// Or Width in Moussavi
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



#endif /* ANIBIKE2V7BT_FILESYSTEM_H_ */