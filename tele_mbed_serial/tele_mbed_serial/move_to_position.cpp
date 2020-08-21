#include <math.h>
#include <string>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include"tel_kin.h"
#include"mbed_Serial.h"

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

int main()
{
    char buffer[1],start[1];
    errno_t error;
    Pos posi;

    mbed = serial_open();
    mbed = serial_initialaize(mbed);
    mbed = serial_Config(mbed);
    buffer[0] = 's';//startコマンドを送る
    error = WriteFile(mbed, buffer, 1, &maindwSendSize, NULL);
    if (!error)
    {
        printf("Send error in s : Cannot send s \n");
        exit(0);
    }

    int t = 0;
    while (1)
    {
        printf("目標値を指定してください\n");
        posi.x = (_getch()-'0') * 0.01;
        posi.y = (_getch() - '0') * 0.01;
        posi.z = (_getch() - '0') * 0.01;
        printf("x,y,z=%lf,%lf,%lf\n", posi.x, posi.y, posi.z);
        double a, b, c;
        double j1, j2, j3;
        j1 = cal_J1(posi.x, posi.y , posi.z );
        j3 = cal_J3(posi.x, posi.y , posi.z,j1);
        j2 = cal_J2(posi.x, posi.y , posi.z,j1, j3);
        printf("j1,j2,j3=[%lf,%lf,%lf]\n", j1, j2, j3);
        a = j1;
        b = j2 - 90 + 181.884072;
        c = j3 - j2 - 101.884072;
        printf("a,b,c=[%lf,%lf,%lf]\n", a, b, c);
        
        //バッファーかます
        a = a / 1.0;
        b = b / 0.85;
        c = c / 0.9;

        start[0] = 'b';//startコマンドを送る
        error = WriteFile(mbed, start, 1, &maindwSendSize, NULL);
        if (!error)
        {
            //printf("Send error in s : Cannot send s \n");
        }
        else
        {
            //printf("SendOK");
            send(a - a_mini);
            send(b - b_mini);
            send(c - c_mini);
            send(0 - d_mini);
            send(0 - e_mini);
            send(0 - f_mini);
        }
        t++;
    }
    return 0;

}
