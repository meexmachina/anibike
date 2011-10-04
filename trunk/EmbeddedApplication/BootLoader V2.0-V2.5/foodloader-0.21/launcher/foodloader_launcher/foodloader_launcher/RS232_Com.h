#pragma once

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <windows.h>

#define READ_ERROR 0x101

using namespace std;


#include "RS232_Com.h"

//#define CBR_110             110
//#define CBR_300             300
//#define CBR_600             600
//#define CBR_1200            1200
//#define CBR_2400            2400
//#define CBR_4800            4800
//#define CBR_9600            9600
//#define CBR_14400           14400
//#define CBR_19200           19200
//#define CBR_38400           38400
//#define CBR_56000           56000
//#define CBR_57600           57600
//#define CBR_115200          115200
//#define CBR_128000          128000
//#define CBR_256000          256000

//#define NOPARITY            0
//#define ODDPARITY           1
//#define EVENPARITY          2
//#define MARKPARITY          3
//#define SPACEPARITY         4

//#define ONESTOPBIT          0
//#define ONE5STOPBITS        1
//#define TWOSTOPBITS         2


//====================================================================================================================
std::wstring s2ws(const std::string& s);

//====================================================================================================================
bool OpenComPort   (string PortSpecifier, int BaudRate, int ByteSize, int Parity, int StopBits,		// Input
					 DCB *dcb, HANDLE *hPort );

//====================================================================================================================
bool WriteComPort(HANDLE hPort, char* data);

//====================================================================================================================
bool WriteCharComPort (HANDLE hPort, char data);

//====================================================================================================================
int ReadByteComPort(HANDLE hPort);

//====================================================================================================================
void CloseComPort ( HANDLE hPort );
