#pragma once

//関数プロトタイプ設定


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

double cal_J1(double,double,double);
double cal_J2(double,double,double,double, double);
double cal_J3(double,double,double,double);