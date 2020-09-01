//2020/08/27 TomokiHirayama
//Aug 28, 2020 Masahiro Furukawa modified
//
//this program is calcurating Inverse Kinematics(IK)
//and save angle data into .csv file
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>

#include "QueryPerformanceTimer.h"
#include "mbed_Serial_binary.h"
#include "Deg_List.h"

//=================================================Function Definition=======================================================

//======PI=====
#define PI  3.14159265358979
//degree struct
struct deg {
    double j1;
    double j2;
    double j3;
};
//position struct
struct Pos {
    double x;
    double y;
    double z;
};

#define Point 8192
#define f_min 0.200
#define Sampling_rate 60.0
#define firstscicle 5.0

//arm's parameter(mm)
double a_1 = 26.50;
double a_2 = 120.03;
double a_3 = 19.92;
double d_4 = 169.28;

//decide scale
double scale = 0.0;
//xy->movespace=0 yz->movespace=1 zx->movespace=2
int movespace;

// degree list
// defined in "Deg_List.h"
Deg_List dl;

// binary communication
// defined in "mbed_Serial_binary.h"
#define NUM_OF_HEADER 2
#define NUM_OF_BUF 26
Com_Binary com;
unsigned char sbuf[4 * 6 + NUM_OF_HEADER];

// global timer
// defined in "QueryPerformanceTimer.h"
QueryPerformanceTimer qpTime;

//radian to degree
double rad2deg(double rad) {
    return rad / PI * 180.0;
}
//degree to radian
double deg2rad(double deg) {
    return deg / 180.0 * PI;
}
//Cos,Sin
double C(double a) {
    return cos(a);
}
double S(double a) {
    return sin(a);
}


//===========================================Kinematics : Calcurate Hand position from J1J2J3================
double cal_fpx(deg J)
{
    double J1 = deg2rad(J.j1);
    double J2 = deg2rad(J.j2);
    double J3 = deg2rad(J.j3);

    return (a_1 - a_2 * S(J2) - a_3 * S(J3) + d_4 * C(J3)) * C(J1);
}
double cal_fpy(deg J)
{
    double J1 = deg2rad(J.j1);
    double J2 = deg2rad(J.j2);
    double J3 = deg2rad(J.j3);

    return (a_1 - a_2 * S(J2) - a_3 * S(J3) + d_4 * C(J3)) * S(J1);
}
double cal_fpz(deg J)
{
    double J1 = deg2rad(J.j1);
    double J2 = deg2rad(J.j2);
    double J3 = deg2rad(J.j3);

    return a_2 * C(J2) + a_3 * C(J3) + d_4 * S(J3);
}


//===========================================Inverse Kinematics : Calcurate J1J2J3 from Hand position ================
deg bfdegJ;

double cal_J1(Pos pos) {
    double px = pos.x;
    double py = pos.y;
    double J1;
    if (px == 0.0)
    {
        //if (py > 0)
        //{
            return rad2deg(PI / 2);
        //}
        //else
        //{
            //return rad2deg(- PI / 2);
        //}
    }
    else if (py == 0.0)
    {
        return 0;
    }
    else
    {
        J1 = rad2deg(atan2(py, px));
        return rad2deg(atan2(py, px));
    }
}
double cal_J2(Pos pos, deg J, deg bfdeg) {
    double J1 = deg2rad(J.j1);
    double J3 = deg2rad(J.j3);
    double px = pos.x;
    double py = pos.y;
    double pz = pos.z;

    double  tmpA = -1 * a_1 + px * C(J1) + py * S(J1);
    double J2_1 = -1 * rad2deg(-1 * acos(-1 * (a_3 * C(J3) + d_4 * S(J3) - pz) / a_2));//+360;
    double J2_2 = -1 * rad2deg(acos(-1 * (a_3 * C(J3) + d_4 * S(J3) - pz) / a_2));

    //check
    //printf("J2_1,J2_2=%lf , %lf\n", J2_1, J2_2);
    //if (J2_1 > 180.0) { J2_1 = J2_1 - 180.0; }
    //else if (J2_1 < -180.0) { J2_1 = J2_1 + 360; }
    //if (J2_2 > 180.0) { J2_2 = J2_2 - 180.0; }
    //else if (J2_2 < -180.0) { J2_2 = J2_2 + 360; }
    //printf("J2_1,J2_2=%lf , %lf\n", J2_1, J2_2);

    //Conditional Branch
    if ((J2_1 >= -150.0 && J2_1 <= -80.0) && (J2_2 < -150.0 || J2_2 > -80.0))
    {
        return J2_1;
    }
    else if ((J2_2 >= -150.0 && J2_2 <= -80.0) && (J2_1 < -150.0 || J2_1 > -80.0))
    {
        return J2_2;
    }
    else if ((J2_1 >= -150.0 && J2_1 <= -80.0) && (J2_2 >= -150.0 && J2_2 <= -80.0))
    {
        //両方とも解の場合は一つ前のデータを用いる
        if (abs(bfdeg.j2 - J2_1) < abs(bfdeg.j2 - J2_2))
        {
            return J2_1;
        }
        else
        {
            return J2_2;
        }
    }
    else if ((J2_1 < -150.0 || J2_1 > -80.0) && (J2_2 < -150.0 || J2_2 > -80.0))
    {
        return EOF;
    }

    // return J2_1;

}
double cal_J3(Pos pos, deg J , deg bfdeg) {
    double J1 = deg2rad(J.j1);
    double px = pos.x;
    double py = pos.y;
    double pz = pos.z;
    double tmpA = -1 * a_1 + px * C(J1) + py * S(J1);

    double L = -1*a_1 + px * C(J1) + py * sin(J1);
    double J3_1 = rad2deg( 2 * atan((-2 * L * a_3 + 2 * d_4 * pz + sqrt(-pow(L, 4) + 2 * pow(L, 2) * pow(a_2, 2) + 2 * pow(L, 2) * pow(a_3, 2) + 2 * pow(L, 2) * pow(d_4, 2) - 2 * pow(L, 2) * pow(pz, 2) - pow(a_2, 4) + 2 * pow(a_2, 2) * pow(a_3, 2) + 2 * pow(a_2, 2) * pow(d_4, 2) + 2 * pow(a_2, 2) * pow(pz, 2) - pow(a_3, 4) - 2 * pow(a_3, 2) * pow(d_4, 2) + 2 * pow(a_3, 2) * pow(pz, 2) - pow(d_4, 4) + 2 * pow(d_4, 2) * pow(pz, 2) - pow(pz, 4))) / (pow(L, 2) + 2 * L * d_4 - pow(a_2, 2) + pow(a_3, 2) + 2 * a_3 * pz + pow(d_4, 2) + pow(pz, 2))));
    double J3_2 = rad2deg(-2 * atan(( 2 * L * a_3 - 2 * d_4 * pz + sqrt(-pow(L, 4) + 2 * pow(L, 2) * pow(a_2, 2) + 2 * pow(L, 2) * pow(a_3, 2) + 2 * pow(L, 2) * pow(d_4, 2) - 2 * pow(L, 2) * pow(pz, 2) - pow(a_2, 4) + 2 * pow(a_2, 2) * pow(a_3, 2) + 2 * pow(a_2, 2) * pow(d_4, 2) + 2 * pow(a_2, 2) * pow(pz, 2) - pow(a_3, 4) - 2 * pow(a_3, 2) * pow(d_4, 2) + 2 * pow(a_3, 2) * pow(pz, 2) - pow(d_4, 4) + 2 * pow(d_4, 2) * pow(pz, 2) - pow(pz, 4))) / (pow(L, 2) + 2 * L * d_4 - pow(a_2, 2) + pow(a_3, 2) + 2 * a_3 * pz + pow(d_4, 2) + pow(pz, 2))));




    //double bunbo = tmpA * tmpA + 2 * tmpA * d_4 - a_2 * a_2 + a_3 * a_3 + 2 * a_3 * pz + d_4 * d_4 + pz * pz;
    /*double ruto = -1 * tmpA * tmpA * tmpA * tmpA +
        2 * tmpA * tmpA * a_2 * a_2 +
        2 * tmpA * tmpA * a_3 * a_3 +
        2 * tmpA * tmpA * d_4 * d_4 -
        2 * tmpA * tmpA * pz * pz -
        a_2 * a_2 * a_2 * a_2 +
        2 * a_2 * a_2 * a_3 * a_3 +
        2 * a_2 * a_2 * d_4 * d_4 +
        2 * a_2 * a_2 * pz * pz -
        a_3 * a_3 * a_3 * a_3 -
        2 * a_3 * a_3 * d_4 * d_4 +
        2 * a_3 * a_3 * pz * pz -
        d_4 * d_4 * d_4 * d_4 +
        2 * d_4 * d_4 * pz * pz -
        pz * pz * pz * pz;*/



    //printf("ruto=%lf\n", ruto);
    //double J3_1 = rad2deg(2 * atan2(-2 * tmpA * a_3 + 2 * d_4 * pz + sqrt(ruto), bunbo));
    //double J3_2 = rad2deg(-2 * atan2(2 * tmpA * a_3 - 2 * d_4 * pz + sqrt(ruto), bunbo));

    //printf("ruto:%lf\n", a_3 * a_3 + d_4 * d_4 - pz * pz + 2 * pz - 1);
    //double J3_1 = rad2deg( 2 * atan2(d_4 - sqrt(a_3 * a_3 + d_4 * d_4 - pz * pz + 2 * pz - 1), a_3 + pz - 1));
    //double J3_2 = rad2deg(-2 * atan2(d_4 + sqrt(a_3 * a_3 + d_4 * d_4 - pz * pz + 2 * pz - 1), a_3 + pz - 1));

    //

    //printf("J3_1,J3_2=[%lf,%lf]\n", J3_1, J3_2);
    if (J3_1 > 180.0) { J3_1 = J3_1 - 180.0; }
    else if (J3_1 < -180.0) { J3_1 = J3_1 + 360; }
    if (J3_2 > 180.0) { J3_2 = J3_2 - 180.0; }
    else if (J3_2 < -180.0) { J3_2 = J3_2 + 360; }
    //printf("J3_1,J3_2=[%lf,%lf]\n", J3_1, J3_2);

    if ((J3_1 <= 90.0 && J3_1 >= -15.0) && (J3_2 > 90.0 || J3_2 < -15.0))
    {
        return J3_1;
    }
    else if ((J3_2 <= 90.0 && J3_2 >= -15.0) && (J3_1 > -90.0 || J3_1 < -15.0))
    {
        return J3_2;
    }
    else if ((J3_1 > 90.0 || J3_1 < -15.0) && (J3_2 > 90.0 || J3_2 < -15.0))
    {
        return EOF;
    }
    else if ((J3_1 <= 90.0 || J3_1 >= -15.0) && (J3_2 <= 90.0 || J3_2 >= -15.0))
    {
        //両方とも解の場合は一つ前のデータを用いる
        if (abs(bfdeg.j3 - J3_1) < abs(bfdeg.j3 - J3_2))
        {
            return J3_1;
        }
        else
        {
            return J3_2;
        }
    }
}

//define Frist Place
deg iniJ()
{
    deg J;
    J.j1 = 0.0;
    J.j2 = -135.0;//-120.0;
    J.j3 = 45.0;//30.0;
    return J;
}

//=====================calculate Now Position=======================
Pos cal_nowpos(Pos csvpos, Pos firstpos, double scale)
{
    Pos pos;
    pos.x = firstpos.x + csvpos.x * scale;
    pos.y = firstpos.y + csvpos.y * scale;
    pos.z = firstpos.z + csvpos.z * scale;
    return pos;
}

//==========================Kinematics Set==============================
Pos cal_kin(deg J)
{
    Pos xyz;
    xyz.x = cal_fpx(J);
    xyz.y = cal_fpy(J);
    xyz.z = cal_fpz(J);
    return xyz;
}

//==========================Inverse Kinematics Set============================
deg cal_invkin(Pos pos,deg bfdeg)
{
    deg J;
    J.j1 = cal_J1(pos);
    J.j3 = cal_J3(pos, J,bfdeg);
    J.j2 = cal_J2(pos, J,bfdeg);
    return J;
}

//
void read_cal_J123(FILE* fp1, FILE* fp2,FILE*fp3,deg bfdeg)
{
    deg master_J, posJ, csvJ;//,into csv
    Pos FirstPos, csvPos, nowPos;
    //calculate FirstPlace
    master_J = iniJ();
    FirstPos = cal_kin(master_J);
    //scan
    for (int i = 0; i < Point+ int(firstscicle / f_min) * Sampling_rate; i++)
    {
        //Decide which plane to turn
        if (movespace == 0)
        {
            fscanf_s(fp1, "%lf,%lf\n", &csvPos.x, &csvPos.y);
            csvPos.z = 0.0;
        }
        else if (movespace == 1)
        {
            fscanf_s(fp1, "%lf,%lf\n", &csvPos.y, &csvPos.z);
            csvPos.x = 0.0;
        }
        else
        {
            fscanf_s(fp1, "%lf,%lf\n", &csvPos.z, &csvPos.x);
            csvPos.y = 0.0;
        }

        //calculate Now Position
        nowPos = cal_nowpos(csvPos, FirstPos, scale);

        //calculate Inverse Kinematics
        posJ = cal_invkin(nowPos,bfdeg);

        //calculate the difference between angles input to the servo
        csvJ.j1 = posJ.j1 - master_J.j1;
        csvJ.j2 = posJ.j2 - master_J.j2;
        csvJ.j3 = posJ.j3 - master_J.j3;
        //printf("%lf\n", posJ.j3);
        //Writing.
        fprintf(fp2, "%lf,%lf,%lf\n", csvJ.j1, csvJ.j2, csvJ.j3);
        fprintf(fp3, "%lf,%lf,%lf\n", nowPos.x,nowPos.y,nowPos.z);

        // store angle list
        //if first, hand should go to orbit.
        if (i == 0)
        {
            for (int ii = 0; ii < 10; ii++)
            {
                dl.add(csvJ.j1, csvJ.j2, csvJ.j3);
            }
        }
        dl.add(csvJ.j1, csvJ.j2, csvJ.j3);
    }
}

void realtime_angle_sending() {
    int ret;
    double j1, j2, j3;
    unsigned char sbuf[4 * 6 + NUM_OF_HEADER];

    printf("\n\n ... realtime_angle_sending ...\n\n");

    // initialize pointer as zero
    dl.setStart();

    // header
    sbuf[0] = '*';
    sbuf[1] = '+';

    // angular sequence trail
    do {
        // refer angular sequence
        ret = dl.getNext(&j1, &j2, &j3);

        // make byte list
        com.float2byte(&sbuf[0 + NUM_OF_HEADER], (float)j1);
        com.float2byte(&sbuf[4 + NUM_OF_HEADER], (float)j2);
        com.float2byte(&sbuf[8 + NUM_OF_HEADER], (float)j3);
        com.float2byte(&sbuf[12 + NUM_OF_HEADER], 0.0f);
        com.float2byte(&sbuf[16 + NUM_OF_HEADER], 0.0f);
        com.float2byte(&sbuf[20 + NUM_OF_HEADER], 0.0f);

        // send via serial port
        com.send_bytes(sbuf, NUM_OF_BUF);

        // constant time loop
        qpTime.wait();

    } while (ret != -1);
}

//create savename
std::string setname(int i)
{
    std::string root = ".csv";
    std::string name = "retestr200f80f02newdeg123data_scale_";
    std::string space, scalesize;
    // Order of names created... 1/4xy,yz,zx 1/8xy,yz,zx  1/16xy,yz,zx
    // by using "if" ,"%",and "/"
    if (i % 3 == 1) {
        space = "xy";
        movespace = 0;
    }
    else if (i % 3 == 0) {
        space = "yz";
        movespace = 1;
    }
    else {
        space = "zx";
        movespace = 2;
    }
    if (i / 3 == 0) {
        scalesize = "1_2";
        scale = 0.5;// 1.0 / 4.0;
    }
    else if (i / 3 == 1) {
        scalesize = "1_6";
        scale = 1.0 / 6.0;// 1.0 / 8.0;
    }
    else {
        scalesize = "1_12";
        scale = 1.0 / 12.0;// 1.0 / 16.0;
    }

    std::string savename = name + scalesize + space + root;
    //std::cout << savename << std::endl;
    //printf("%sn", savename.c_str());
    return savename;
}
//create savename
std::string saveposname(int i)
{
    std::string root = ".csv";
    std::string name = "retestr200f80f02newposxydata_scale_";
    std::string space, scalesize;
    // Order of names created... 1/4xy,yz,zx 1/8xy,yz,zx  1/16xy,yz,zx
    // by using "if" ,"%",and "/"
    if (i % 3 == 1) {
        space = "xy";
        movespace = 0;
    }
    else if (i % 3 == 0) {
        space = "yz";
        movespace = 1;
    }
    else {
        space = "zx";
        movespace = 2;
    }
    if (i / 3 == 0) {
        scalesize = "1_2";
        scale = 0.5;// 1.0 / 4.0;
    }
    else if (i / 3 == 1) {
        scalesize = "1_6";
        scale = 1.0/6.0;// 1.0 / 8.0;
    }
    else {
        scalesize = "1_12";
        scale = 1.0/12.0;// 1.0 / 16.0;
    }

    std::string savename = name + scalesize + space + root;
    //std::cout << savename << std::endl;
    //printf("%sn", savename.c_str());
    return savename;
}

int main(void)
{

    // timer
    qpTime.setFps(60);
    qpTime.wait();

    FILE* fp1, * fp2,*fp3;
    errno_t error_fp;

    //1/1の時に200㎜で行おう
    const char* filename = "r160_f80_f02_P81newsweep.csv";


    //set before degree
    bfdegJ.j1 = 0.0;
    bfdegJ.j2 = 0.0;
    bfdegJ.j3 = 0.0;

    //for (int i = 0; i < 9; i++)
    for(int i=0;i<9;i++)
    {

        // angle list to realize realtime angle sending
        dl.init();

        printf("%d\n", i);
        error_fp = fopen_s(&fp1, filename, "r");
        if (fp1 == NULL)
        {
            printf("fp1 error!");
            exit(0);
        }
        else
        {
            printf("fp1 open OK\n");

            //create savename
            std::string tmp = setname(i);
            const char* savename = tmp.c_str();
            printf("savename %s\n", savename);

            error_fp = fopen_s(&fp2, savename, "w");

            if (fp2 == NULL)
            {
                printf("fp2 error!");
                exit(0);
            }
            else {
                printf("fp2 Open OK\n");
                
                std::string tmp2 = saveposname(i);
                const char* saveposname = tmp2.c_str();
                error_fp = fopen_s(&fp3, saveposname, "w");

                if (fp3 == NULL)
                {
                    printf("fp3 error!");
                    exit(0);
                }
                else 
                {
                    printf("fp3 Open OK\n");
                    //header
                    fprintf(fp2, "J1,J2,J3\n");
                    fprintf(fp3, "x,y,z\n");
                    printf("header writing OK\n");
                    read_cal_J123(fp1, fp2, fp3,bfdegJ);
                    printf("read_cal_J123 OK\n");
                    fclose(fp1);
                    fclose(fp2);
                    fclose(fp3);
                    //free(fp2);
                }
            }
        }


        // start realtime angle sending
        realtime_angle_sending();

    }
    return 0;
}