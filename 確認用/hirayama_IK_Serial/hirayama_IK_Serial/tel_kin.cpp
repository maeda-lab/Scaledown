#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>

#include"tel_kin.h"

//=================================================逆運動学=======================================================
#define PI  3.14159265358979
//アームのパラメータ
//単位はすべてミリメートル(mm)
#define g  26.50//0.03078
#define h  102.0//0.102
#define j  120.03//0.12003
#define k  19.92//0.01992
#define l  123.08//0.12008
#define m  16.0//0.016
#define n  26.0//0.026


//それぞれの角の範囲
#define a_min  -120.0
#define a_max  90.0
#define b_min  -73.0
#define b_max  90.0
#define c_min  -3.0
#define c_max  130.0
#define d_min  -90.0
#define d_max  115.0
#define e_min  -100.0
#define e_max  85.0
#define f_min  -100.0
#define f_max  90.0


double a_1 = g;
double a_2 = j;
double a_3 = k;
double d_4 = l;

//座標変換行列
double Trans[4][4];


double rad2deg(double rad) {
    return rad / PI * 180.0 ;
}
//度数からラジアンに
double deg2rad(double deg) {
    return deg / 180.0 * PI;
}
//Cos,Sinの略記
double C(double a) {
    return cos(a);
}
double S(double a) {
    return sin(a);
}

//J1J2J3から手先位置を計算する順運動学
double cal_fpx(double j1, double j2, double j3)
{
    double J1 = deg2rad(j1);
    double J2 = deg2rad(j2);
    double J3 = deg2rad(j3);

    return (a_1 - a_2 * S(J2) - a_3 * S(J3) + d_4 * C(J3)) * C(J1);
}
double cal_fpy(double j1, double j2, double j3)
{
    double J1 = deg2rad(j1);
    double J2 = deg2rad(j2);
    double J3 = deg2rad(j3);

    return (a_1 - a_2 * S(J2) - a_3 * S(J3) + d_4 * C(J3)) * S(J1);
}
double cal_fpz(double j1, double j2, double j3)
{
    double J1 = deg2rad(j1);
    double J2 = deg2rad(j2);
    double J3 = deg2rad(j3);

    return a_2 * C(J2) + a_3 * C(J3) + d_4 * S(J3);
}


double cal_J1(double x,double y,double z){
    double px = x;
    double py = y;
    double J1 = rad2deg(atan2(py, px));
    return J1;
}
double cal_J2(double x, double y, double z,double j1,double j3) {
    double J1 = deg2rad(j1);
    double J3 = deg2rad(j3);
    double px = x;
    double py = y;
    double pz = z;

    double  tmpA = -1 * a_1 + px * C(J1) + py * S(J1);
    //double J2 = rad2deg(atan2(tmpA + a_3 * S(J3) - d_4 * C(J3), a_2 * (-1 * a_3 * C(J3) - d_4 * S(J3) + pz)));
    double J2_1 = -1 * rad2deg(-1 * acos(-1 * (a_3 * C(J3) + d_4 * S(J3) - pz) / a_2));//+360;
    double J2_2 = -1 * rad2deg(      acos( -1 * (a_3 * C(J3) + d_4 * S(J3) - pz) / a_2));
    
    //printf("J2_1,J2_2=%lf , %lf\n", J2_1, J2_2);

    //if (J2_1 > 180.0) { J2_1 = J2_1 - 180.0; }
    //else if (J2_1 < -180.0) { J2_1 = J2_1 + 360; }
    //if (J2_2 > 180.0) { J2_2 = J2_2 - 180.0; }
    //else if (J2_2 < -180.0) { J2_2 = J2_2 + 360; }
    //printf("J2_1,J2_2=%lf , %lf\n", J2_1, J2_2);



    //条件分岐
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
double cal_J3(double x, double y, double z,double j1){
    double J1 = deg2rad(j1);
    double px = x;
    double py = y;
    double pz = z;
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
    double J3_1 = rad2deg( 2 * atan2(-2 * tmpA * a_3 + 2 * d_4 * pz + sqrt(ruto), bunbo));
    double J3_2 = rad2deg(-2 * atan2( 2 * tmpA * a_3 - 2 * d_4 * pz + sqrt(ruto), bunbo));

    //printf("ルートの中身:%lf\n", a_3 * a_3 + d_4 * d_4 - pz * pz + 2 * pz - 1);
    //double J3_1 = rad2deg( 2 * atan2(d_4 - sqrt(a_3 * a_3 + d_4 * d_4 - pz * pz + 2 * pz - 1), a_3 + pz - 1));
    //double J3_2 = rad2deg(-2 * atan2(d_4 + sqrt(a_3 * a_3 + d_4 * d_4 - pz * pz + 2 * pz - 1), a_3 + pz - 1));

    //解の調整

    //printf("J3_1,J3_2=[%lf,%lf]\n", J3_1, J3_2);
    if (J3_1 > 180.0) { J3_1 = J3_1 - 180.0; }
    else if (J3_1 < -180.0) { J3_1 = J3_1 + 360; }
    if (J3_2 > 180.0) { J3_2 = J3_2 - 180.0; }
    else if (J3_2 < -180.0) { J3_2 = J3_2 + 360; }
    //printf("J3_1,J3_2=[%lf,%lf]\n", J3_1, J3_2);



    //個々の条件分岐おかしい
    if ((J3_1 <= 90 && J3_1 >= -15) && (J3_2 > 90 || J3_2<-15))
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
