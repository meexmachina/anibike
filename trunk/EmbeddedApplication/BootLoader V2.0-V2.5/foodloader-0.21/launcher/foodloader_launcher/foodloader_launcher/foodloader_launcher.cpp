// foodloader_launcher.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "foodloader_launcher.h"
#include "SyncSerialComm.h"



//====================================================================================================================
int main(int argc, char* argv[])
{
	int bErr;
	int iErrCount = 1000;
	int bitrate;
	char *w, *r;
	DWORD size;

	w = new char[2];
	r = new char[2];

	if( (argc != 3) || strcmp(argv[1],"-h")==0 || strcmp(argv[1], "--help")==0 ) {
		printf("Usage: %s <serial device> <baudrate>\n", argv[0]);
	} else {
		printf("Opening serial device %s.\n", argv[1]);

		/* process baud rate */
		switch (atoi(argv[2])) {
			case 4800: bitrate = CBR_4800; break;
			case 9600: bitrate = CBR_9600; break;
			case 19200: bitrate = CBR_19200; break;
			case 38400: bitrate = CBR_38400; break;
			case 57600: bitrate = CBR_57600; break;
			case 115200: bitrate = CBR_115200; break;
			default:
				fprintf(stderr, "Unknown bitrate \"%s\n\"", argv[2]);
				exit(1);
		}

		/* open the serial device */
		iErrCount = 1000;

		CSyncSerialComm port(argv[1]);
		port.ConfigPort(bitrate);

		while ( ( bErr = port.Open() ) != S_OK && iErrCount--) 
		{
				printf("Serial device does not exist, waiting for it to be plugged in.\n");
				// if the serial device isn't there, try opening it again
				Sleep(20);
		}

		if ( bErr != S_OK && iErrCount <= 0 ) 
		{
			fprintf(stderr, "%s: Couldn't find port.\n", argv[1]);
			exit(1);
		}

		w[0] = BOOTLOADER_ENTRY_CHAR;

		// set timeout for checking readability of serial device 
		//tv.tv_sec = 0;
		//tv.tv_usec = DELAY_TIME;

		printf("Sending bootloader entry command (%c).\n", BOOTLOADER_ENTRY_CHAR);
		printf("You may now plug in the target device.\n");

		for(;;) 
		{
			// write the character to enter bootloader
			if ( port.Write(w, 1) != S_OK ) 
			{
				fprintf(stderr, "error during write.\n");
				port.Close( );
				exit(6);
			}

			if ( port.Read( &r, size ) != S_OK ) 
			{
				fprintf(stderr, "error during read.\n");
				port.Close( );
				exit(8);
			}

			// check if it's the right character, if not keep
			// looping 
			if(r[0] == BOOTLOADER_SUCCESS_CHAR) 
			{
				printf("Bootloader running.\n");
				port.Close( );
				exit(0);
			}
		}
		port.Close( );
	}
	

	delete []w;
	delete []r;

	return 0;
}

