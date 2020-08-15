#include "mbed.h"

PwmOut servo0(p21);//θ0に対応するピン
PwmOut servo1(p22);//θ1に対応するピン
PwmOut servo2(p23);//θ2に対応するピン
PwmOut servo3(p24);//θ3に対応するピン
PwmOut servo4(p25);//θ4に対応するピン
PwmOut servo5(p26);//θ5に対応するピン

//name.baud(9600);
//MG996Rのほう

Serial pc(USBTX, USBRX);


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
//githubからのコピーだけど、1度あたりのパルス幅は計算しなおす必要があるかも
int cal_input0(float arg) {
    return 1645 + int(8.6667 * arg);
}
int cal_input1(float arg) {
    return 1420 + int(8.6667 * arg);
}
int cal_input2(float arg) {
    return 632 + int(9.64 * arg);
}
int cal_input3(float arg) {
    return 1310 + int(8.556 * arg);
}
int cal_input4(float arg) {
    return 1368 + int(10.556 * arg);
}
int cal_input5(float arg) {
    return 1500 + int(10.556 * arg);
}

//受け取った値を変換
void cal_Out()
{
    aOut = 1000 * num[3] + 100 * num[2] + 10 * num[1] + 1 * num[0];//
    bOut = 1000 * num[7] + 100 * num[6] + 10 * num[5] + 1 * num[4];//
    cOut = 1000 * num[11] + 100 * num[10] + 10 * num[9] + 1 * num[8];//
    dOut = 1000 * num[15] + 100 * num[14] + 10 * num[13] + 1 * num[12];//
    eOut = 1000 * num[19] + 100 * num[18] + 10 * num[17] + 1 * num[16];//
    fOut = 1000 * num[23] + 100 * num[22] + 10 * num[21] + 1 * num[20];//
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
    int cnt;//カウンタ
    unsigned char ch;//serial用の文字受け取り場所
    //unsigned char data;
    float w = 50.0;//制御周波数,規定は50でこれ以上大きくするとやばいかも。
    float PWMperiod = 1.0 / w;    //PWM周期の計算

    pc.baud(115200);

    servo0.period(PWMperiod);          // servo0のPwm周期を20msとする
    servo1.period(PWMperiod);          // servo1のPwm周期を20msとする
    servo2.period(PWMperiod);          // servo2のPwm周期を20msとする
    servo3.period(PWMperiod);          // servo3のPwm周期を20msとする
    //SG90のほう
    servo4.period(PWMperiod);          // servo4のPwm周期を20msとする
    servo5.period(PWMperiod);          // servo5のPwm周期を20msとする

    pc.printf("Prease push key:\nr:Moving to FirstPlace\nq:quit\na:SerialTest\ns:SerialStart\n");
    ///受け取り準備開始
    while (1)
    {
        ch = pc.getc();         // 1文字受信バッファより取り出し
        switch (ch)
        {
        case 't':
            pc.putc(1);
            break;
        case 'r':
            //初期位置へ動く
            pc.printf("moving to first place...\n");
            for (cnt = 0; cnt < 24; cnt++)
            {
                num[cnt] = 0;
            }
            move();
            pc.printf("moving finish!\n");
            break;
        case 'q':
            pc.printf("serial test is finished...\n");
            break;
        case 'a':
            //シリアル通信のテスト
            pc.printf("serial test starts\n");
            cnt = 0;
            //numの初期化
            num_ini();

            while (1)
            {
                ch = pc.getc();

                num[cnt] = ctoi(ch);//数字を数値に変換
                if (cnt > 23)
                {
                    //pc.putc();
                    move();
                    break;
                }
                cnt++;
            }
            break;
        case 's':
            //シリアル通信をスタートする
            pc.printf("serial starts\n");
            cnt = 0;
            //numの初期化
            num_ini();
            while (1)
            {

                ch = pc.getc();         // 1文字受信バッファより取り出し
                //data = pc.getc();
                num[cnt] = ctoi(ch);//数字を数値に変換

                if (cnt >= 23)
                {
                    //きちんと送れているかどうかのチェック
                    move();
                }
                if (ch == 'e')
                {
                    break;
                }
                cnt++;
            }
            break;
        }
        wait(0.1);
    }

}