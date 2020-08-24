// 2020/09/24 TomokiHirayama

#include <math.h>
#include <string>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include"tel_kin.h"
#include"mbed_Serial.h"
#include "hirayama_IK_Serial.h"

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

void send(double arg)
{
    for (int cnt = 0; cnt < 4; cnt++)
    {
        serial_Write(mbed, arg, cnt);
    }

}
VOID APIENTRY DoSomething(LPVOID Args, DWORD low, DWORD high) 
{
    printf(">");
}
int main()
{



    // Masahiro Furukawa
    // Aug 24, 2020
    //
    // Interval Timer 
    //
    // reference : https://qiita.com/tobira-code/items/ae0764634f078407662f

    HANDLE timer = NULL;


    timer = CreateWaitableTimer(
        NULL,  // LPSECURITY_ATTRIBUTES lpTimerAttributes,
        TRUE,  // BOOL bManualReset,
        NULL   // LPCTSTR lpTimerName
    );
    if (timer == NULL) {
        /* error */
    }
    LARGE_INTEGER interval;
    interval.QuadPart = -10 * 1000*3000; /* unit:100nsec, wait xx msec */
    if (!SetWaitableTimer(
        timer,      // HANDLE hTimer,
        &interval,  // LARGE_INTEGER *pDueTime,
        3,          // LONG lPeriod,
        DoSomething,// PTIMERAPCROUTINE pfnCompletionRoutine,
        NULL,       // LPVOID lpArgToCompletionRoutine,
        FALSE       // BOOL fResume
    )) {
        /* error */
    }

    if (WaitForSingleObject(timer, INFINITE) != WAIT_OBJECT_0) {
        /* error */
    }

    if (!CloseHandle(timer)) {
        /* error */
    }

    Pos posi;

    //シリアル通信の設定
    mbed = serial_open();
    mbed = serial_initialaize(mbed);
    mbed = serial_Config(mbed);
    
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

    printf("\n\n\n初期位置は以下の通りです．\n");
    printf("x,y,z=[%lf,%lf,%lf]\n\n", master_x,master_y, master_z);
    printf("アーム全体が実現する初期角は以下の通りです．\n");
    printf("J1,J2,J3=[%lf,%lf,%lf]\n\n", master_J1, master_J2, master_J3);
    printf("個々のサーボが実現する回転角は以下の通りです．\n");
    printf("a,b,c=[%lf,%lf,%lf]\n\n", master_theta1, master_theta2, master_theta3);
    
    //初期位置に移動させる
    for (int i = 0; i < 10; i++)
    {
        send(0 - a_mini);
        send(0 - b_mini);
        send(0 - c_mini);
        send(0 - d_mini);
        send(0 - e_mini);
        send(0 - f_mini);
    }
    //カウンタ
    int t = 0;
    while (1)
    {
        //変数の設定
       double a, b, c;
       double j1, j2, j3;
       double theta1, theta2, theta3;
       double x, y, z;
       //printf("==============================================\n");
       //printf("\n目標値を指定してください.単位はmmです．\n");


       //===手打ちで目標変位を入力したいときはこちらを使う
       //scanf_s("%lf", &posi.x);
       //scanf_s("%lf", &posi.y);
       //scanf_s("%lf", &posi.z);


       //===連続で送信し続けるときにこちらを使う
       posi.x = 0.0 +t * 0.01;
       posi.y = 0.0;// +t * 0.01;
       posi.z = 0.0;// +t * 0.01;


       //========================================初期位置からの変位をposに代入===========================================
       //                                  ->逆運動学計算には世界座標系から見た位置を足し算する．

       //printf("指定した初期位置からの変位は以下の通りです\n");
       //printf("x,y,z=%lf,%lf,%lf\n\n", posi.x, posi.y, posi.z);
       //指定した変位 ＊ scale　＋　初期位置で動かしたい世界座標系での位置を計算する
       x = posi.x * scale + master_x;
       y = posi.y * scale + master_y;
       z = posi.z * scale + master_z;
       //printf("世界座標系での位置は以下の通りです\tただし，スケール比は%.4lfです\n",scale);
       //printf("x,y,z=%lf,%lf,%lf\n\n", x, y, z);

       //逆運動学の計算
       j1 = cal_J1(x, y , z );
       j3 = cal_J3(x, y , z,j1);
       j2 = cal_J2(x, y , z,j1, j3);
       //printf("j1,j2,j3=[%lf,%lf,%lf]\n\n", j1, j2, j3);
       
       //逆運動学で計算した値とサーボに入力する角は異なるため，変換式に代入する
       theta1 = j1 ;
       theta2 = -1*j2 - 90.0;
       theta3 = -j3 - j2 - 90.0;
       //printf("theta1,theta2,theta3=[%lf,%lf,%lf]\n", theta1, theta2, theta3);

       //初期座標からの差分を計算してサーボに送る値を計算する
       a = theta1 - master_theta1;
       b = theta2 - master_theta2;
       c = theta3 - master_theta3;

       
       printf("x=%lf\n", posi.x);


       //printf("目標位置へ変位させるために入力する角度は次の通りです\n");
       printf("a,b,c=[%.3lf , %.3lf , %.3lf]\n\n", a, b, c);
       send(a - a_mini);
       send(b - b_mini);
       send(c - c_mini);
       send(0 - d_mini);
       send(0 - e_mini);
       send(0 - f_mini);
       //printf("==============================================\n\n\n");
       t++;
     }
    return 0;

}
