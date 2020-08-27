// Aug 25, 2020
// Masahiro Furukawa 
// 
// Serial Binary Communication

#include <math.h>
#include <string>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

#include "QueryPerformanceTimer.h"
#include "mbed_Serial_binary.h"

// Target Angle in deg
struct TargetAngle {
	float J1_deg;
	float J2_deg;
	float J3_deg;
	float J4_deg;
	float J5_deg;
	float J6_deg;
} tgtAng;

#define NUM_OF_HEADER 2
#define NUM_OF_BUF 26

int main()
{
	// serial port
	Com_Binary com;
	unsigned char sbuf[4 * 6 + NUM_OF_HEADER];
	unsigned char fbuf[4];
	float fret;
	
	// timer
	QueryPerformanceTimer qpTime;
	qpTime.setFps(60);
	qpTime.wait();

	tgtAng.J1_deg = 5.3f;
	tgtAng.J2_deg = 5.2f;
	tgtAng.J3_deg = -0.3f;
	tgtAng.J4_deg = 40.0f;
	tgtAng.J5_deg = 50.0f;
	tgtAng.J6_deg = 60.0f;

	// make float structure -> byte list
	com.float2byte(&sbuf[0 + NUM_OF_HEADER], tgtAng.J1_deg);
	com.float2byte(&sbuf[4 + NUM_OF_HEADER], tgtAng.J2_deg);
	com.float2byte(&sbuf[8 + NUM_OF_HEADER], tgtAng.J3_deg);
	com.float2byte(&sbuf[12 + NUM_OF_HEADER], tgtAng.J4_deg);
	com.float2byte(&sbuf[16 + NUM_OF_HEADER], tgtAng.J5_deg);
	com.float2byte(&sbuf[20 + NUM_OF_HEADER], tgtAng.J6_deg);

	// header
	sbuf[0] = '*';
	sbuf[1] = '+';

	// bytes -> float conversion test
	memcpy(fbuf, &sbuf[0 + NUM_OF_HEADER], 4);
	com.byte2float(&fret, fbuf);
	std::printf("J1_deg is %3.3lf \n", fret);

	memcpy(fbuf, &sbuf[4 + NUM_OF_HEADER], 4);
	com.byte2float(&fret, fbuf);
	std::printf("J2_deg is %3.3lf \n", fret);

	memcpy(fbuf, &sbuf[8 + NUM_OF_HEADER], 4);
	com.byte2float(&fret, fbuf);
	std::printf("J3_deg is %3.3lf \n", fret);

	memcpy(fbuf, &sbuf[12 + NUM_OF_HEADER], 4);
	com.byte2float(&fret, fbuf);
	std::printf("J4_deg is %3.3lf \n", fret);

	memcpy(fbuf, &sbuf[16 + NUM_OF_HEADER], 4);
	com.byte2float(&fret, fbuf);
	std::printf("J5_deg is %3.3lf \n", fret);

	memcpy(fbuf, &sbuf[20 + NUM_OF_HEADER], 4);
	com.byte2float(&fret, fbuf);
	std::printf("J6_deg is %3.3lf \n", fret);

	std::printf("The byte list : %02x %02x   ", sbuf[0], sbuf[1]);
	// dump byte list
	for (int j = 0; j < 6; j++){
		for (int i = 0; i < 4; i++){
			std::printf("%02x ", (unsigned char) sbuf[i + 4 * j + NUM_OF_HEADER]);
		}
		printf("  ");
	}
	printf("\n\nsizeof(sbuf) = <%d>\n",(int)sizeof(sbuf));

	// test for 24byte sending while 2 seconds
	for (int i = 0; i < 120; i++) {
		com.send_bytes(sbuf, NUM_OF_BUF);
		qpTime.wait();
	}

	return 0;

}
