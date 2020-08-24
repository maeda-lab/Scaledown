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
LocalFileSystem local("local");

float aOut, bOut, cOut, dOut, eOut, fOut;//それぞれの角度
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
int cal_input0(float arg) {
    return 1475 + int(10.48 * arg);
}
int cal_input1(float arg) {
    //return 1520 + 10.2467 * (-30.0 + arg);
    return 1218 + int(10.2467 * arg);
}
int cal_input2(float arg) {
    //return 853 + 10.59 * (30.0 + arg);
    return 1306 + 10.59 * arg;
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


//受け取った値を変換
void cal_Out()
{
    //aOut = 1000 * num[3] + 100 * num[2] + 10 * num[1] + 1 * num[0];//
    //bOut = 1000 * num[7] + 100 * num[6] + 10 * num[5] + 1 * num[4];//
    //cOut = 1000 * num[11] + 100 * num[10] + 10 * num[9] + 1 * num[8];//
    //dOut = 1000 * num[15] + 100 * num[14] + 10 * num[13] + 1 * num[12];//
    //eOut = 1000 * num[19] + 100 * num[18] + 10 * num[17] + 1 * num[16];//
    //fOut = 1000 * num[23] + 100 * num[22] + 10 * num[21] + 1 * num[20];//
    //aOut = 1000 * num[4] + 100 * num[3] + 10 * num[2] + 1 * num[1];//
    //bOut = 1000 * num[8] + 100 * num[7] + 10 * num[6] + 1 * num[5];//
    //cOut = 1000 * num[12] + 100 * num[11] + 10 * num[10] + 1 * num[9];//
    //dOut = 1000 * num[16] + 100 * num[15] + 10 * num[14] + 1 * num[13];//
    //eOut = 1000 * num[20] + 100 * num[19] + 10 * num[18] + 1 * num[17];//
    //fOut = 1000 * num[24] + 100 * num[23] + 10 * num[22] + 1 * num[21];//

       //送られてくるのは整数2桁＋小数3桁ではなく
    //1000倍されたなことに注意
    aOut = 10.0 * num[4] + 1.0 * num[3] + 0.1 * num[2] + 0.01 * num[1] + 0.001 * num[0] - 60.0;//
    bOut = 10.0 * num[9] + 1.0 * num[8] + 0.1 * num[7] + 0.01 * num[6] + 0.001 * num[5] - 60.0;//
    cOut = 10.0 * num[14] + 1.0 * num[13] + 0.1 * num[12] + 0.01 * num[11] + 0.001 * num[10] - 60.0;//
    dOut = 10.0 * num[19] + 1.0 * num[18] + 0.1 * num[17] + 0.01 * num[16] + 0.001 * num[15] - 60.0;//
    eOut = 10.0 * num[24] + 1.0 * num[23] + 0.1 * num[22] + 0.01 * num[21] + 0.001 * num[20] - 60.0;//
    fOut = 10.0 * num[29] + 1.0 * num[28] + 0.1 * num[27] + 0.01 * num[26] + 0.001 * num[25] - 60.0;//

    //aOut = 10.0 * num[28] + 1.0 * num[27] + 0.1 * num[26] + 0.01 * num[25] + 0.001 * num[24] - 60.0;//
    //bOut = 10.0 * num[3]  + 1.0 * num[2]  + 0.1 * num[1]  + 0.01 * num[0]  + 0.001 * num[29] - 60.0;//
    //cOut = 10.0 * num[8]  + 1.0 * num[7]  + 0.1 * num[6]  + 0.01 * num[5]  + 0.001 * num[4]  - 60.0;//
    //dOut = 10.0 * num[13] + 1.0 * num[12] + 0.1 * num[11] + 0.01 * num[10] + 0.001 * num[9]  - 60.0;//
    //eOut = 10.0 * num[18] + 1.0 * num[17] + 0.1 * num[16] + 0.01 * num[15] + 0.001 * num[14] - 60.0;//
    //fOut = 10.0 * num[23] + 1.0 * num[22] + 0.1 * num[21] + 0.01 * num[20] + 0.001 * num[19] - 60.0;//

}
void cal_pw()
{
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
    cal_Out();
    cal_pw();
    send_servo();
}
int main()
{
    float output[500][6];
    float w = 60.0;//========制御周波数===========
    float PWMperiod = 1.0 / w;    //PWM周期の計算
    char ch;
    //===============ボーレート=============
    pc.baud(115200);

    servo0.period(PWMperiod);
    servo1.period(PWMperiod);
    servo2.period(PWMperiod);
    servo3.period(PWMperiod);
    servo4.period(PWMperiod);
    servo5.period(PWMperiod);

    ///受け取り準備開始
    //pc.printf("serial starts\n");
    //シリアル通信をスタートする
    //numの初期化
    num_ini();
    //while (1)
    for (int counter = 0; counter < 100; counter++)
    {

        //pc.gets(buf, 31);         // 1文字受信バッファより取り出し
        for (int i = 0; i < 30; i++)
        {
            ch = pc.getc();
            num[i] = ctoi(ch);//数字を数値に変換
        }

        move();

        //動かしたら初期化する
        output[counter][0] = aOut;
        output[counter][1] = bOut;
        output[counter][2] = cOut;
        output[counter][3] = dOut;
        output[counter][4] = eOut;
        output[counter][5] = fOut;
        num_ini();

    }
    FILE* fp;
    fp = fopen("/local/Output.csv", "w");
    if (fp == NULL)
    {
        //pc.printf("error");
    }
    //while (1)
    fprintf(fp, "aOut,bOut,cOut,dOut,eOut,fOut\n");
    for (int j = 0; j < 500; j++)
    {
        fprintf(fp, "%f,%f,%f,%f,%f,%f\n", output[j][0], output[j][1], output[j][2], output[j][3], output[j][4], output[j][5]);
    }
    fclose(fp);
    wait(0.1);

}