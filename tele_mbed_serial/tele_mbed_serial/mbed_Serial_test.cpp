//#include"stdafx.h"
#include <stdio.h>
#include <iostream>
#include <tchar.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
//#include <unistd.h>

#include"mbed_Serial.h"
#include"tel_kin.h"
#define a_min -120.0
#define b_min -73.0
#define c_min -3.0
#define d_min -90.0
#define e_min -100.0
#define f_min -100.0

HANDLE mbed_test;
DWORD maindwSendSize;
DWORD maindwGetSize;
unsigned char start[1];
//unsigned char end[1];

void send(double arg);

int main(void)
{
    mbed_test = serial_open();
    mbed_test = serial_initialaize(mbed_test);
    mbed_test = serial_Config(mbed_test);
    start[0] = 'b';
    //end[0] = 'e';


    //cal_T();
    double a = 0.0;// +120.0;//cal_a();
    double b = 50.0;// +73.0;//cal_b(a);
    double c = 20.0;// +3.0;//cal_c(a);
    double d =  0.0;// +90.0;//cal_d(a, b, c);
    double e = 40.0;// +100.0;//cal_e(a, b, c, d);
    double f = 10.0;// +100.0;//cal_f(a, b, c);

    printf("%lf,%lf,%lf,%lf,%lf,%lf\n\n", a, b, c, d, e, f);
    int cmd,endcmd;
    char buffer[1];
    errno_t error;
    while (1)
    {
        printf("waitkey...\n");
        printf("      Prease push key: t:Serial test  r:move to FirstPlace    q:quit  a:MovingTest   s:SerialStart\n");
        cmd = _getch();
        switch (cmd)
        {

        case 't':
            printf("serial test");
            buffer[0] = 't';
            error = WriteFile(mbed_test, buffer, 1, &maindwSendSize, NULL);
            if (!error)
            {
                printf("Send error in t : Cannot send t \n");
            }
            else
            {
                printf("Sending Success!!\n");
                char getter[1];
                //================================WriteFileではなくReadFileです===================================
                Sleep(1);
                error = ReadFile(mbed_test, getter, 1, &maindwGetSize, NULL);
                if (!error)
                {
                    printf("Get error in t : Cannot get t \n");
                }
                else
                {
                    printf("%d\n", int(getter[0]));
                }
            }
            break;

        case 'r':
            //初期位置へ動く
            printf("\nMove to FirstPoint... \n");
            buffer[0] = 'r';
            error = WriteFile(mbed_test, buffer, 1, &maindwSendSize, NULL);
            if (!error)
            {
                printf("Send error in r : Cannot send r \n");
            }
            else
            {
                printf("Moving to FirstPoint Success!!\n");
            }
            break;
        case 'q':
            //終了する
            buffer[0] = 'q';
            error = WriteFile(mbed_test, buffer, 1, &maindwSendSize, NULL);
            if (!error)
            {
                printf("Send error in q : Cannot send q \n");
            }
            else
            {
                printf("Quit command Sending  Success!!\n");
            }
            system("PAUSE");
            exit(0);
            break;
        case 'a':
            //シリアルテスト
            buffer[0] = 'a';
            error = WriteFile(mbed_test, buffer, 1, &maindwSendSize, NULL);
            if (!error)
            {
                printf("Send error in a : Cannot send a \n");
            }
            else
            {
                printf("Test command  a Sending  Success!!\n");
                WriteFile(mbed_test, start, 1, &maindwSendSize, NULL);
                //送信するときはminを足して送信することに注意
                send(a-a_min);
                send(b-b_min);
                send(c-c_min);
                send(d-d_min);
                send(e-e_min);
                send(f-f_min);
                //a += a_min; b -= b_min; c -= c_min; d -= d_min; e -= e_min; f -= f_min;
                //send(a);
                //send(b);
                //send(c);
                //send(d);
                //send(e);
                //send(f);
                //WriteFile(mbed_test, end, 1, &maindwSendSize, NULL);


            }
            break;


        case 's':
            //シリアル通信を開始，角を送る
            buffer[0] = 's';//startコマンドを送る
            error = WriteFile(mbed_test, buffer, 1, &maindwSendSize, NULL);
            if (!error)
            {
                printf("Send error in s : Cannot send s \n");
            }
            else 
            {
                double t=0.0;
                while (1)
                {
                    WriteFile(mbed_test, start, 1, &maindwSendSize, NULL);
                    printf("Start command Sending  Success!!\n");
                    send((a - a_min)+0.05*t);
                    send(b - b_min);
                    send(c - c_min);
                    send(d - d_min);
                    send(e - e_min);
                    send(f - f_min);
                    //WriteFile(mbed_test, end, 1, &maindwSendSize, NULL);
                    t = t + 1.0;
                    if (_kbhit() != 0 && _getch()==0x1b)
                    {
                        //0x1bはescを表す．
                        //keyが押された状態になったなら
                        printf("'esc was pushed.\nFinish Serial moveing... \n'");
                        buffer[0] = 'e';//startコマン ドを送る
                        error = WriteFile(mbed_test, buffer, 1, &maindwSendSize, NULL);
                        if (!error)
                        {
                            printf("Send error in e : Cannot send e \n");
                        }
                        printf("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee\n");
                        
                        break;
                    }
                }
            }
            break;
        }
    }
    return 0;

}


void send(double arg)
{
    for (int cnt = 0; cnt < 4; cnt++)
    {
        serial_Write(mbed_test, arg, cnt);
        char getter[1];
        errno_t error = ReadFile(mbed_test, getter, 1, &maindwGetSize, NULL);
        if (!error)
        {
            printf("Get error in send : Cannot get data \n");
        }
        else
        {
            printf("%c\n", getter[0]);
        }
    }
}