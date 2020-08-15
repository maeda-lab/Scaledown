#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>

#include"tel_kin.h"






//���W�ϊ��s��
double Trans[4][4];


double rad2deg(double rad) {
    return rad / PI * 180.0 ;
}
//�x�����烉�W�A����
double deg2rad(double deg) {
    return deg / 180.0 * PI;
}
//Cos,Sin�̗��L
double C(double a) {
    return cos(a);
}
double S(double a) {
    return sin(a);
}

//�N�H�[�^�j�I�������W�ϊ��s��̌v�Z
void cal_T(double x, double y, double z, double qx, double qy, double qz, double qw) {

    //printf("qx=%f\nqy=%f\nqz=%f\nqw=%f\n\n", qx, qy, qz, qw);
    //�m�F�ς݁�

    Trans[0][0] = qx * qx - qy * qy - qz * qz + qw * qw;//nx 0
    Trans[1][0] = 2.0 * qx * qy + 2.0 * qz * qw;//ny 0
    Trans[2][0] = 2.0 * qx * qz - 2.0 * qy * qw;//nz 1 ?
    Trans[3][0] = 0.0;//0

    Trans[0][1] = 2.0 * qx * qy - 2.0 * qz * qw;//ox //reference : https://qiita.com/drken/items/0639cf34cce14e8d58a5
    Trans[1][1] = -qx * qx + qy * qy - qz * qz + qw * qw;//oy?
    Trans[2][1] = 2.0 * qy * qz + 2.0 * qx * qw;//oz
    Trans[3][1] = 0.0;//0

    Trans[0][2] = 2.0 * qx * qz + 2.0 * qy * qw;//ax
    Trans[1][2] = 2.0 * qy * qz - 2.0 * qx * qw;//ay?
    Trans[2][2] = -qx * qx - qy * qy + qz * qz + qw * qw;//az
    Trans[3][2] = 0.0;//0

    Trans[0][3] = x;//px
    Trans[1][3] = y;//py
    Trans[2][3] = z;//pz
    Trans[3][3] = 1.0;//1
}
double cal_a() {
    //double x = (m + n) * Trans[0][0] - Trans[0][3];
    double x = (m + n) * Trans[0][2] - Trans[0][3];
    //double y = (m + n) * Trans[1][0] - Trans[1][3];
    double y = (m + n) * Trans[1][2] - Trans[1][3];

    double a = rad2deg(atan2(y, x));
    if (x == 0 && y == 0)
    {
        printf("atan2error in a : x and y equal 0\n");
        return a;
    }
    else if (a_min > a || a_max < a)
    {
            printf("atan2error in a : No answer\n");
            return EOF;
    }
        return a;
}

double cal_b(double a)   {
    //double A = Trans[2][3] - (m + n) * Trans[2][0] - h;
    double A = Trans[2][3] - (m + n) * Trans[2][2] - h;
    //double B = Trans[0][3] * C(a) + Trans[1][3] * S(a) - (m + n) * (Trans[0][0] * C(a) + Trans[1][0] * S(a)) - g;
    double B = Trans[0][3] * C(a) + Trans[1][3] * S(a) - (m + n) * (Trans[0][2] * C(a) + Trans[1][2] * S(a)) - g;
    double D = (A * A + B * B + j * j - k * k - l * l) / (2 * j);
    double b1 = rad2deg(2 * atan2(-B + sqrt(abs(B * B + A * A - D * D)), A + D));
    double b2 = rad2deg(2 * atan2(-B - sqrt(abs(B * B + A * A - D * D)), A + D));

    //�G���[���N���闝�R�Fsqrt�̒��g�����ɂȂ��Ă���C�Ȃ�ꍇ������

    //printf("sqrt(D=b^2-4ac)=%lf\n" ,- B + sqrt(B * B + A * A - D * D));

    if (b1 > 180.0) { b1 = b1 - 180.0; }
    else if (b1 < -180.0) { b1 = b1 + 360; }
    if (b2 > 180.0) { b2 = b2 - 180.0; }
    else if (b2 < -180.0) { b2 = b2 + 360; }

    //printf("A=%lf\nB=%lf\nD=%lf\nb1=%lf\nb2=%lf\n\n", A, B, D, b1, b2);

    //������łĂ���̂Ŕ͈͊O�Ȃ�͂���
    if ((b_min<=b1 && b_max>=b1) && (b_min > b2 || b_max < b2)) {
        return b1;
    }
    else if ((b_min<=b2 && b_max>=b2) && (b_min > b1 || b_max < b1)) {
        return b2;
    }
    else if ((b_min<b1 && b_max>b1) && (b_min<b2 && b_max>b2))
    {//�����Ƃ����ɂȂ肤��ꍇ,�Ƃ肠�����G���[
        printf("answer error in b : two answer\n");
        return EOF;
    }
    else if ((b_min > b1 || b_max < b1) && (b_min > b2 || b_max < b2))
    {//�������Ȃ��ꍇ
        printf("atan2error in b�FNo answer\n");
        return EOF;
    }
    else
    {
        printf("error in b:�Ȃ񂩂��������D\n");
        return EOF;
    }
}
double cal_c(double a) {
    //double A = Trans[2][3] - (m + n) * Trans[2][0] - h;
    double A = Trans[2][3] - (m + n) * Trans[2][2] - h;
    //double B = Trans[0][3] * C(a) + Trans[1][3] * S(a) - (m + n) * (Trans[0][0] * C(a) + Trans[1][0] * S(a)) - g;
    double B = Trans[0][3] * C(a) + Trans[1][3] * S(a) - (m + n) * (Trans[0][2] * C(a) + Trans[1][2] * S(a)) - g;
    double E = (A * A + B * B - j * j - k * k - l * l) / (2 * j);
    double c1 = rad2deg(2 * atan2(-l + sqrt(abs(l * l + k * k - E * E)), E + k));
    double c2 = rad2deg(2 * atan2(-l - sqrt(abs(l * l + k * k - E * E)), E + k));
    


    //���̒���
    if (c1 > 180.0) { c1 = c1 - 180.0; }
    else if (c1 < -180.0) { c1 = c1 + 360; }
    if (c2 > 180.0) { c2 = c2 - 180.0; }
    else if (c2 < -180.0) { c2 = c2 + 360; }
    
    //������łĂ���̂Ŕ͈͊O�Ȃ�͂���
    if ((c_min<c1 && c_max>c1) && (c_min > c2 || c_max < c2)) {
        return c1;
    }
    else if ((c_min<c2 && c_max>c2) && (c_min > c1 || c_max < c1)) {
        return c2;
    }
    else if ((c_min<c1 && c_max>c1) && (c_min<c2 && c_max>c2))
    {//�����Ƃ����ɂȂ肤��ꍇ,�Ƃ肠�����G���[
        printf("answer error in c : two answer\n");
        return EOF;
    }
    else if ((c_min > c1 || c_max < c1) && (c_min > c2 || c_max < c2))
    {//�������Ȃ��ꍇ
        printf("atan2error in c�FNo answer\n");
        return EOF;
    }
    else
    {
        printf("error in c:�Ȃ񂩂��������D\n");
        return EOF;
    }
}
double cal_d(double a, double b, double c) {
    //double x = -1 * (Trans[0][0] * C(a) + Trans[1][0] * S(a)) * S(b + c) + Trans[3][0] * C(b + c);
    //double y = Trans[0][0] * S(a) - Trans[1][0] * S(a);
    double x = -1 * (Trans[0][2] * C(a) + Trans[1][2] * S(a)) * S(b + c) + Trans[2][2] * C(b + c);
    double y = Trans[0][2] * S(a) - Trans[1][2] * C(a);

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
    //double x = (Trans[0][0] * S(a) + Trans[1][0] * C(a)) * S(d) - ((Trans[0][0] * C(a) + Trans[1][0] * S(a)) * S(b + c) - Trans[3][0] * C(b + c)) * C(d);
    //double y = (Trans[0][0] * C(a) + Trans[1][0] * S(a)) * C(b + c) + Trans[3][0] * S(b + c);
    double x = (Trans[0][0] * S(a) - Trans[1][0] * C(a)) * S(d) - ((Trans[0][0] * C(a) + Trans[1][0] * S(a)) * S(b + c) - Trans[2][0] * C(b + c)) * C(d);
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
}
double cal_f(double a, double b, double c) {
    //double x = (Trans[0][2] * C(a) + Trans[1][2] * S(a)) * C(b + c) + Trans[2][2] * S(b + c);
    double x = (Trans[0][0] * C(a) + Trans[1][0] * S(a)) * C(b + c) + Trans[2][0] * S(b + c);
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
}
