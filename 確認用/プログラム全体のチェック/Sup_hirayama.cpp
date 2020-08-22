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

#define scale 1.0
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
        //serial_Write(mbed, arg, cnt);
    }

}

int main()
{
    Pos posi;

    //mbed = serial_open();
    //mbed = serial_initialaize(mbed);
    //mbed = serial_Config(mbed);
    
    //世界座標系から見た初期位置の座標(単位はm)
    //double master_x = 0.24;
    //double master_y = -0.01;
    //double master_z = -0.095427;
    //double master_a = -2.385944;
    //double master_b = 8.259;
    //double master_c = 88.316;//109.338;// -95.783661;
    
    //初期位置に向かう関節角
    double master_J1 = 0.0;
    double master_J2 = 120.0;
    double master_J3 = 0.0;//109.338;// -95.783661;

    //初期位置座標の計算
    double master_x = cal_fpx(master_J1, master_J2, master_J3);
    double master_y = cal_fpy(master_J1, master_J2, master_J3);
    double master_z = cal_fpz(master_J1, master_J2, master_J3);

    //入力角の変換
    double master_a = master_J1;
    double master_b = -1*(master_J2 - 90.0);//機構上反転させたものを戻す
    double master_c = master_J2 + master_J3 - 90.0;

    printf("\n\n\n初期位置は以下の通りです．\n");
    printf("x,y,z=[%lf,%lf,%lf]\n\n", master_x,master_y, master_z);
    printf("初期角は以下の通りです．\n");
    printf("J1,J2,J3=[%lf,%lf,%lf]\n\n", master_J1, master_J2, master_J3);
    printf("初期角は以下の通りです．\n");
    printf("a,b,c=[%lf,%lf,%lf]\n\n", master_a, master_b, master_c);

    int t = 0;
    while (1)
    {
       printf("==============================================\n");
       printf("\n目標値を指定してください\n");
       posi.x = (_getch()-'0')   * 0.01;
       posi.y = (_getch() - '0') * 0.01;
       posi.z = (_getch() - '0') * 0.01;

       //初期位置からの変位をposに代入
       //->逆運動学計算には世界座標系から見た位置を足し算する．

       //この変位の単位は何？
       printf("指定した初期位置からの変位は以下の通りです\n");
       printf("x,y,z=%lf,%lf,%lf\n\n", posi.x, posi.y, posi.z);
       double a, b, c;
       double j1, j2, j3;
       double x, y, z;

       //指定した変位＊scale　＋　初期位置
       x = posi.x * scale + master_x;
       y = posi.y * scale + master_y;
       z = posi.z * scale + master_z;
       printf("世界座標系での位置は以下の通りです\tただし，スケール比は%.4lfです\n",scale);
       printf("x,y,z=%lf,%lf,%lf\n\n", x, y, z);

       //逆運動学の計算
       j1 = cal_J1(x, y , z );
       j3 = cal_J3(x, y , z,j1);
       j2 = cal_J2(x, y , z,j1, j3);
       printf("j1,j2,j3=[%lf,%lf,%lf]\n\n", j1, j2, j3);
       
       

       a = j1-master_a;
       b = -1*(j2 - 90.0) - master_b;//機構上の補正
       c = j3 + j2 - 90.0 - master_c;
       
       //a = master_a - j1;
       //b = master_b - (j2 - 90);
       //c = master_c - (j3 + j2);

       
       printf("目標位置へ変位させるために入力する角度は次の通りです\n");

       //バッファーかます
       a = a / 1.0;
       b = -1*b / 0.85;
       c = c / 0.9;
       printf("a,b,c=[%.3lf , %.3lf , %.3lf]\n\n", a, b, c);

       send(a - a_mini);
       send(b - b_mini);
       send(c - c_mini);
       send(0 - d_mini);
       send(0 - e_mini);
       send(0 - f_mini);
       printf("==============================================\n\n\n");

     }
    return 0;

}
