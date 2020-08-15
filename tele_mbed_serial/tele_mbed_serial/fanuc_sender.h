
// Masahiro Furukawa
// Jan 23, 2018 updated
// Nov 28, 2017
//
// Reference: 
//
//		Win32 socket programming
//		http://www.geekpage.jp/programming/winsock/
//		Byte order
//		http://wisdom.sakura.ne.jp/system/winapi/winsock/winSock4.html


#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <conio.h>
#include <WS2tcpip.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")


#define DST_IP   "192.168.1.56" // SIX : Hardware
//#define DST_IP   "192.168.1.58" // Eight : SIMULATOR (OLD!DO NOT USE DUE TO ETHERNET ADAPTAR POWER MANAGEMENT ISSUE)
//#define DST_IP   "192.168.1.237" // Eight : SIMULATOR (NEW! USE THIS. This address must equals to the host computer)
// #define DST_IP   "localhost" // Eight : SIMULATOR
#define DST_PORT 11000

class FANUC_SENDER {

private:

	// sockaddr_in struct
	WSAData wsaData;
	SOCKET client;
	struct sockaddr_in addr;
	int cnt;

public:

	// FLOAT -> 4 BYTEs(float32)
	// 4 BYTEs(float32)->FLOAT
	// https://stackoverflow.com/questions/17732630/cast-byte-array-to-float
	//
	struct XYZWPR {

		// header part (index : 0-19)
		BYTE header[20];

		// xyzwpr part (index : 20-43)
		union {
			BYTE bytes[24];
			struct {
				union { BYTE bytes[4];	float f; } x;
				union { BYTE bytes[4];	float f; } y;
				union { BYTE bytes[4];	float f; } z;
				union { BYTE bytes[4];	float f; } w;
				union { BYTE bytes[4];	float f; } p;
				union { BYTE bytes[4];	float f; } r;
			} pos;
		} xyzwpr;

		/// no use. just padded  (index : 44-59)
		BYTE NC[16];

	} val; // end of struct XYZWPR


	// constructor
	FANUC_SENDER();
	// destructor
	~FANUC_SENDER();
	int make_connection(void);
	int send_and_receive();
	void cleanup(void);

}; // end of class FANUC_SENDER