// Created by Tomoki Hirayama
// Modified by Masahiro Furukawa, Aug 15, 2020

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>

#define PI  3.14159265358979
//アームのパラメータ
//単位はすべてメートル(m)
#define g  0.030
#define h  0.102
#define j  0.120
#define k  0.020
#define l  0.129
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


struct Arg {
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;
};
struct Pos {
    double x;
    double y;
    double z;
};



double rad2deg(double rad) {
    return rad * 180.0 / PI;
}
//度数からラジアンに
double deg2rad(double deg) {
    return deg * PI / 180.0 ;
}
//Cos,Sinの略記
double C(double a) {
    return cos(a);
}
double S(double a) {
    return sin(a);
}

//=======================================順運動学======================================

double cal_x(Arg arg)
{
    //ラジアン表記に変換すること
    double a = deg2rad(arg.a);
    double b = deg2rad(arg.b);
    double c = deg2rad(arg.c);
    double d = deg2rad(arg.d);
    double e = deg2rad(arg.e);
    double f = deg2rad(arg.f);

    double x = (n + m) * (C(a) * (C(b + c) * C(e) - S(b + c) * C(d) * S(e)) + S(a) * S(d) * S(e)) + l * C(a) * C(b + c) - k * C(a) * S(b + c) - j * C(a) * S(b) + g * C(a);
    return x;
}
double cal_y(Arg arg)
{
    //ラジアン表記に変換すること
    double a = deg2rad(arg.a);
    double b = deg2rad(arg.b);
    double c = deg2rad(arg.c);
    double d = deg2rad(arg.d);
    double e = deg2rad(arg.e);
    double f = deg2rad(arg.f);

    double y = (n + m) * (S(a) * (C(b + c) * C(e) - S(b + c) * C(d) * S(e)) - C(a) * S(d) * S(e)) + l * S(a) * C(b + c) - k * S(a) * S(b + c) - j * S(a) * S(b) + g * S(a);
    return y;
}
double cal_z(Arg arg)
{
    //ラジアン表記に変換すること
    double a = deg2rad(arg.a);
    double b = deg2rad(arg.b);
    double c = deg2rad(arg.c);
    double d = deg2rad(arg.d);
    double e = deg2rad(arg.e);
    double f = deg2rad(arg.f);

    double z = (n + m) * (S(b + c) * C(e) * C(b + c) + C(d) * S(e)) + l * S(b + c) + k * C(b + c) + j * C(b) + h;
    return z;
}



//=================================================逆運動学=======================================================

//座標変換行列
double Trans[4][4];
//クォータニオン→座標変換行列の計算
void cal_T(double x, double y, double z, double qx, double qy, double qz, double qw) {

    //printf("qx=%f\nqy=%f\nqz=%f\nqw=%f\n\n", qx, qy, qz, qw);
    //確認済み↑

    Trans[0][0] = qx * qx - qy * qy - qz * qz + qw * qw;//nx
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
    Trans[3][3] = 1.0;//1
}

void cal_T_from_arg(Arg arg,Pos pos)
{
    double a = deg2rad(arg.a);
    double b = deg2rad(arg.b);
    double c = deg2rad(arg.c);
    double d = deg2rad(arg.d);
    double e = deg2rad(arg.e);
    double f = deg2rad(arg.f);

    Trans[0][0] = -C(a) * S(b + c) * (C(d) * C(e) * S(f) + S(d) * C(f) + C(b + c) * S(e) * S(f))   +   S(a) * (S(d) * C(e) * S(f) - C(d) * C(f));//nx
    Trans[1][0] = -S(a) * S(b + c) * (C(d) * C(e) * S(f) + S(d) * C(f) + C(b + c) * S(e) * S(f))   -   C(a) * (S(d) * C(e) * S(f) - C(d) * C(f));//ny
    Trans[2][0] =         C(b + c) * (C(d) * C(e) * S(f) + S(d) * C(f) - S(b + c) * S(e) * S(f));//nz
    Trans[3][0] = 0.0;//0

    Trans[0][1] = -C(a) * S(b + c) * (C(d) * C(e) * C(f) - S(d) * S(f) + C(b + c) * S(e) * C(f))   +   S(a) * (S(d) * C(e) * C(f) + C(d) * S(f));//ox
    Trans[1][1] = -S(a) * S(b + c) * (C(d) * C(e) * C(f) - S(d) * S(f) + C(b + c) * S(e) * C(f))   -   C(a) * (S(d) * C(e) * C(f) + C(d) * S(f));//oy
    Trans[2][1] =         C(b + c) * (C(d) * C(e) * C(f) - S(d) * S(f) - S(b + c) * S(e) * C(f));//oz
    Trans[3][1] = 0.0;//0

    Trans[0][2] = -C(a) * (S(b + c) * C(d) * S(e) - C(b + c) * C(e))   +   S(a) * S(d) * S(e);//ax
    Trans[1][2] = -S(a) * (S(b + c) * C(d) * S(e) - C(b + c) * C(e))   -   C(a) * S(d) * S(e);;//ay
    Trans[2][2] =          C(b + c) * C(d) * S(e) + S(b + c) * C(e);//az
    Trans[3][2] = 0.0;//0

    Trans[0][3] = pos.x;//px
    Trans[1][3] = pos.y;//py
    Trans[2][3] = pos.z;//pz
    Trans[3][3] = 1.0;//1

    printf("座標変換行列です\n");
    for (int cnti = 0; cnti < 4; cnti++)
    {
        for (int cntj = 0; cntj < 4; cntj++)
        {
            printf("%.3lf\t", Trans[cnti][cntj]);
        }
        printf("\n");
    }
    printf("\n");
}
double cal_a() {
    double x = -(m + n) * Trans[0][2] + Trans[0][3];
    double y = -(m + n) * Trans[1][2] + Trans[1][3];
    double a = rad2deg(atan2(y, x));
    printf("x,y,a=%lf,%lf,%lf\n", x,y,a);
    if (x==-0||y==0)
    {
        printf("y=0(a)\n");
        return 0;
    }
    else {}
    if (a_min > a || a_max < a)
    { 
        printf("x,y,a=%lf,  %lf, %lf\n",x,y,a);
        printf("atan2error in a : No answer\n");
        return EOF;
    }
    return a;
}
double cal_b(Arg arg) {
    double a = deg2rad(arg.a);

    double A = Trans[2][3] - (m + n) * Trans[2][2] - h;
    double B = Trans[0][3] * C(a) + Trans[1][3] * S(a) - (m + n) * (Trans[0][2] * C(a) + Trans[1][2] * S(a)) - g;
    double D = (A * A + B * B + j * j - k * k - l * l) / (2 * j);
    /*double b1 = rad2deg(2 * atan2(-B + sqrt(abs(B * B + A * A - D * D)), A + D));
    //double b2 = rad2deg(2 * atan2(-B - sqrt(abs(B * B + A * A - D * D)), A + D));

    printf("b1,b2=%lf,%lf", b1, b2);
    //エラーが起きる理由：sqrtの中身が負になっている，なる場合がある

    printf("sqrt(D=b^2-4ac)=%lf\n" ,- B + sqrt(B * B + A * A - D * D));
    
    if (b1 > 180.0) { b1 = b1 - 180.0; }
    else if (b1 < -180.0) { b1 = b1 + 360; }
    if (b2 > 180.0) { b2 = b2 - 180.0; }
    else if (b2 < -180.0) { b2 = b2 + 360; }

    //printf("A=%lf\nB=%lf\nD=%lf\nb1=%lf\nb2=%lf\n\n", A, B, D, b1, b2);

    //解が二つでているので範囲外ならはじく
    if ((b_min <= b1 && b_max >= b1) && (b_min > b2 || b_max < b2)) {
        return b1;
    }
    else if ((b_min <= b2 && b_max >= b2) && (b_min > b1 || b_max < b1)) {
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

    double tmp1,tmp2;
    if (A * A + B * B - D * D == 0 || D==0)
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
double cal_c(Arg arg) {
    double a = deg2rad(arg.a);
    
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
        tmp = atan2(E,sqrt(l * l + k * k - E * E));
    }
    double c = rad2deg(tmp - atan2(k, l));

    if (c_min > c || c_max < c)
    {
        printf("atan2error in c : No answer\n");
        return EOF;
    }

    return c;



    //解の調整
    /*if (c1 > 180.0) { c1 = c1 - 180.0; }
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
        

}
double cal_d(Arg arg) {
    
    double a = deg2rad(arg.a);
    double b = deg2rad(arg.b);
    double c = deg2rad(arg.c);

    double x = Trans[2][2] * C(b + c) - (Trans[0][2] * C(a) + Trans[1][2] * S(a)) * S(b + c) ;
    double y = Trans[0][2] * S(a) - Trans[1][2] * C(a);
    double d = rad2deg(atan2(y, x));

    //printf("角度dについて\n");
    //printf("x,y,d=%lf,%lf,%lf\n", x, y,d);
    if (x == 0.0 || y == 0.0)
    {
        return 0.0;
    }

    if (d_min > d) { d = d + 180; }
    else if (a_max < d) { d = d - 180; }

    if(d_min > d || d_max < d)
    {
        printf("atan2error in d : No answer\n");
        return EOF;
    }

    return d;
}
double cal_e(Arg arg) {
    double a = deg2rad(arg.a);
    double b = deg2rad(arg.b);
    double c = deg2rad(arg.c);
    double d = deg2rad(arg.d);

    double x = (Trans[0][0] * S(a) - Trans[1][0] * C(a)) * S(d) - ((Trans[0][0] * C(a) + Trans[1][0] * S(a)) * S(b + c) - Trans[2][0] * C(b + c)) * C(d);
    double y = (Trans[0][0] * C(a) + Trans[1][0] * S(a)) * C(b + c) + Trans[2][0] * S(b + c);

    //printf("x,y=%lf,%lf\n", x, y);
    double e = rad2deg(atan2(y, x));
    if (x == 0 || y == 0)
    {
        printf("y=0(e)\n");
        return 0;
    }
    if (e_min > e) { e = e + 180; }
    else if (a_max < e) { e = e - 180; }

    if (e_min > e || e_max < e)
    {
        printf("atan2error in e : No answer\n");
        return EOF;
    }
    return e;
}
double cal_f(Arg arg) {
    double a = deg2rad(arg.a);
    double b = deg2rad(arg.b);
    double c = deg2rad(arg.c);

    double x = (Trans[0][0] * C(a) + Trans[1][0] * S(a)) * C(b + c) + Trans[2][0] * S(b + c);
    double y = (Trans[0][1] * C(a) + Trans[1][1] * S(a)) * C(b + c) + Trans[2][1] * S(b + c);
    double f = rad2deg(atan2(y, x));

    printf("x,y,f=%lf,%lf,%lf\n", x, y, f);
    if (x == 0 || y == 0)
    {
        printf("y=0(f)\n");
    }
    if (f_min > f) { f = f + 180; }
    else if (f_max < f) { f = f - 180; }

    if (f_min > f || f_max < f)
    {
        printf("atan2error in f : No answer\n");
        return EOF;
    }
    return f;
}

int main()
{
    Arg arg,ans;
    Pos pos;

    arg.a = 40.0;
    arg.b = 0.0;
    arg.c = 0.0;
    arg.d = 30.0;
    arg.e = 30.0;
    arg.f = 0.0;


    printf("逆運動学のチェックを行います.\n計算は次の角度情報を入力します\n");
    printf("arg[a,b,c,d,e,f]=[%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf]\n",arg.a,arg.b,arg.c,arg.d,arg.e,arg.f);
    
    printf("\n\n\n\nまず，順運動学で手先位置の計算を行います．\nプログラムの詳細は関数cal_x,cal_y,cal_zを確認してください\n");
    pos.x = cal_x(arg);
    pos.y = cal_y(arg);
    pos.z = cal_z(arg);

    printf("純運動学を計算した結果．原点から見た手先位置は次のようになりました．\n");
    printf("pos[x,y,z]=[%.3lf,%.3lf,%.3lf]\tただし単位はOptiからの長さと合わせるために[m]です\n", pos.x, pos.y, pos.z);

    printf("\n\n\n\n続いて逆運動学で各関節の角度の計算を行います．\nプログラムの詳細は関数cal_a～cal_fを確認してください\n");
    
    cal_T_from_arg(arg, pos);
    ans.a = cal_a();
    ans.b = cal_b(arg);
    ans.c = cal_c(arg);
    ans.d = cal_d(arg);
    ans.e = cal_e(arg);
    ans.f = cal_f(arg);

    printf("逆運動学を計算した結果．関節角度は次のようになりました．\n");
    printf("arg[a,b,c,d,e,f]=[%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf]\n", ans.a, ans.b, ans.c, ans.d, ans.e, ans.f);
    printf("\n\n解を比較します．\n");

    printf("arg[a,b,c,d,e,f]=[%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf]\n", arg.a, arg.b, arg.c, arg.d, arg.e, arg.f);
    printf("arg[a,b,c,d,e,f]=[%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf]\n", ans.a, ans.b, ans.c, ans.d, ans.e, ans.f);



    return 0;
}