
// Masahiro Furukawa
// Jan 23, 2018 updated
// Nov 28, 2017
// m.furukawa@ist.osaka-u.ac.jp
//
// Reference: 
// http://www.geekpage.jp/programming/winsock/


#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <conio.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <time.h>
#include "fanuc_sender.h"

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

// constructor
FANUC_SENDER::FANUC_SENDER()
{
	/// int32 values (consists of 4 bytes = 4 chars)
	/// must be big-endian (byte-order)
	const BYTE header[20] = {		/// KAREL Language in ROS written as Fanuc Driver
		0x38, 0x00, 0x00, 0x00, /// READ fd(this.length_) 0x38 = 56(10) **** 56 bytes ****
		0x0a, 0x00, 0x00, 0x00,	/// READ fd(this.msg_type_)
		0x01, 0x00, 0x00, 0x00, /// READ fd(this.comm_type_)

		0x00, 0x00, 0x00, 0x00, /// READ fd(this.reply_]type_)
		0x00, 0x00, 0x00, 0x00  /// READ fd(this.seq_nr_)
	};

	const BYTE pos[24] = {		/// KAREL Language in ROS written as Fanuc Driver
		/// READ fd(this.joint_data_[i__])
		0x28, 0xa5, 0x8a, 0x3d, /// x
		0xb0, 0x5f, 0x09, 0xbd, /// y
		0xbe, 0x68, 0xf7, 0xbd, /// z
		0xe9, 0xf1, 0x82, 0x3f, /// r
		0x2f, 0x1b, 0xf6, 0xbe, /// p
		0x2f, 0x1b, 0xf6, 0xbe  /// w
	};

	/// int32 values (consists of 4 bytes)
	/// must be big-endian (byte-order)
	const BYTE NC[16] = {
		0x00, 0x00, 0x00, 0x00, /// NC
		0x00, 0x00, 0x00, 0x00, /// NC
		0x00, 0x00, 0x00, 0x00, /// NC
		0x00, 0x00, 0x00, 0x00  /// NC
	};

	memcpy(val.header, header, 20);
	memcpy(val.xyzwpr.bytes, pos, 24);
	memcpy(val.NC, NC, 16);

	val.xyzwpr.pos.x.f = 30.000f;
	val.xyzwpr.pos.y.f = 48.893f;
	val.xyzwpr.pos.z.f = -487.925f;
	val.xyzwpr.pos.w.f = -180.000f;
	val.xyzwpr.pos.p.f = -0.0f;
	val.xyzwpr.pos.r.f = -30.0f;

	cnt = 0;

} // end of constractor FANUC_SENDER

// destructor
FANUC_SENDER::~FANUC_SENDER() {
	cleanup();
}

int FANUC_SENDER::make_connection(void)
{

	WSAStartup(MAKEWORD(2, 0), &wsaData);

	// connect 
	//memset(&addr, 0, sizeof(addr));
	addr.sin_port = htons(DST_PORT);		// Fanuc port
	addr.sin_family = AF_INET;				// IPv4
	addr.sin_addr.s_addr = inet_addr(DST_IP);

	// TCP Client
	if (true)
		client = socket(AF_INET, SOCK_STREAM, 0); // TCP confirmed by Python 
	else
		client = socket(AF_INET, SOCK_DGRAM, 0);  // UDP

	// connect server
	printf("\n   Fanuc M-1iA at %s:%d -- connecting\n", DST_IP, DST_PORT);

	if (connect(client, (struct sockaddr*) & addr, sizeof(addr)))
	{
		printf("   %s:%d -- failed connecting\n", DST_IP, DST_PORT);
		printf("   hit any key to exit\n");

		_getch();

		return 0;
	}

	// Confirmed to here
	printf("\n *** TCP Connection(%s:%d) Established!!*** \n", DST_IP, DST_PORT);

	return 1;

} // end of FANUC_SENDER::make_connection

int FANUC_SENDER::send_and_receive()
{

	if (++cnt % 6 != 0) return 0; //stream 240Hz

	int ret, n = 0;
	char buf[60];
	unsigned short u;

	// current target position
	printf("%f, %f, %f ",
		val.xyzwpr.pos.x.f,
		val.xyzwpr.pos.y.f,
		val.xyzwpr.pos.z.f);

	// header part (index : 0-19)
	// BYTE ORDER TRANSFER (host to "network short")
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 4; j++)
			buf[4 * i + j] = val.header[4 * i + 3 - j];

	// xyzwpr part (index : 20-43)
	// BYTE ORDER TRANSFER (host to "network short")
	for (int i = 5; i < 11; i++)
		for (int j = 0; j < 4; j++)
			buf[4 * i + j] = val.xyzwpr.bytes[4 * (i - 5) + 3 - j];

	/// no use. just padded  (index : 44-59)
	memcpy(buf + 44, val.NC, 16);



	// send
	ret = send(client, buf, 60, 0);

	//receive response
	//n = recv(client, buf, sizeof(buf), 0);

	if (n < 0) {
		printf("recv err : %d\n", WSAGetLastError());
		_getch();
		return 0;
	}

	printf("received %d bytes, \n", n);

	//_getch();

	return n;

} // end of FANUC_SENDER::send_and_receive

void FANUC_SENDER::cleanup(void) {
	WSACleanup();
}