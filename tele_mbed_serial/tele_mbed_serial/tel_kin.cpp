#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>

#include"tel_kin.h"

//座標変換行列
double Trans[4][4];

double rad2deg(double rad) {
	return rad / PI * 180.0;
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

	_nx_ = qx * qx - qy * qy - qz * qz + qw * qw;        // nx 0
	_ny_ = 2.0 * qx * qy + 2.0 * qz * qw;                // ny 0
	_nz_ = 2.0 * qx * qz - 2.0 * qy * qw;                // nz 1 
	Trans[3][0] = 0.0;                                          // 0

	_ox_ = 2.0 * qx * qy - 2.0 * qz * qw;                // ox //reference : https://qiita.com/drken/items/0639cf34cce14e8d58a5
	_oy_ = -qx * qx + qy * qy - qz * qz + qw * qw;       // oy 
	_oz_ = 2.0 * qy * qz + 2.0 * qx * qw;                // oz
	Trans[3][1] = 0.0;                                          // 0

	_ax_ = 2.0 * qx * qz + 2.0 * qy * qw;                // ax
	_ay_ = 2.0 * qy * qz - 2.0 * qx * qw;                // ay
	_az_ = -qx * qx - qy * qy + qz * qz + qw * qw;       // az
	Trans[3][2] = 0.0;                                          // 0

	_px_ = x;                                            // px
	_py_ = y;                                            // py
	_pz_ = z;                                            // pz
	Trans[3][3] = 1.0;                                          // 1
}

double cal_a() {
	double x = _px_ - (m + n) * _ax_;
	double y = _py_ - (m + n) * _ay_;
	double a = rad2deg(atan2(y, x));

	if (x == 0 && y == 0)
	{
		printf("atan2error in a : x and y equal to 0\n");
		return a;
	}
	else if (a_min > a || a_max < a)
	{
		printf("atan2error in a : No answer\n");
		return EOF;
	}
	printf("[a] angle \"a\" found : %3.2f [deg] in y = %3.2f, x = %3.2f\n", a, y, x);
	return a;
}

double cal_b(double a) {
	//double A = _pz_ - (m + n) * _nz_ - h;
	double A = _pz_ - (m + n) * _az_ - h;
	//double B = _px_ * Ca+ _py_ * Sa- (m + n) * (_nx_* Ca+ _ny_ * Sa) - g;
	double B = _px_ * Ca + _py_ * Sa - (m + n) * (_ax_ * Ca + _ay_ * Sa) - g;
	double D = (A * A + B * B + j * j - k * k - l * l) / (2 * j);
	double b1 = rad2deg(2 * atan2(-B + sqrt(abs(B * B + A * A - D * D)), A + D));
	double b2 = rad2deg(2 * atan2(-B - sqrt(abs(B * B + A * A - D * D)), A + D));

	//エラーが起きる理由：sqrtの中身が負になっている，なる場合がある

	//printf("sqrt(D=b^2-4ac)=%lf\n" ,- B + sqrt(B * B + A * A - D * D));

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
	}
}
double cal_c(double a) {
	//double A = _pz_ - (m + n) * _nz_ - h;
	double A = _pz_ - (m + n) * _az_ - h;
	//double B = _px_ * Ca+ _py_ * Sa- (m + n) * (_nx_* Ca+ _ny_ * Sa) - g;
	double B = _px_ * Ca + _py_ * Sa - (m + n) * (_ax_ * Ca + _ay_ * Sa) - g;
	double E = (A * A + B * B - j * j - k * k - l * l) / (2 * j);
	double c1 = rad2deg(2 * atan2(-l + sqrt(abs(l * l + k * k - E * E)), E + k));
	double c2 = rad2deg(2 * atan2(-l - sqrt(abs(l * l + k * k - E * E)), E + k));



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
	}
}
double cal_d(double a, double b, double c) {
	//double x = -1 * (_nx_* Ca+ _ny_ * Sa) * Sbc + Trans[3][0] * Cbc;
	//double y = _nx_* Sa- _ny_ * Sa;
	double x = -1 * (_ax_ * Ca + _ay_ * Sa) * Sbc + _az_ * Cbc;
	double y = _ax_ * Sa - _ay_ * Ca;

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
	return d;
}
double cal_e(double a, double b, double c, double d) {
	//double x = (_nx_* Sa+ _ny_ * Ca) * Sd - ((_nx_* Ca+ _ny_ * Sa) * Sbc - Trans[3][0] * Cbc) * Cd;
	//double y = (_nx_* Ca+ _ny_ * Sa) * Cbc + Trans[3][0] * Sbc;
	double x = (_nx_ * Sa - _ny_ * Ca) * Sd - ((_nx_ * Ca + _ny_ * Sa) * Sbc - _nz_ * Cbc) * Cd;
	double y = (_nx_ * Ca + _ny_ * Sa) * Cbc + _nz_ * Sbc;

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
}
double cal_f(double a, double b, double c) {
	//double x = (_ax_ * Ca+ _ay_ * Sa) * Cbc + _az_ * Sbc;
	double x = (_nx_ * Ca + _ny_ * Sa) * Cbc + _nz_ * Sbc;
	double y = (_ox_ * Ca + _oy_ * Sa) * Cbc + _oz_ * Sbc;
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
}
