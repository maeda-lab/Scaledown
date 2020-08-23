#include "mbed.h"

PwmOut servo0(p21);//θ0に対応するピン
PwmOut servo1(p22);//θ1に対応するピン
PwmOut servo2(p23);//θ2に対応するピン
PwmOut servo3(p24);//θ3に対応するピン
PwmOut servo4(p25);//θ4に対応するピン
PwmOut servo5(p26);//θ5に対応するピン

Serial pc(USBTX, USBRX);
LocalFileSystem local("local");
int aOut, bOut, cOut, dOut, eOut, fOut;//それぞれの角度
int num[30];//数字格納場所，基本的に1桁の数字しか入らない
int pw0, pw1, pw2, pw3, pw4, pw5;//出力パルス幅

//きちんと値が受け取れているかチェックするための初期化
void num_ini()
{
    for (int i = 0; i < 30; i++)
    {
        num[i] = 9999;
    }
}
bool num_check(int n[])
{
    for (int i = 0; i < 24; i++)
    {
        if (n[i] == 9999)
            return -1;
    }
    return 1;
}
//char型で受け取った文字をint型に返す
int ctoi(char c)
{
    //1文字の数字（char型）を数値（int型）に変換
    if ('0' <= c && c <= '9')
    {
        return (c - '0');
    }
    else
    {
        return -1;
    }
}

//角度[°]から入力パルス幅[us]に変換する関数
//(逆運動学時の初期位置に対応するパルス幅)＋（deg->us）*(初期姿勢角＋変角arg)

int cal_input0(float arg){
 return 1475 + 8.6667*( 0.0 + arg); }
int cal_input1(float arg){
 return 1520 + 10.2467*( -30.0 + arg ); }
int cal_input2(float arg){
 return 853 + 11.11*( 30.0 + arg); }
int cal_input3(float arg){
 return 1224+int(8.556*arg); }
int cal_input4(float arg){
 return 1460+int(10.556*arg); }
int cal_input5(float arg){
 return 1922+int(10.556*arg); }
 

//受け取った値を変換
void cal_Out()
{
    aOut = 1000 * num[3]  + 100 * num[2]  + 10 * num[1]  + 1 * num[0]  - 1200;//
    bOut = 1000 * num[7]  + 100 * num[6]  + 10 * num[5]  + 1 * num[4]  - 730 ;//
    cOut = 1000 * num[11] + 100 * num[10] + 10 * num[9]  + 1 * num[8]  - 30  ;//
    dOut = 1000 * num[15] + 100 * num[14] + 10 * num[13] + 1 * num[12] - 900 ;//
    eOut = 1000 * num[19] + 100 * num[18] + 10 * num[17] + 1 * num[16] - 1000;//
    fOut = 1000 * num[23] + 100 * num[22] + 10 * num[21] + 1 * num[20] - 1000;//
}
void cal_pw()
{
    //送られてくるのは整数3桁＋小数1桁ではなく
    //10倍されたなことに注意
    pw0 = cal_input0(0.1 * (float)aOut);
    pw1 = cal_input1(0.1 * (float)bOut);
    pw2 = cal_input2(0.1 * (float)cOut);
    pw3 = cal_input3(0.1 * (float)dOut);
    pw4 = cal_input4(0.1 * (float)eOut);
    pw5 = cal_input5(0.1 * (float)fOut);
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
    cal_Out();
    cal_pw();
    send_servo();
}

int main()
{
    int cnt=0;//カウンタ
    char ch[256];//serial用の文字受け取り場所
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
    pw0=0.0;pw1=0.0;pw2=0.0;pw3=0.0;pw4=0.0;pw5=0.0;
    //シリアル通信をスタートする
    pc.printf("serial starts\n");
    //numの初期化
    //for(int cnt=0;cnt<5;cnt++)
    while(1)
    {
        num_ini();
        pc.gets(ch,25);//24個受け取ったら次の処理
        for(int i=0;i<24;i++)
        {
             num[i] = ctoi(ch[i]);//数字を数値に変換
        }
        //きちんと送れているかどうかのチェック
        move();
        //動かしたら初期化する
        num_ini();
        cnt++;
        wait(0.1);
    }    
}    