#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>

#include"tel_kin.h"

//=================================================逆運動学=======================================================
#define PI  3.14159265358979
//アームのパラメータ
//単位はすべてメートル(m)
#define g  0.03078
#define h  0.102
#define j  0.12003//0.120
#define k  0.01992//0.020
#define l  0.12008
#define m  0.016
#define n  0.026


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

//クォータニオン→座標変換行列の計算
void cal_T(double x, double y, double z, double qx, double qy, double qz, double qw) {

    //printf("qx=%f\nqy=%f\nqz=%f\nqw=%f\n\n", qx, qy, qz, qw);
    //確認済み↑

/*    Trans[0][0] = qx * qx - qy * qy - qz * qz + qw * qw;//nx
    Trans[1][0] = 2.0 * qx * qy + 2.0 * qz * qw;//ny
    Trans[2][0] = 2.0 * qx * qz - 2.0 * qy * qw;//nz
    Trans[3][0] = 0.0;//0

    Trans[0][1] = 2.0 * qx * qy + 2.0 * qz * qw;//ox
    Trans[1][1] = -qx * qx + qy * qy - qz * qz + qw * qw;//oy
    Trans[2][1] = 2.0 * qy * qz + 2.0 * qx * qw;//oz
    Trans[3][1] = 0.0;//0

    Trans[0][2] = 2.0 * qx * qz + 2.0 * qy * qw;//ax
    Trans[1][2] = 2.0 * qy * qz - 2.0 * qx * qw;//ay
    Trans[2][2] = -qx * qx - qy * qy + qz * qz + qw * qw;//az
    Trans[3][2] = 0.0;//0

    Trans[0][3] = x;//px
    Trans[1][3] = y;//py
    Trans[2][3] = z;//pz
    Trans[3][3] = 1.0;//1*/
    Trans[0][0] = qx* qx - qy * qy - qz * qz + qw * qw;//nx
    Trans[1][0] = 2.0 * qx * qy + 2.0 * qz * qw;//ny
    Trans[2][0] = 2.0 * qx * qz - 2.0 * qy * qw;//nz
    Trans[3][0] = 0.0;//0

    Trans[0][1] = 2.0 * qx * qy - 2.0 * qz * qw;//ox
    Trans[1][1] = -qx * qx + qy * qy - qz * qz + qw * qw;//oy
    Trans[2][1] = 2.0 * qy * qz + 2.0 * qx * qw;//oz
    Trans[3][1] = 0.0;//0

    Trans[0][2] = 2.0 * qx * qz + 2.0 * qy * qw;//ax
    Trans[1][2] = 2.0 * qy * qz - 2.0 * qx * qw;//ay
    Trans[2][2] = -qx * qx - qy * qy + qz * qz + qw * qw;//az
    Trans[3][2] = 0.0;//0

    Trans[0][3] = x;//px
    Trans[1][3] = y;//py
    Trans[2][3] = z;//pz
    Trans[3][3] = 1.0;//1

}
double cal_a() {

    double x = -(m + n) * Trans[0][2] + Trans[0][3];
    double y = -(m + n) * Trans[1][2] + Trans[1][3];
    double a = rad2deg(atan2(y, x));
    //printf("x,y,a=%lf,%lf,%lf\n", x,y,a);
    if (x == -0 || y == 0)
    {
        printf("y=0(a)\n");
        return 0;
    }
    else {}
    if (a_min > a || a_max < a)
    {
        printf("x,y,a=%lf,  %lf, %lf\n", x, y, a);
        printf("atan2error in a : No answer\n");
        return EOF;
    }

    return a;
}

double cal_b(double a)   {
  /*double A = Trans[2][3] - (m + n) * Trans[2][0] - h;
    double B = Trans[0][3] * C(a) + Trans[1][3] * S(a) - (m + n) * (Trans[0][0] * C(a) + Trans[1][0] * S(a)) - g;
    double D = (A * A + B * B + j * j - k * k - l * l) / (2 * j);
    double b1 = rad2deg(2 * atan2(-B + sqrt(B * B + A * A - D * D), A + D));
    double b2 = rad2deg(2 * atan2(-B - sqrt(B * B + A * A - D * D), A + D));

    //エラーが起きる理由：sqrtの中身が負になっている，なる場合がある

    //printf("sqrt(D=b^2-4ac)=%lf\n" ,- B + sqrt(B * B + A * A - D * D));

    if (b1 > 180.0) { b1 = b1 - 180.0; }
    else if (b1 < -180.0) { b1 = b1 + 360; }
    if (b2 > 180.0) { b2 = b2 - 180.0; }
    else if (b2 < -180.0) { b2 = b2 + 360; }

    //printf("A=%lf\nB=%lf\nD=%lf\nb1=%lf\nb2=%lf\n\n", A, B, D, b1, b2);

    //解が二つでているので範囲外ならはじく
    if ((b_min<=b1 && b_max>=b1) && (b_min > b2 || b_max < b2)) {
        return b1;
    }
    else if ((b_min<=b2 && b_max>=b2) && (b_min > b1 || b_max < b1)) {
        return b2;
    }
    else if ((b_min<b1 && b_max>b1) && (b_min<b2 && b_max>b2))
    {//両方とも解になりうる場合,とりあえずエラー
        printf("answer error in b : two answer\n");
        return EOF;
    }
    else if ((b_min > b1 || b_max < b1) && (b_min > b2 || b_max < b2))
    {//答えがない場合
        printf("atan2error in b：No answer\n");
        return EOF;
    }
    else
    {
        printf("error in b:なんかおかしい．\n");
        return EOF;
    }*/

    double A = Trans[2][3] - (m + n) * Trans[2][2] - h;
    double B = Trans[0][3] * C(a) + Trans[1][3] * S(a) - (m + n) * (Trans[0][2] * C(a) + Trans[1][2] * S(a)) - g;
    double D = (A * A + B * B + j * j - k * k - l * l) / (2 * j);

    double tmp1, tmp2;
    if (A * A + B * B - D * D == 0 || D == 0)
    {
        tmp1 = 0.0;
    }
    else
    {
        tmp1 = atan2(sqrt(A * A + B * B - D * D), D);
    }

    if (A == 0 || B == 0)
    {
        tmp2 = 0.0;
    }
    else
    {
        tmp2 = atan2(B, A);
    }

    double b = rad2deg(tmp1 - tmp2);


    if (b_min > b || b_max < b)
    {
        printf("atan2error in b : No answer\n");
        return EOF;
    }

    return b;


}
double cal_c(double a) {
    /*double A = Trans[2][3] - (m + n) * Trans[2][0] - h;
    double B = Trans[0][3] * C(a) + Trans[1][3] * S(a) - (m + n) * (Trans[0][0] * C(a) + Trans[1][0] * S(a)) - g;
    double E = (A * A + B * B - j * j - k * k - l * l) / (2 * j);
    double c1 = rad2deg(2 * atan2(-l + sqrt(l * l + k * k - E * E), E + k));
    double c2 = rad2deg(2 * atan2(-l - sqrt(l * l + k * k - E * E), E + k));
    


    //解の調整
    if (c1 > 180.0) { c1 = c1 - 180.0; }
    else if (c1 < -180.0) { c1 = c1 + 360; }
    if (c2 > 180.0) { c2 = c2 - 180.0; }
    else if (c2 < -180.0) { c2 = c2 + 360; }
    
    //解が二つでているので範囲外ならはじく
    if ((c_min<c1 && c_max>c1) && (c_min > c2 || c_max < c2)) {
        return c1;
    }
    else if ((c_min<c2 && c_max>c2) && (c_min > c1 || c_max < c1)) {
        return c2;
    }
    else if ((c_min<c1 && c_max>c1) && (c_min<c2 && c_max>c2))
    {//両方とも解になりうる場合,とりあえずエラー
        printf("answer error in c : two answer\n");
        return EOF;
    }
    else if ((c_min > c1 || c_max < c1) && (c_min > c2 || c_max < c2))
    {//答えがない場合
        printf("atan2error in c：No answer\n");
        return EOF;
    }
    else
    {
        printf("error in c:なんかおかしい．\n");
        return EOF;
    }*/
    double A = Trans[2][3] - (m + n) * Trans[2][2] - h;
    double B = Trans[0][3] * C(a) + Trans[1][3] * S(a) - (m + n) * (Trans[0][2] * C(a) + Trans[1][2] * S(a)) - g;
    double E = (A * A + B * B - j * j - k * k - l * l) / (2 * j);

    double tmp;
    if (l * l + k * k - E * E == 0 || E == 0)
    {
        tmp = 0.0;
    }
    else
    {
        tmp = atan2(E, sqrt(l * l + k * k - E * E));
    }
    double c = rad2deg(tmp - atan2(k, l));




    if (c_min > c || c_max < c)
    {
        printf("atan2error in c : No answer\n");
        return EOF;
    }

    return c;

}
double cal_d(double a, double b, double c) {
    //double x = -1 * (Trans[0][0] * C(a) + Trans[1][0] * S(a)) * S(b + c) + Trans[3][0] * C(b + c);
    //double y = Trans[0][0] * S(a) - Trans[1][0] * S(a);
    /*double x = -1 * (Trans[0][2] * C(a) + Trans[1][2] * S(a)) * S(b + c) + Trans[2][2] * C(b + c);
    double y = Trans[0][2] * S(a) - Trans[1][2] * S(a);

    double d = rad2deg(atan2(y, x));
    if (x == 0 && y == 0)
    {
        printf("atan2 in d : x and y equal 0\n");
    }
    else if (d_min > d || d_max < d)
    {
        printf("atan2error in d : No answer\n");
        return EOF;
    }
    return d;*/

    double x = Trans[2][2] * C(b + c) - (Trans[0][2] * C(a) + Trans[1][2] * S(a)) * S(b + c);
    double y = Trans[0][2] * S(a) - Trans[1][2] * C(a) + 0.00000001;
    double d = rad2deg(atan2(y, x));

    //printf("角度dについて\n");
    printf("x,y,d=%lf,%lf,%lf\n", x, y, d);
    if (x == 0.0 || y == 0.0)
    {
        return 0.0;
    }

    if (d_min > d) { d = d + 180; }
    else if (a_max < d) { d = d - 180; }

    if (d_min > d || d_max < d)
    {
        printf("atan2error in d : No answer\n");
        return EOF;
    }

    return d;


}
double cal_e(double a, double b, double c, double d) {
    //double x = (Trans[0][0] * S(a) + Trans[1][0] * C(a)) * S(d) - ((Trans[0][0] * C(a) + Trans[1][0] * S(a)) * S(b + c) - Trans[3][0] * C(b + c)) * C(d);
    //double y = (Trans[0][0] * C(a) + Trans[1][0] * S(a)) * C(b + c) + Trans[3][0] * S(b + c);
   /* double x = (Trans[0][0] * S(a) - Trans[1][0] * C(a)) * S(d) - ((Trans[0][0] * C(a) + Trans[1][0] * S(a)) * S(b + c) - Trans[2][0] * C(b + c)) * C(d);
    double y = (Trans[0][0] * C(a) + Trans[1][0] * S(a)) * C(b + c) + Trans[2][0] * S(b + c);

    double e = rad2deg(atan2(y, x));
    if (x == 0 && y == 0)
    {
        printf("atan2 in e : x and y equal 0\n");
    }
    else if (e_min > e || e_max < e)
    {
        printf("atan2error in e : No answer\n");
        return EOF;
    }
    return e;

    */
    double tmpA = (Trans[0][3] - g * C(a) + j * C(a) * S(b) + k * C(a) * S(b + c) - l * C(a) * C(b + c)) / (n + m);
    double tmpB = (Trans[1][3] - g * S(a) + j * S(a) * S(b) + k * S(a) * S(b + c) - l * S(a) * C(b + c)) / (n + m);
    double tmpC = (Trans[2][3] - h - j * C(b) - k * C(b + c) - l * S(b + c)) / (n + m);

    double tmpE = ((tmpA * C(a) + tmpB * S(a)) * C(b + c) + tmpC * S(b + c)) * (n + m);
    //printf("tmpA=%lf\ntempB=%lf\ntempC=%lf\ntempE=%lf\n", tmpA,tmpB,tmpC,tmpE);
    //printf("(n + m)* (n + m)-tmpE*tmpE=%lf\n", (n + m) * (n + m) - tmpE * tmpE);
    //double e = rad2deg(atan2( sqrt((n + m)* (n + m)-tmpE*tmpE), tmpE));
    double e = rad2deg(atan(sqrt((n + m) * (n + m) - tmpE * tmpE) / tmpE));
    //printf("e=%lf\n", e);

    //どこかでabsが入っている
    //sqrtが入っているから怪しい


    //printf("x,y=%lf,%lf\n", x, y);
    //double e = rad2deg(atan2(y, x));
   /* if (x == 0 || y == 0)
    {
       // printf("y=0(e)\n");
        return 0;
    }
    if (e_min > e) { e = e + 180; }
    else if (a_max < e) { e = e - 180; }
    */
    if (e_min > e || e_max < e)
    {
        printf("atan2error in e : No answer\n");
        return EOF;
    }
    return e;

}
double cal_f(double a, double b, double c) {
    /*double x = (Trans[0][2] * C(a) + Trans[1][2] * S(a)) * C(b + c) + Trans[2][2] * S(b + c);
    double y = (Trans[0][1] * C(a) + Trans[1][1] * S(a)) * C(b + c) + Trans[2][1] * S(b + c);
    double f = rad2deg(atan2(y, x));
    if (x == 0 && y == 0)
    {
        printf("atan2 in f : x and y equal 0\n");
    }
    else if (f_min > f || f_max < f)
    {
        printf("atan2error in f : No answer\n");
        return EOF;
    }
    return f;
*/
    double x = (Trans[0][0] * C(a) + Trans[1][0] * S(a)) * C(b + c) + Trans[2][0] * S(b + c);
    double y = (Trans[0][1] * C(a) + Trans[1][1] * S(a)) * C(b + c) + Trans[2][1] * S(b + c);
    double f = rad2deg(atan2(y, x));

    //printf("x,y,f=%lf,%lf,%lf\n", x, y, f);
    if (x == 0 || y == 0)
    {
        // printf("y=0(f)\n");
    }
    if (f_min > f) { f = f + 180; }
    else if (f_max < f) { f = f - 180; }

    if (f_min > f || f_max < f)
    {
        printf("atan2error in f : No answer\n");
        return 0;
    }
    return f;



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
    double J2_1 = rad2deg( -1 * acos( -1 * (a_3 * C(J3) + d_4 * S(J3) - pz) / a_2))+360;
    double J2_2 = rad2deg(      acos( -1 * (a_3 * C(J3) + d_4 * S(J3) - pz) / a_2));
    
    //printf("J2_1,J2_2=%lf , %lf\n", J2_1, J2_2);
    //条件分岐
    if ((J2_1 <= 150.0 && J2_1 >= 10.0) && (J2_2 > 150.0 || J2_2 < 10.0))
    {
        return J2_1;
    }
    else if ((J2_2 <= 150.0 && J2_2 >= 10.0) && (J2_1 > 150.0 || J2_1 < 10.0))
    {
        return J2_2;
    }
    else if ((J2_1 <= 150.0 && J2_1 >= 10.0) && (J2_2 <= 150.0 && J2_2 >= 10.0))
    {
        return EOF;
    }
    else if ((J2_1 > 150.0 || J2_1 < 10.0) && (J2_2 > 150.0 || J2_2 < 10.0))
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
