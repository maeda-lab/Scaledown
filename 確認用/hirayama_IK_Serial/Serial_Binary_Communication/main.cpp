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

int main()
{
	QueryPerformanceTimer qpTime;
	qpTime.setFps(60);
	qpTime.wait();

	tgtAng.J1_deg = 0.1f;
	tgtAng.J2_deg = 0.2f;
	tgtAng.J3_deg = 0.3f;
	tgtAng.J4_deg = 40.0f;
	tgtAng.J5_deg = 50.0f;
	tgtAng.J6_deg = 60.0f;

	Com_Binary com;

	unsigned char sbuf[4 * 6];
	unsigned char fbuf[4];
	float fret;

	// make float structure -> byte list
	com.float2byte(&sbuf[ 0], tgtAng.J1_deg);
	com.float2byte(&sbuf[ 4], tgtAng.J2_deg);
	com.float2byte(&sbuf[ 8], tgtAng.J3_deg);
	com.float2byte(&sbuf[12], tgtAng.J4_deg);
	com.float2byte(&sbuf[16], tgtAng.J5_deg);
	com.float2byte(&sbuf[20], tgtAng.J6_deg);

	// bytes -> float conversion test
	memcpy(fbuf, sbuf, 4);
	com.byte2float(&fret, fbuf);
	std::printf("The value is %3.3lf *\n", fret);

	memcpy(fbuf, &sbuf[4], 4);
	com.byte2float(&fret, fbuf);
	std::printf("The value is %3.3lf *4\n", fret);

	memcpy(fbuf, &sbuf[8], 4);
	com.byte2float(&fret, fbuf);
	std::printf("The value is %3.3lf *8\n", fret);

	memcpy(fbuf, &sbuf[12], 4);
	com.byte2float(&fret, fbuf);
	std::printf("The value is %3.3lf *12\n", fret);

	memcpy(fbuf, &sbuf[16], 4);
	com.byte2float(&fret, fbuf);
	std::printf("The value is %3.3lf *16\n", fret);

	memcpy(fbuf, &sbuf[20], 4);
	com.byte2float(&fret, fbuf);
	std::printf("The value is %3.3lf *20\n", fret);


	// test
	for (std::size_t i = 0; i != 4 * 6; ++i)
	{
		std::printf("The byte #%zu is 0x%02X *\n", i, sbuf[i]);
	}

	com.send_one_float(0.1);
	com.send_bytes(sbuf);
	return 0;

}
