#pragma once

//�֐��v���g�^�C�v�ݒ�


//=================================================�t�^���w=======================================================
#define PI  3.14159265358979
//�A�[���̃p�����[�^
//�P�ʂ͂��ׂă��[�g��(m)
#define g  0.030
#define h  0.102
#define j  0.120
#define k  0.020
#define l  0.129
#define m  0.016
#define n  0.026


//���ꂼ��̊p�͈̔�


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
//�x�����烉�W�A����
double deg2rad(double);
//Cos,Sin�̗��L
double C(double);
double S(double);	
//�N�H�[�^�j�I�������W�ϊ��s��̌v�Z
void cal_T(double, double, double, double, double, double, double);
double cal_a();
double cal_b(double);
double cal_c(double);
double cal_d(double, double, double);
double cal_e(double, double, double, double);
double cal_f(double, double, double);
