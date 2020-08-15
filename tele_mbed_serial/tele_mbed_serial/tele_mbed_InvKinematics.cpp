#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>

#define PI 3.14159265358979
//アームのパラメータ
//単位はすべてミリ(mm)
#define g 30.0
#define h 102.0
#define j 120.0
#define k 20.0
#define l 129.0
#define m 16.0
#define n 26.0

//スケール比の定義 0 < N <= 1
#define N 1.0

//それぞれの角の範囲
#define a_min -120.0
#define a_max 90.0
#define b_min -73.0
#define b_max 90.0
#define c_min -3.0
#define c_max 130.0
#define d_min -90.0
#define d_max 115.0
#define e_min -100.0
#define e_max 85.0
#define f_min -100.0
#define f_max 90.0

float T[4][4];
//  T[0][0]=nx  T[0][1]=ox  T[0][2]=ax  T[0][3]=px
//  T[1][0]=ny  T[1][1]=oy  T[1][2]=ay  T[1][3]=py
//  T[2][0]=nz  T[2][1]=oz  T[2][2]=az  T[2][3]=pz
//  T[3][0]=0   T[3][1]=0   T[3][2]=0   T[3][3]=1


//ラジアンから度数に
float rad2deg(double rad) {
	return rad * 180.0 / PI;
}
//度数からラジアンに
float deg2rad(double deg) {
	return deg * PI / 180.0;
}
//Cos,Sinの略記
float C(float a) {
	return cos(a);
}
float S(float a) {
	return sin(a);
}

//クォータニオン→座標変換行列の計算
void cal_T(float x, float y, float z, float qx,float qy,float qz,float qw) {

	printf("qx=%lf\nqy=%lf\nqz=%lf\nqw=%lf\n\n",qx,qy,qz,qw);

	T[0][0] = qx*qx-qy*qy-qz*qz+qw*qw;//nx
	T[1][0] = 2.0*qx*qy + 2.0*qz*qw;//ny
	T[2][0] = 2.0*qx*qz - 2.0*qy*qw;//nz
	T[3][0] = 0.0;//0

	T[0][1] = 2.0*qx*qy + 2.0*qz*qw;//ox
	T[1][1] = -qx*qx + qy*qy - qz*qz + qw*qw;//oy
	T[2][1] = 2.0*qy*qz + 2.0*qx*qw;//oz
	T[3][1] = 0.0;//0

	T[0][2] = 2.0*qx*qz + 2.0*qy*qw;//ax
	T[1][2] = 2.0*qy*qz - 2.0*qx*qw;//ay
	T[2][2] = -qx*qx - qy*qy + qz*qz+qw*qw;//az
	T[3][2] = 0.0;//0

	T[0][3] = x;//px
	T[1][3] = y;//py
	T[2][3] = z;//pz
	T[3][3] = 1.0;//1
}
float cal_a() {
	float x = (m + n) * T[0][0] - T[0][3];
	float y = (m + n) * T[1][0] - T[1][3];
	if (x == 0 && y == 0)
	{
		printf("atan2error in a : x and y equal 0\n");
		return a;
	}
	else
	{
		a = rad2deg(atan2(y, x));
		if (a_min > a || a_max < a) {
			printf("atan2error in a : No answer\n");
			return EOF;
		}
		return a;
	}
}

float cal_b(float a) {
	float A = T[2][3] - (m + n) * T[2][0] - h;
	float B = T[0][3]*C(a)+T[1][3]*S(a)-(m+n)*(T[0][0]*C(a)+T[1][0]*S(a))-g;
	float D = (A*A + B*B +j*j - k*k - l*l) / (2 * j);
	float b1 = rad2deg(2*atan2(-B + sqrt(B*B + A*A - D*D),A+D));
	float b2 = rad2deg(2*atan2(-B - sqrt(B*B + A*A - D*D),A+D));

	//printf("A=%lf\nB=%lf\nD=%lf\nb1=%lf\nb2=%lf\n\n",A,B,D,b1,b2);

	//解が二つでているので範囲外ならはじく
	if((b_min<b1 && b_max>b1)&&(b_min>b2 || b_max<b2)){
		return b1;
	}
	else if((b_min<b2 && b_max>b2)&&(b_min>b1 || b_max<b1)){
		return b2;
	}
	else if ((b_min<b1 && b_max>b1)&& (b_min<b2 && b_max>b2))
	{//両方とも解になりうる場合,とりあえずエラー
		printf("answer error in b : two answer\n");
		return EOF;
	}
	else if((b_min>b1 || b_max<b1)&&(b_min>b2 || b_max<b2))
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

float cal_c(float a) {
	float A = T[2][3] - (m + n) * T[2][0] - h;
	float B = T[0][3] * C(a) + T[1][3] * S(a) - (m + n) * (T[0][0] * C(a) + T[1][0] * S(a)) - g;
	float E = (A*A + B*B - j*j - k*k - l*l) / (2*j);
	float c1 = rad2deg(2 * atan2(-l + sqrt(l*l + k*k - E*E), E+k));
	float c2 = rad2deg(2 * atan2(-l - sqrt(l*l + k*k - E*E), E+k));
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
	else if ((c_min>c1 || c_max<c1) && (c_min>c2 || c_max<c2))
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
float cal_d(float a,float b,float c) {
	float x = -1 * (T[0][0] * C(a) + T[1][0] * S(a)) * S(b + c) + T[3][0] * C(b + c);
	float y = T[0][0] * S(a) - T[1][0] * S(a);
	if (x == 0 && y == 0)
	{
		printf("atan2 in d : x and y equal 0\n");
	}
	return rad2deg(atan2(y, x));
}
float cal_e(float a,float b,float c,float d) {
	float x = (T[0][0] * S(a) + T[1][0] * C(a)) * S(d) - ((T[0][0] * C(a) + T[1][0] * S(a))*S(b + c) - T[3][0] * C(b + c)) * C(d);
	float y = (T[0][0] * C(a) + T[1][0] * S(a)) * C(b + c) + T[3][0] * S(b + c);
	if (x == 0 && y == 0)
	{
		printf("atan2 in e : x and y equal 0\n");
	}
	return rad2deg(atan2(y, x));
}
float cal_f(float a, float b, float c) {
	float x = (T[0][2] * C(a) + T[1][2] * S(a)) * C(b + c) + T[2][2] * S(b + c);
	float y = (T[0][1] * C(a) + T[1][1] * S(a)) * C(b + c) + T[2][1] * S(b + c);
	if (x == 0 && y == 0)
	{
		printf("atan2 in f : x and y equal 0\n");
	}
	return rad2deg(atan2(y, x));
}

//xyz->頭部の位置，exeyez->頭部姿勢の姿勢方向(単位ベクトル)，fi->回転角
double x, y, z, qx, qy, qz, qw;
double a, b, c, d, e, f;

int main(void){
//OptiTrackから情報を読み取る
	
	
//座標変換行列をつくる
	cal_T(N*x, N*y, N*z, qx, qy, qz, qw);
	for (int counti = 0; counti < 4; counti++)
	{
		for (int countj = 0; countj < 4; countj++)
		{
			printf("%.3lf ", T[counti][countj]);
		}
		printf("\n");
	}
	printf("\n");
//逆運動学の計算
	a = cal_a();
	b = cal_b(a);
	c = cal_c(a);
	d = cal_d(a, b, c);
	e = cal_e(a, b, c, d);
	f = cal_f(a, b, c);

	//確認用
	printf("a=%lf\nb=%lf\nc=%lf\nd=%lf\ne=%lf\nf=%lf\n",a, b, c, d, e, f);
}