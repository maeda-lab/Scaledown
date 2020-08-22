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
        serial_Write(mbed, arg, cnt);
    }

}

int main()
{
    Pos posi;

    mbed = serial_open();
    mbed = serial_initialaize(mbed);
    mbed = serial_Config(mbed);
    
    //���E���W�n���猩�������ʒu�̍��W(�P�ʂ�m)
    //double master_x = 0.24;
    //double master_y = -0.01;
    //double master_z = -0.095427;
    //double master_a = -2.385944;
    //double master_b = 8.259;
    //double master_c = 88.316;//109.338;// -95.783661;
    
    //�����ʒu�Ɍ������֐ߊp
    double master_J1 = 0.0;
    double master_J2 = 120.0;
    double master_J3 = 0.0;//109.338;// -95.783661;

    //�����ʒu���W�̌v�Z
    double master_x = cal_fpx(master_J1, master_J2, master_J3);
    double master_y = cal_fpy(master_J1, master_J2, master_J3);
    double master_z = cal_fpz(master_J1, master_J2, master_J3);

    //���͊p�̕ϊ�
    double master_a = master_J1;
    double master_b = (master_J2 - 90.0)* -1;//�@�\�㔽�]���������̂�߂�
    double master_c = master_J2 + master_J3 - 90.0;

    printf("\n\n\n�����ʒu�͈ȉ��̒ʂ�ł��D\n");
    printf("x,y,z=[%lf,%lf,%lf]\n\n", master_x,master_y, master_z);
    printf("�����p�͈ȉ��̒ʂ�ł��D\n");
    printf("J1,J2,J3=[%lf,%lf,%lf]\n\n", master_J1, master_J2, master_J3);
    printf("�����p�͈ȉ��̒ʂ�ł��D\n");
    printf("a,b,c=[%lf,%lf,%lf]\n\n", master_a, master_b, master_c);

    int t = 0;
    while (1)
    {
       printf("==============================================\n");
       printf("\n�ڕW�l���w�肵�Ă�������\n");
       posi.x = (_getch()-'0')   * 0.01;
       posi.y = (_getch() - '0') * 0.01;
       posi.z = (_getch() - '0') * 0.01;

       //�����ʒu����̕ψʂ�pos�ɑ��
       //->�t�^���w�v�Z�ɂ͐��E���W�n���猩���ʒu�𑫂��Z����D

       //���̕ψʂ̒P�ʂ͉��H
       printf("�w�肵�������ʒu����̕ψʂ͈ȉ��̒ʂ�ł�\n");
       printf("x,y,z=%lf,%lf,%lf\n\n", posi.x, posi.y, posi.z);
       double a, b, c;
       double j1, j2, j3;
       double x, y, z;

       //�w�肵���ψʁ�scale�@�{�@�����ʒu
       x = posi.x * scale + master_x;
       y = posi.y * scale + master_y;
       z = posi.z * scale + master_z;
       printf("���E���W�n�ł̈ʒu�͈ȉ��̒ʂ�ł�\t�������C�X�P�[�����%.4lf�ł�\n",scale);
       printf("x,y,z=%lf,%lf,%lf\n\n", x, y, z);

       //�t�^���w�̌v�Z
       j1 = cal_J1(x, y , z );
       j3 = cal_J3(x, y , z,j1);
       j2 = cal_J2(x, y , z,j1, j3);
       printf("j1,j2,j3=[%lf,%lf,%lf]\n\n", j1, j2, j3);
       
       

       a = j1-master_a;
       b = (j2 - 90.0)*(-1) - master_b;//�@�\��̕␳
       c = j3 + j2 - 90.0 - master_c;
       
       //a = master_a - j1;
       //b = master_b - (j2 - 90);
       //c = master_c - (j3 + j2);

       
       printf("�ڕW�ʒu�֕ψʂ����邽�߂ɓ��͂���p�x�͎��̒ʂ�ł�\n");
       printf("a,b,c=[%.3lf , %.3lf , %.3lf]\n\n", a, b, c);

       //�o�b�t�@�[���܂�
       a = a / 1.0;
       b = b / 0.85;
       c = c / 0.9;
       //send(a - a_mini);
       //send(b - b_mini);
       //send(c - c_mini);
       //send(0 - d_mini);
       //send(0 - e_mini);
       //send(0 - f_mini);
       printf("==============================================\n\n\n");

     }
    return 0;

}
