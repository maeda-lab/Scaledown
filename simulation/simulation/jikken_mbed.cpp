#include "mbed.h"
#include <stdio.h>

PwmOut servo0(p21);//θ0 pin
PwmOut servo1(p22);//θ1 pin
PwmOut servo2(p23);//θ2 pin
PwmOut servo3(p24);//θ3 pin
PwmOut servo4(p25);//θ4 pin
PwmOut servo5(p26);//θ5 pin

//for plogram check
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

Serial pc(USBTX, USBRX);
LocalFileSystem local("local");

float aOut, bOut, cOut, dOut, eOut, fOut;//J123456 degree
int pw0, pw1, pw2, pw3, pw4, pw5;//Output Pulse Width

//degree to PulseWidth
int cal_input0(float arg) {
    return 1475 + int(10.48 * arg);
}
int cal_input1(float arg) {
    return 1218 + int(10.2467 * arg);
}
int cal_input2(float arg) {
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
//calcurate PulseWidth
void cal_pw()
{
    pw0 = cal_input0(aOut);
    pw1 = cal_input1(bOut);
    pw2 = cal_input2(cOut);
    pw3 = cal_input3(dOut);
    pw4 = cal_input4(eOut);
    pw5 = cal_input5(fOut);
}
//send PulsWidth to servo
void send_servo()
{
    servo0.pulsewidth_us(pw0);
    servo1.pulsewidth_us(pw1);
    servo2.pulsewidth_us(pw2);
    servo3.pulsewidth_us(pw3);
    servo4.pulsewidth_us(pw4);
    servo5.pulsewidth_us(pw5);
}

//move function set
void move()
{
    cal_pw();
    send_servo();
}
int main(void)
{
    //pc.baud(115200);
    //pc.format(8, Serial::None, 1);
    //pc.printf("start\n");
    float w = 60.0;//========Control Frequency===========
    float PWMperiod = 1.0 / w;    
    servo0.period(PWMperiod);
    servo1.period(PWMperiod);
    servo2.period(PWMperiod);
    servo3.period(PWMperiod);
    servo4.period(PWMperiod);
    servo5.period(PWMperiod);
    led4=1.0;
    float Out[2548][6];
    led2=1.0;
    int i;
    FILE* fp;

    fp = fopen("/local/deg123data_scale_1_4xy.csv", "r");
    led1=1.0;
    if (fp == NULL)
    {
        led1 = 1.0;
        //pc.printf("error\n");
    }
    else
    {
        led3=1.0;
        //pc.printf("moving\n");
        for(i=0;i<2548;i++)
        {
            led3=0.0;
            fscanf(fp, "%f,%f,%f\n",&Out[i][0],&Out[i][1],&Out[i][2]);
            Out[i][3]=0.0;
            Out[i][4]=0.0;
            Out[i][5]=0.0;
            led3=1.0;
            aOut = Out[i][0];
            bOut = Out[i][1];
            cOut = Out[i][2];
            dOut = Out[i][3];
            eOut = Out[i][4];
            fOut = Out[i][5];                                                
            move();
            wait(0.16);//wait
        }
        fclose(fp);
        led2=1.0;
        /*for(i=0;i<2548;i++)
        {
            aOut = Out[i][0];
            bOut = Out[i][1];
            cOut = Out[i][2];
            dOut = Out[i][3];
            eOut = Out[i][4];
            fOut = Out[i][5];                                                
            move();
            //wait_ms(0.16);
        } */  
    }
    led1=0.0;
    led2=0.0;
    led3=0.0;
    led4=0.0;
    return 0;
}