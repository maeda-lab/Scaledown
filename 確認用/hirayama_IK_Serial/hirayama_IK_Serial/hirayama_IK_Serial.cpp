// 2020/09/24 TomokiHirayama

#include <math.h>
#include <string>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tel_kin.h"
#include "mbed_Serial.h"
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
    for (int i = 0; i < 5; i++)
    {
        serial_Write(mbed, arg, i);
    }
}

// Masahiro Furukawa
// Aug 24, 2020
//
// Interval Timer 
//
// reference ; https://misakichi-k.hatenablog.com/entry/2018/10/19/010134#WaitableTimer%E3%81%AB%E3%82%88%E3%82%8B%E5%9B%BA%E5%AE%9AFPS
//
////通常は有効でいい、テスト用に存在する
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
    //タイマーがタイム・アウトしている場合はwait-wait間ですでに時間が過ぎているものとして誤差調整処理の対象外にする
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

    LARGE_INTEGER li;
    QueryPerformanceFrequency((LARGE_INTEGER*)&li);
    auto freq = li.QuadPart;

    //get perf timer
    auto getTm = []()->LONGLONG {
        LARGE_INTEGER cnt;
        QueryPerformanceCounter(&cnt);
        return cnt.QuadPart;
    };


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

    //シリアル通信の設定
    mbed = serial_open();
    mbed = serial_initialaize(mbed);
    mbed = serial_Config(mbed);

    //初期位置の座標を計算する
    double master_J1 = 0.0;
    double master_J2 = -120.0;
    double master_J3 = 30.0;


    //�����ʒu�ɂ�����X�̃T�[�{�̉�]�p�̐ݒ�
    double master_theta1 = master_J1;
    double master_theta2 = -1 * master_J2 - 90.0;//-1 * ( -1 * master_J2 - 90.0);
    double master_theta3 = -1 * master_J3 - master_J2 - 90.0;//-1 * (-1 * master_J3 - master_J2 - 90.0);

    //�����ʒu�̍��W���v�Z����
    double master_x = cal_fpx(master_J1, master_J2, master_J3);
    double master_y = cal_fpy(master_J1, master_J2, master_J3);
    double master_z = cal_fpz(master_J1, master_J2, master_J3);

    printf("\n\n\n初期位置は以下の通りです．\n");
    printf("x,y,z=[%lf,%lf,%lf]\n\n", master_x, master_y, master_z);
    printf("アーム全体が実現する初期角は以下の通りです．\n");
    printf("J1,J2,J3=[%lf,%lf,%lf]\n\n", master_J1, master_J2, master_J3);
    printf("個々のサーボが実現する回転角は以下の通りです．\n");
    printf("a,b,c=[%lf,%lf,%lf]\n\n", master_theta1, master_theta2, master_theta3);

    //�����ʒu�Ɉړ�������
    /*for (int i = 0; i < 10; i++)
    {
        send(60.0);
        send(60.0);
        send(60.0);
        send(60.0);
        send(60.0);
        send(60.0);
    }*/
    //�J�E���^
    int t = 0;
    while (1)
    {
        //�ϐ��̐ݒ�
        double a, b, c;
        double j1, j2, j3;
        double theta1, theta2, theta3;
        double x, y, z;
        //printf("==============================================\n");
        //printf("\n�ڕW�l���w�肵�Ă�������.�P�ʂ�mm�ł��D\n");


        //===��ł��ŖڕW�ψʂ���͂������Ƃ��͂�������g��
        //scanf_s("%lf", &posi.x);
        //scanf_s("%lf", &posi.y);
        //scanf_s("%lf", &posi.z);


        //===�A���ő��M��������Ƃ��ɂ�������g��
        //=====�J�E���^�����Ă���̂ŁC�ω�����������
        posi.x = 30.0 * sin(2 * 3.14 * t * 0.01);
        posi.y = 0.0;// +t * 0.01;
        posi.z = 0.0;// +t * 0.01;


        //========================================�����ʒu����̕ψʂ�pos�ɑ��===========================================
        //                                  ->�t�^���w�v�Z�ɂ͐��E���W�n���猩���ʒu�𑫂��Z����D

        //printf("�w�肵�������ʒu����̕ψʂ͈ȉ��̒ʂ�ł�\n");
        //printf("x,y,z=%lf,%lf,%lf\n\n", posi.x, posi.y, posi.z);
        //�w�肵���ψ� �� scale�@�{�@�����ʒu�œ������������E���W�n�ł̈ʒu���v�Z����
        x = posi.x * scale + master_x;
        y = posi.y * scale + master_y;
        z = posi.z * scale + master_z;
        //printf("���E���W�n�ł̈ʒu�͈ȉ��̒ʂ�ł�\t�������C�X�P�[�����%.4lf�ł�\n",scale);
        //printf("x,y,z=%lf,%lf,%lf\n\n", x, y, z);

        //=============================================�t�^���w�̌v�Z=============================================
        //===========================���蕔����1�O�̊p�x�f�[�^���g���̂ŗv�C��================================
        j1 = cal_J1(x, y, z);
        j3 = cal_J3(x, y, z, j1);
        j2 = cal_J2(x, y, z, j1, j3);


        //======================���鎞��t�ɑ΂���t-1�̎����̊p�x�̃f�[�^�������Ă���===========================================
        //prev_J2 = j2;
        //prev_J3 = j3;



        if (j2 == EOF || j3 == EOF)
        {
            printf("EOEOF");
            break;
        }
        //printf("j1,j2,j3=[%lf,%lf,%lf]\n\n", j1, j2, j3);

        //�t�^���w�Ōv�Z�����l�ƃT�[�{�ɓ��͂���p�͈قȂ邽�߁C�ϊ����ɑ������
        theta1 = j1;
        theta2 = -1 * j2 - 90.0;
        theta3 = -j3 - j2 - 90.0;
        //printf("theta1,theta2,theta3=[%lf,%lf,%lf]\n", theta1, theta2, theta3);

        //�������W����̍������v�Z���ăT�[�{�ɑ���l���v�Z����
        a = theta1 - master_theta1;
        b = theta2 - master_theta2;
        c = theta3 - master_theta3;


        //printf("x=%lf\n", posi.x);


        //printf("�ڕW�ʒu�֕ψʂ����邽�߂ɓ��͂���p�x�͎��̒ʂ�ł�\n");
        printf("a,b,c=[%.3lf , %.3lf , %.3lf]\n\n", a, b, c);
        send(a + 60.0);
        send(b + 60.0);
        send(c + 60.0);
        send(0.0 + 60.0);
        send(0.0 + 60.0);
        send(0.0 + 60.0);
        //printf("==============================================\n\n\n");
        t++;
        wait();
    }
    return 0;

}
