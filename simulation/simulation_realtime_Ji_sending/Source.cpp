//2020/08/27 TomokiHirayama
//
//this program is calcurating Inverse Kinematics(IK)
//and save angle data into .csv file
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>

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

//arm's parameter(mm)
double a_1 = 26.50;
double a_2 = 120.03;
double a_3 = 19.92;
double d_4 = 123.08;
//decide scale
double scale;
//xy->movespace=0 yz->movespace=1 zx->movespace=2
int movespace;

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
double cal_J1(Pos pos) {
    double px = pos.x;
    double py = pos.y;
    double J1 = rad2deg(atan2(py, px));
    return J1;
}
double cal_J2(Pos pos,deg J) {
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
        //return EOF; <- 
        //zenkai tukatta atai to onaji atai wo return suru, kai no rireki
        //if (/*zennkai no J2 */)
        //    ;
        return J2_1;
    }
    else if ((J2_1 < -150.0 || J2_1 > -80.0) && (J2_2 < -150.0 || J2_2 > -80.0))
    {
        return EOF;
    }

    // return J2_1;

}
double cal_J3(Pos pos,deg J) {
    double J1 = deg2rad(J.j1);
    double px = pos.x;
    double py = pos.y;
    double pz = pos.z;
    double tmpA = -1 * a_1 + px * C(J1) + py * S(J1);


    double bunbo = tmpA * tmpA + 2 * tmpA * d_4 - a_2 * a_2 + a_3 * a_3 + 2 * a_3 * pz + d_4 * d_4 + pz * pz;
    double ruto = -1 * tmpA * tmpA * tmpA * tmpA +
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
        pz * pz * pz * pz;



    //printf("ruto=%lf\n", ruto);
    double J3_1 = rad2deg(2 * atan2(-2 * tmpA * a_3 + 2 * d_4 * pz + sqrt(ruto), bunbo));
    double J3_2 = rad2deg(-2 * atan2(2 * tmpA * a_3 - 2 * d_4 * pz + sqrt(ruto), bunbo));

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

    if ((J3_1 <= 90 && J3_1 >= -15) && (J3_2 > 90 || J3_2 < -15))
    {
        return J3_1;
    }
    else if ((J3_2 <= 90 && J3_2 >= -15) && (J3_1 > -90 || J3_1 < -15))
    {
        return J3_2;
    }
    else if ((J3_1 > 90 || J3_1 < -15) && (J3_2 > 90 || J3_2 < -15))
    {
        return EOF;
    }
    else if ((J3_1 <= 90 || J3_1 >= -15) && (J3_2 <= 90 || J3_2 >= -15))
    {
        return EOF;
    }
}

//define Frist Place
deg iniJ()
{
    deg J;
    J.j1 = 0.0;
    J.j2 = -120.0;
    J.j3 = 30.0;
    return J;
}

//=====================calculate Now Position=======================
Pos cal_nowpos(Pos csvpos, Pos firstpos,double scale)
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
deg cal_invkin(Pos pos)
{
    deg J;
    J.j1 = cal_J1(pos);
    J.j3 = cal_J3(pos,J);
    J.j2 = cal_J2(pos,J);
    return J;
}

//
void read_cal_J123(FILE* fp1,FILE* fp2)
{
    deg master_J,posJ,csvJ;//,into csv
    Pos FirstPos,csvPos,nowPos;
    //calculate FirstPlace
    master_J = iniJ();
    FirstPos = cal_kin(master_J);
    //scan
    for (int i = 0; i < 2458; i++)
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
        posJ= cal_invkin(nowPos);
        
        //calculate the difference between angles input to the servo
        csvJ.j1 = posJ.j1 - master_J.j1;
        csvJ.j2 = posJ.j2 - master_J.j2;
        csvJ.j3 = posJ.j3 - master_J.j3;

        //Writing.
        fprintf(fp2, "%lf,%lf,%lf\n", csvJ.j1, csvJ.j2, csvJ.j3);
    }
}

//create savename
std::string setname(int i)
{
    std::string root = ".csv";
    std::string name = "deg123data_scale_";
    std::string space,scalesize;
    // Order of names created... 1/4xy,yz,zx 1/8xy,yz,zx  1/16xy,yz,zx
    // by using "if" ,"%",and "/"
    if (i % 3 == 0) {
        space = "xy"; 
        movespace = 0;
    }
    else if (i % 3 == 1) {
        space = "yz";
        movespace = 1;
    }
    else {
        space = "zx";
        movespace = 2;
    }
    if (i / 3 == 0) {
        scalesize = "1_4";
        scale = 0.25;
    }
    else if (i / 3 == 1) {
        scalesize = "1_8"; 
        scale = 0.125;
    }
    else {
        scalesize = "1_16";
        scale = 0.0625;
    }

    std::string savename = name + scalesize + space + root;
    //std::cout << savename << std::endl;
    //printf("%sn", savename.c_str());
    return savename;
}

int main(void)
{
    FILE* fp1,*fp2;
    errno_t error_fp;
    const char* filename = "sweep.csv";
    for (int i = 0; i < 9; i++)
    {
        printf("%d\n", i);
        error_fp = fopen_s(&fp1, filename, "r");
        if (fp1==NULL)
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
            printf("%s\n", savename);
            error_fp = fopen_s(&fp2, savename, "w");
            if (fp2 == NULL)
            {
                printf("fp2 error!");
                exit(0);
            }
            else {
                printf("fp2 Open OK\n");
                //header
                fprintf(fp2, "J1,J2,J3\n");
                printf("header writing OK\n");
                read_cal_J123(fp1, fp2);
                printf("read_cal_J123 OK\n");
                fclose(fp1);
                fclose(fp2);
                //free(fp2);
            }
        }
    }
    return 0;
}
