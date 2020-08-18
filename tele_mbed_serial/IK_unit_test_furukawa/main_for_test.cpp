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


//=======================================順運動学======================================
// Masahiro Furukawa
// Aug 17, 2020
void cal_T_from_arg_mod(Arg arg)
{
    double a = deg2rad(arg.a);
    double b = deg2rad(arg.b);
    double c = deg2rad(arg.c);
    double d = deg2rad(arg.d);
    double e = deg2rad(arg.e);
    double f = deg2rad(arg.f); 

    // Generated from sympy
    _nx_ = ((Sa * sin(d) - sin(b + c) * cos(a) * cos(d)) * cos(e) - sin(e) * cos(a) * cos(b + c)) * sin(f) - (Sa * cos(d) + sin(d) * sin(b + c) * cos(a)) * cos(f);
    _ny_ = (-(Sa * sin(b + c) * cos(d) + sin(d) * cos(a)) * cos(e) - Sa * sin(e) * cos(b + c)) * sin(f) + (-Sa * sin(d) * sin(b + c) + cos(a) * cos(d)) * cos(f);
    _nz_ = (-sin(e) * sin(b + c) + cos(d) * cos(e) * cos(b + c)) * sin(f) + sin(d) * cos(f) * cos(b + c);
    Trans[3][0] = 0;

    _ox_ = ((Sa * sin(d) - sin(b + c) * cos(a) * cos(d)) * cos(e) - sin(e) * cos(a) * cos(b + c)) * cos(f) + (Sa * cos(d) + sin(d) * sin(b + c) * cos(a)) * sin(f);
    _oy_ = (-(Sa * sin(b + c) * cos(d) + sin(d) * cos(a)) * cos(e) - Sa * sin(e) * cos(b + c)) * cos(f) - (-Sa * sin(d) * sin(b + c) + cos(a) * cos(d)) * sin(f);
    _oz_ = (-sin(e) * sin(b + c) + cos(d) * cos(e) * cos(b + c)) * cos(f) - sin(d) * sin(f) * cos(b + c);
    Trans[3][1] = 0;

    _ax_ = (Sa * sin(d) - sin(b + c) * cos(a) * cos(d)) * sin(e) + cos(a) * cos(e) * cos(b + c);
    _ay_ = -(Sa * sin(b + c) * cos(d) + sin(d) * cos(a)) * sin(e) + Sa * cos(e) * cos(b + c);
    _az_ = sin(e) * cos(d) * cos(b + c) + sin(b + c) * cos(e);
    Trans[3][2] = 0;

    _px_ = g * cos(a) - j * sin(b) * cos(a) - k * sin(b + c) * cos(a) + l * cos(a) * cos(b + c) + m * Sa * sin(d) * sin(e) - m * sin(e) * sin(b + c) * cos(a) * cos(d) + m * cos(a) * cos(e) * cos(b + c) + n * Sa * sin(d) * sin(e) - n * sin(e) * sin(b + c) * cos(a) * cos(d) + n * cos(a) * cos(e) * cos(b + c);
    _py_ = g * Sa - j * Sa * sin(b) - k * Sa * sin(b + c) + l * Sa * cos(b + c) - m * Sa * sin(e) * sin(b + c) * cos(d) + m * Sa * cos(e) * cos(b + c) - m * sin(d) * sin(e) * cos(a) - n * Sa * sin(e) * sin(b + c) * cos(d) + n * Sa * cos(e) * cos(b + c) - n * sin(d) * sin(e) * cos(a);
    _pz_ = h + j * cos(b) + k * cos(b + c) + l * sin(b + c) + m * sin(e) * cos(d) * cos(b + c) + m * sin(b + c) * cos(e) + n * sin(e) * cos(d) * cos(b + c) + n * sin(b + c) * cos(e);
    Trans[3][3] = 1;

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

int main()
{
    Arg arg, ans;
    Pos pos;
    PointandPosture arm;

    arg.a = -37.0;//40.0
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
    cal_T_from_arg_mod(arg);
    pos.x = _px_;
    pos.y = _py_;
    pos.z = _pz_;

    printf("順運動学を計算した結果．原点から見た手先位置は次のようになりました．\n");
    printf("\npos[x,y,z]=[%.3lf,%.3lf,%.3lf]\n\nただし単位はOptiからの長さと合わせるために[m]です\n\n", pos.x, pos.y, pos.z);

    dumpT();

    printf("\n\n続いて逆運動学で各関節の角度の計算を行います．\nプログラムの詳細は関数cal_a～cal_fを確認してください\n");
    //アームの単位はmmで計算していることに注意　
    //アームには変位を入力している.

    //=======================================================================1000倍すると誤差も1000倍になることに注意====================================

    //単位はｍなので上手な変換

    //もしクオータニオンから姿勢行列を再計算する場合は以下の行を有効にする
    if (false) {
        //======================手先座標系→基準座標系========================
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
        double norm = sqrt(arm.qx * arm.qx + arm.qy * arm.qy + arm.qz * arm.qz + arm.qw * arm.qw);
        if (norm != 0.0) {
            arm.qx /= norm;
            arm.qy /= norm;
            arm.qz /= norm;
            arm.qw /= norm;
        }
        cal_T(arm.x, arm.y, arm.z, arm.qx, arm.qy, arm.qz, arm.qw);
    }
    
    dumpT();

    ans.a = cal_a();
    ans.b = cal_b(ans.a);
    ans.c = cal_c(ans.a);
    ans.d = cal_d(ans.a, ans.b, ans.c);
    ans.e = cal_e(ans.a, ans.b, ans.c, ans.d);
    ans.f = cal_f(ans.a, ans.b, ans.c);


    printf("逆運動学を計算した結果．関節角度は次のようになりました．\n");
    printf("ans[a,b,c,d,e,f] = [%.3lf,\t%.3lf,\t%.3lf,\t%.3lf,\t%.3lf,\t%.3lf]\n", ans.a, ans.b, ans.c, ans.d, ans.e, ans.f);
    printf("\n\n解を比較します．\n");

    printf("arg[a,b,c,d,e,f] = [%.3lf,\t%.3lf,\t%.3lf,\t%.3lf,\t%.3lf,\t%.3lf]\n", arg.a, arg.b, arg.c, arg.d, arg.e, arg.f);
    printf("ans[a,b,c,d,e,f] = [%.3lf,\t%.3lf,\t%.3lf,\t%.3lf,\t%.3lf,\t%.3lf]\n", ans.a, ans.b, ans.c, ans.d, ans.e, ans.f);



    return 0;
}