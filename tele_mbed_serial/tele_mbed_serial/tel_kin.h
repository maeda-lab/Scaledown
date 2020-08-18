#pragma once

//関数プロトタイプ設定


//=================================================逆運動学=======================================================
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



//#define a_min  -180.0
//#define a_max  180.0
//#define b_min  -180.0
//#define b_max  180.0
//#define c_min  -180.0
//#define c_max  180.0

double rad2deg(double);
//度数からラジアンに
double deg2rad(double);
//Cos,Sinの略記
double C(double);
double S(double);	
//クォータニオン→座標変換行列の計算
void cal_T(double, double, double, double, double, double, double);
double cal_a();
double cal_b(double);
double cal_c(double);
double cal_d(double, double, double);
double cal_e(double, double, double, double);
double cal_f(double, double, double);

// Macro
#define _nx_ Trans[0][0] 
#define _ny_ Trans[1][0]
#define _nz_ Trans[2][0]

#define _ox_ Trans[0][1]
#define _oy_ Trans[1][1]
#define _oz_ Trans[2][1]

#define _ax_ Trans[0][2]
#define _ay_ Trans[1][2]
#define _az_ Trans[2][2]

#define _px_ Trans[0][3]
#define _py_ Trans[1][3]
#define _pz_ Trans[2][3]

#define Ca C(a)
#define Cb C(b)
#define Cc C(c)
#define Cd C(d)
#define Ce C(e)
#define Cf C(f)
#define Cbc C(b+c)
#define Sbc S(b+c)

#define Sa S(a)
#define Sb S(b)
#define Sc S(c)
#define Sd S(d)
#define Se S(e)
#define Sf S(f)