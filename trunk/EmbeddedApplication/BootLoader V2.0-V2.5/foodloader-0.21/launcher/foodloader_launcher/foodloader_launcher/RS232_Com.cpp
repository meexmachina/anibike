#include "RS232_Com.h"


//====================================================================================================================
std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

//====================================================================================================================
bool OpenComPort   (string PortSpecifier, int BaudRate, int ByteSize, int Parity, int StopBits,		// Input
					 DCB *dcb, HANDLE *hPort )														// Output
{
	wstring stemp = s2ws(PortSpecifier);
	LPCWSTR result = stemp.c_str();

	*hPort = CreateFile( result, GENERIC_ALL, 0, NULL, OPEN_EXISTING, 0, NULL );

	GetCommState(*hPort, dcb);

	dcb->BaudRate = BaudRate;	//CBR_9600; //9600 Baud
	dcb->ByteSize = ByteSize;	//8 data bits
	dcb->Parity = Parity;		//NOPARITY; //no parity
	dcb->StopBits = StopBits;	//ONESTOPBIT; //1 stop

	if (!SetCommState(*hPort,dcb))
		return false;

	return true;
}

//====================================================================================================================
bool WriteComPort(HANDLE hPort, char* data)
{
	DWORD byteswritten;
	
	bool retVal = WriteFile(hPort, data, strlen(data), &byteswritten, NULL);
	return retVal;
}

//====================================================================================================================
bool WriteCharComPort (HANDLE hPort, char data)
{
	DWORD byteswritten;
	
	bool retVal = WriteFile(hPort, &data, 1, &byteswritten, NULL);
	return retVal;
}

//====================================================================================================================
int ReadByteComPort(HANDLE hPort)
{
	int retVal;
	BYTE Byte;
	DWORD dwBytesTransferred;
	DWORD dwCommModemStatus;

	SetCommMask (hPort, EV_RXCHAR | EV_ERR); //receive character event
	WaitCommEvent (hPort, &dwCommModemStatus, 0); //wait for character
	if (dwCommModemStatus & EV_RXCHAR)
		ReadFile (hPort, &Byte, 1, &dwBytesTransferred, 0); //read 1
	else if (dwCommModemStatus & EV_ERR)
		retVal = READ_ERROR;
	retVal = Byte;
	return retVal;
}

//====================================================================================================================
void CloseComPort ( HANDLE hPort )
{
	CloseHandle(hPort);
}
