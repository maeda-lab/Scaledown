//mbed側のプログラムです
#include "mbed.h"

PwmOut servo0(p21);//θ0に対応するピン
PwmOut servo1(p22);//θ1に対応するピン
PwmOut servo2(p23);//θ2に対応するピン
PwmOut servo3(p24);//θ3に対応するピン
PwmOut servo4(p25);//θ4に対応するピン
PwmOut servo5(p26);//θ5に対応するピン

Serial pc(USBTX, USBRX);
LocalFileSystem local("local");
float aOut, bOut, cOut, dOut, eOut, fOut;//それぞれの角度
int num[30];//数字格納場所，基本的に1桁の数字しか入らない

int a_num[8];
int b_num[8];
int c_num[8];
int d_num[8];
int e_num[8];
int f_num[8];



int pw0, pw1, pw2, pw3, pw4, pw5;//出力パルス幅

//きちんと値が受け取れているかチェックするための初期化
//角度[°]から入力パルス幅[us]に変換する関数
//(逆運動学時の初期位置に対応するパルス幅)＋（deg->us）*(初期姿勢角＋変角arg)

int cal_input0(float arg) {
    return 1475 + 8.6667 * (0.0 + arg);
}
int cal_input1(float arg) {
    return 1520 + 10.2467 * (-30.0 + arg);
}
int cal_input2(float arg) {
    return 853 + 11.11 * (30.0 + arg);
}
int cal_input3(float arg) {
    return 1224 + int(8.556 * arg);
}
int cal_input4(float arg) {
    return 1460 + int(10.556 * arg);
}
int cal_input5(float arg) {
    return 1922 + int(10.556 * arg);
}


void cal_pw()
{
    //送られてくるのは整数3桁＋小数1桁ではなく
    //10倍されたなことに注意
    pw0 = cal_input0(aOut);
    pw1 = cal_input1(bOut);
    pw2 = cal_input2(cOut);
    pw3 = cal_input3(dOut);
    pw4 = cal_input4(eOut);
    pw5 = cal_input5(fOut);
}
void send_servo()
{
    servo0.pulsewidth_us(pw0);//パルス変調幅を1625usで入力
    servo1.pulsewidth_us(pw1);//パルス変調幅を1420usで入力
    servo2.pulsewidth_us(pw2); //パルス変調幅を 632usで入力
    servo3.pulsewidth_us(pw3);//パルス変調幅を1310usで入力
    servo4.pulsewidth_us(pw4);//パルス変調幅を1368usで入力
    servo5.pulsewidth_us(pw5);//パルス変調幅を1500usで入力
}

//動く関数．総まとめ
void move()
{
    //cal_Out();
    cal_pw();
    send_servo();
}

int main()
{
    FILE* fp;
    fp = fopen("/local/Out.csv", "w");
    fprintf(fp, "cnt,aOut,bOut,cOut,dOut,eOut,fOut\n");
    float w = 60.0;//制御周波数,規定は50でこれ以上大きくするとやばいかも。
    float PWMperiod = 1.0 / w;    //PWM周期の計算
    pc.baud(115200);
    servo0.period(PWMperiod);          // servo0のPwm周期を20msとする
    servo1.period(PWMperiod);          // servo1のPwm周期を20msとする
    servo2.period(PWMperiod);          // servo2のPwm周期を20msとする
    servo3.period(PWMperiod);          // servo3のPwm周期を20msとする
    servo4.period(PWMperiod);          // servo4のPwm周期を20msとする
    servo5.period(PWMperiod);          // servo5のPwm周期を20msとする

    ///受け取り準備開始
    //初期位置の設定
    pw0 = 0.0; pw1 = 0.0; pw2 = 0.0; pw3 = 0.0; pw4 = 0.0; pw5 = 0.0;
    //シリアル通信をスタートする
    pc.printf("serial starts\n");
    for(cnt=0;cnt<100;cnt++)
    //while (1)
    {
        pc.scanf("%f", &aOut);
        pc.scanf("%f", &bOut);
        pc.scanf("%f", &cOut);
        pc.scanf("%f", &dOut);
        pc.scanf("%f", &eOut);
        pc.scanf("%f", &fOut);
        fprintf(fp, "%d,%d,%d,%d,%d,%d,%d\n",aOut, bOut, cOut, dOut, eOut, fOut);

        //move();
    }


    fclose(fp);

    //FILE *fp;
    //fp = fopen("/local/Out.csv","w");
    //fprintf(fp,"cnt,aOut,bOut,cOut,dOut,eOut,fOut\n");
    //
    //for(cnt=0;cnt<100;cnt++)
    //{
    //   fprintf(fp,"%d,%d,%d,%d,%d,%d,%d\n",cnt,aOut,bOut,cOut,dOut,eOut,fOut);
    //}
    //fclose(fp);
    wait(0.1);

}
