// Aug 25, 2020
// Masahiro Furukawa 
// reference : hirayama_IK_Serial.cpp

#include <math.h>
#include <string>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>

#include "hirayama_IK_Serial/hirayama_IK_Serial.h"
#include "hirayama_IK_Serial/tel_kin.h"

#define PI 3.14159265358979

DWORD maindwSendSize;
DWORD maindwGetSize;
unsigned char start[1];
char buffer[1];
errno_t error;
HANDLE mbed;

#define a_mini  -120.0
#define b_mini  -73.0
#define c_mini  -3.0
#define d_mini  -90.0
#define e_mini  -100.0
#define f_mini  -100.0

#define scale 1.0


double prev_J2;
double prev_J3;

struct Pos
{
	double x;
	double y;
	double z;
};

// Masahiro Furukawa
// Aug 24, 2020
//
// Interval Timer 
//
// reference ; https://misakichi-k.hatenablog.com/entry/2018/10/19/010134#WaitableTimer%E3%81%AB%E3%82%88%E3%82%8B%E5%9B%BA%E5%AE%9AFPS
//
#define ENABLE_ERROR_CORRECTION

unsigned int	fps_;
bool			preframeIsDelay_;
HANDLE			timer_;
LONGLONG		freq_;
LONGLONG		preframeTime_;
LONGLONG		waitTime_;

LONGLONG getTime();
LONGLONG getWaitTime();
LONGLONG msTo100Ns(LONGLONG ms);
LONGLONG usTo100Ns(LONGLONG us);

LONGLONG getWaitTime() {
	return waitTime_;
}
LONGLONG getTime() {
	LONGLONG tm;
	QueryPerformanceCounter((LARGE_INTEGER*)&tm);
	return tm;
}
LONGLONG msTo100Ns(LONGLONG ms) {
	return usTo100Ns(ms * 1000);
}
LONGLONG usTo100Ns(LONGLONG us) {
	return us * 10;
}
void wait() {
	//timer object wait. one frame time each if to timeout.

#ifdef ENABLE_ERROR_CORRECTION
	auto waitRet = WaitForSingleObject(timer_, (1000 + fps_ - 1) / fps_);
#else
	auto waitRet = WaitForSingleObject(timer_, INFINITE);
#endif
	auto current = getTime();
#ifdef ENABLE_ERROR_CORRECTION
	auto sub = (current - preframeTime_) - freq_ / fps_;
	auto delay = waitRet == WAIT_TIMEOUT;
	if (delay == false && preframeIsDelay_ == false) {
		waitTime_ += sub;
	}
	preframeIsDelay_ = delay;
#endif
	SetWaitableTimer(timer_, (LARGE_INTEGER*)&waitTime_, 0, NULL, NULL, FALSE);
	preframeTime_ = getTime();

}

int main()
{
	//// Masahiro Furukawa
	//// Aug 24, 2020
	////
	//// Interval Timer 
	////
	//// https://misakichi-k.hatenablog.com/entry/2018/10/19/010134#WaitableTimer%E3%81%AB%E3%82%88%E3%82%8B%E5%9B%BA%E5%AE%9AFPS


	// timer interval 
	fps_ = 60;


	timer_ = CreateWaitableTimer(NULL, FALSE, NULL);
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq_);
	preframeTime_ = getTime();
	waitTime_ = -msTo100Ns(1000) / fps_;
	SetWaitableTimer(timer_, (LARGE_INTEGER*)&waitTime_, 0, NULL, NULL, FALSE);


	LONGLONG		startTime_;
	LONGLONG		endTime_;

	//// timer sample
	//for (int i = 0; i < 200; i++) {
	//    for (int j = 0; j < 6; j++) {
	//        for (int k = 0; k < 10; k++) {
	//            printf(">");
	//            wait();
	//        }
	//        printf(" ");
	//    }
	//    printf("\n");
	//}

	Pos posi;

	////シリアル通信の設定
	//mbed = serial_open();
	//mbed = serial_initialaize(mbed);
	//mbed = serial_Config(mbed);

	//初期位置におけるアーム全体が実現する関節角の設定
	double master_J1 = 0.0;
	double master_J2 = -120.0;
	double master_J3 = 30.0;


	//初期位置における個々のサーボの回転角の設定
	double master_theta1 = master_J1;
	double master_theta2 = -1 * master_J2 - 90.0;//-1 * ( -1 * master_J2 - 90.0);
	double master_theta3 = -1 * master_J3 - master_J2 - 90.0;//-1 * (-1 * master_J3 - master_J2 - 90.0);

	//初期位置の座標を計算する
	double master_x = cal_fpx(master_J1, master_J2, master_J3);
	double master_y = cal_fpy(master_J1, master_J2, master_J3);
	double master_z = cal_fpz(master_J1, master_J2, master_J3);

	int t = 0;

	double a, b, c;
	double j1, j2, j3;
	double theta1, theta2, theta3;
	double x, y, z;

	// start time
	startTime_ = getTime();

#define NUM_OF_IK_CYCLE 100000

	for (int i = 0; i < NUM_OF_IK_CYCLE; i++)
	{
		// target position
		posi.x = 30.0 * sin(2 * PI * t * 0.01);
		posi.y = 0.0;// +t * 0.01;
		posi.z = 0.0;// +t * 0.01;

		x = posi.x * scale + master_x;
		y = posi.y * scale + master_y;
		z = posi.z * scale + master_z;

		//逆運動学の計算
		j1 = cal_J1(x, y, z);
		j3 = cal_J3(x, y, z, j1);
		j2 = cal_J2(x, y, z, j1, j3);

		if (j2 == EOF || j3 == EOF)
		{
			printf("EOEOF");
			break;
		}

	   //逆運動学で計算した値とサーボに入力する角は異なるため，変換式に代入する
		theta1 = j1;
		theta2 = -1 * j2 - 90.0;
		theta3 = -j3 - j2 - 90.0;
		//printf("theta1,theta2,theta3=[%lf,%lf,%lf]\n", theta1, theta2, theta3);

	   //初期座標からの差分を計算してサーボに送る値を計算する
		a = theta1 - master_theta1;
		b = theta2 - master_theta2;
		c = theta3 - master_theta3;


		////printf("x=%lf\n", posi.x);
		t++;
	}

	endTime_ = getTime();

#ifndef _DEBUG                  /* For RELEASE builds */
	printf("\n *** Relase Build ****\n\n");
#else                           /* For DEBUG builds   */
	printf("\n !!! Debug Build  !!!! --- SLOW !!! DO NOT BUILD UNDER Debug Mode !!!\n\n");
#endif

	printf("\nperformance counter frequency : %lld [MHz]",    freq_ / 1000000);
	printf("\nperformance counter term      : %lld [ns]\n\n", 1000000000 / freq_);
	
	printf("num of cycle : %d\n\n",   t);
	printf("start count  : %lld\n", startTime_);
	printf("end   count  : %lld\n", endTime_);
	printf("diff  count  : %lld\n", endTime_ - startTime_);
	
	printf("\nIK solusion took %.3lf [ns/cycle] = %.3lf [us/cycle]\n\n",
		(double)(endTime_ - startTime_) * (double)1000000000 / (double)freq_ / (double)NUM_OF_IK_CYCLE,
		(double)(endTime_ - startTime_) * (double)1000000 / (double)freq_ / (double)NUM_OF_IK_CYCLE);

	return 0;

}
