// Created by Tomoki Hirayama
// Modified by Masahiro Furukawa, Aug 15, 2020

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>
#include"tel_kin.h"

//
//extern double PI;
////アームのパラメータ
////単位はすべてメートル(m)
//extern double g;
//extern double h;
//extern double j;
//extern double k;
//extern double l;
//extern double m;
//extern double n;

extern double Trans[4][4];


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


struct PointandPosture {
    double x;
    double y;
    double z;
    double qx;
    double qy;
    double qz;
    double qw;
};



void cal_T_from_arg(Arg arg, Pos pos)
{
    double a = deg2rad(arg.a);
    double b = deg2rad(arg.b);
    double c = deg2rad(arg.c);
    double d = deg2rad(arg.d);
    double e = deg2rad(arg.e);
    double f = deg2rad(arg.f);

    Trans[0][0] = -C(a) * S(b + c) * (C(d) * C(e) * S(f) + S(d) * C(f) + C(b + c) * S(e) * S(f)) + S(a) * (S(d) * C(e) * S(f) - C(d) * C(f));//nx
    Trans[1][0] = -S(a) * S(b + c) * (C(d) * C(e) * S(f) + S(d) * C(f) + C(b + c) * S(e) * S(f)) - C(a) * (S(d) * C(e) * S(f) - C(d) * C(f));//ny
    Trans[2][0] = C(b + c) * (C(d) * C(e) * S(f) + S(d) * C(f) - S(b + c) * S(e) * S(f));//nz
    Trans[3][0] = 0.0;//0

    Trans[0][1] = -C(a) * S(b + c) * (C(d) * C(e) * C(f) - S(d) * S(f) + C(b + c) * S(e) * C(f)) + S(a) * (S(d) * C(e) * C(f) + C(d) * S(f));//ox
    Trans[1][1] = -S(a) * S(b + c) * (C(d) * C(e) * C(f) - S(d) * S(f) + C(b + c) * S(e) * C(f)) - C(a) * (S(d) * C(e) * C(f) + C(d) * S(f));//oy
    Trans[2][1] = C(b + c) * (C(d) * C(e) * C(f) - S(d) * S(f) - S(b + c) * S(e) * C(f));//oz
    Trans[3][1] = 0.0;//0

    Trans[0][2] = -C(a) * (S(b + c) * C(d) * S(e) - C(b + c) * C(e)) + S(a) * S(d) * S(e);//ax
    Trans[1][2] = -S(a) * (S(b + c) * C(d) * S(e) - C(b + c) * C(e)) - C(a) * S(d) * S(e);;//ay
    Trans[2][2] = C(b + c) * C(d) * S(e) + S(b + c) * C(e);//az
    Trans[3][2] = 0.0;//0

    Trans[0][3] = pos.x;//px
    Trans[1][3] = pos.y;//py
    Trans[2][3] = pos.z;//pz
    Trans[3][3] = 1.0;//1

}
void dumpT(void) {
    printf("\n座標変換行列です\n");
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



int main()
{
    Arg arg, ans;
    Pos pos;
    PointandPosture arm;

    arg.a = 0.0;//40.0
    arg.b = 0.0;
    arg.c = 0.0;
    arg.d = 0.0;//30.0;
    arg.e = 0.0;//30.0;
    arg.f = 0.0;

    ans.a = 999.0;
    ans.b = 999.0;
    ans.c = 999.0;
    ans.d = 999.0;
    ans.e = 999.0;
    ans.f = 999.0;


    printf("逆運動学のチェックを行います.\n計算は次の角度情報を入力します\n");
    printf("arg[a,b,c,d,e,f] = [%.3lf,\t%.3lf,\t%.3lf,\t%.3lf,\t%.3lf,\t%.3lf]\n", arg.a, arg.b, arg.c, arg.d, arg.e, arg.f);

    printf("\n\nまず，順運動学で手先位置の計算を行います．\nプログラムの詳細は関数cal_x,cal_y,cal_zを確認してください\n");
    pos.x = cal_x(arg);
    pos.y = cal_y(arg);
    pos.z = cal_z(arg);

    printf("純運動学を計算した結果．原点から見た手先位置は次のようになりました．\n");
    printf("\npos[x,y,z]=[%.3lf,%.3lf,%.3lf]\n\nただし単位はOptiからの長さと合わせるために[m]です\n\n", pos.x, pos.y, pos.z);

    cal_T_from_arg(arg, pos); 
    dumpT();

    printf("\n\n続いて逆運動学で各関節の角度の計算を行います．\nプログラムの詳細は関数cal_a～cal_fを確認してください\n");
    //アームの単位はmmで計算していることに注意　
    //アームには変位を入力している.

    //=======================================================================1000倍すると誤差も1000倍になることに注意====================================

    //単位はｍなので上手な変換

    arm.x = pos.x;//* 0.005;// -1.0 * double(data->RigidBodies[i].z - HumanFirstPlace.z) * scale;
    arm.y = pos.y;// *0.005;//        double(data->RigidBodies[i].y - HumanFirstPlace.y)* scale;
    arm.z = pos.z;// *0.005;//        double(data->RigidBodies[i].x - HumanFirstPlace.x)* scale;
    // 
    // https://qiita.com/drken/items/0639cf34cce14e8d58a5
    double th = 90;
    struct Vec { double x; double y; double z; };
    Vec vec;
    vec.x = 1.0;
    vec.y = 1.0;
    vec.z = 1.0;
    arm.qx = vec.x * S(deg2rad(th / 2.0));// -1.0 * double(data->RigidBodies[i].qz);
    arm.qy = vec.y * S(deg2rad(th / 2.0));//       double(data->RigidBodies[i].qy);
    arm.qz = vec.z * S(deg2rad(th / 2.0));//      double(data->RigidBodies[i].qx);
    arm.qw = C(deg2rad(th / 2.0));//      double(data->RigidBodies[i].qw);
    //double norm = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    double norm = sqrt(arm.qx * arm.qx + arm.qy * arm.qy + arm.qz * arm.qz + arm.qw * arm.qw);
    if (norm != 0.0) {
        arm.qx /= norm;
        arm.qy /= norm;
        arm.qz /= norm;
        arm.qw /= norm;
    }
    //======================手先座標系→基準座標系========================
    cal_T(arm.x, arm.y, arm.z, arm.qx, arm.qy, arm.qz, arm.qw);
    dumpT();

    double a = cal_a();
    double b = cal_b(a);
    double c = cal_c(a);
    double d = cal_d(a, b, c);
    double e = cal_e(a, b, c, d);
    double f = cal_f(a, b, c);


    printf("逆運動学を計算した結果．関節角度は次のようになりました．\n");
    printf("ans[a,b,c,d,e,f] = [%.3lf,\t%.3lf,\t%.3lf,\t%.3lf,\t%.3lf,\t%.3lf]\n", ans.a, ans.b, ans.c, ans.d, ans.e, ans.f);
    printf("\n\n解を比較します．\n");

    printf("arg[a,b,c,d,e,f] = [%.3lf,\t%.3lf,\t%.3lf,\t%.3lf,\t%.3lf,\t%.3lf]\n", arg.a, arg.b, arg.c, arg.d, arg.e, arg.f);
    printf("ans[a,b,c,d,e,f] = [%.3lf,\t%.3lf,\t%.3lf,\t%.3lf,\t%.3lf,\t%.3lf]\n", ans.a, ans.b, ans.c, ans.d, ans.e, ans.f);



    return 0;
}