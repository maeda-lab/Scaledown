#include "mbed.h"

PwmOut servo0(p21);//��0�ɑΉ�����s��
PwmOut servo1(p22);//��1�ɑΉ�����s��
PwmOut servo2(p23);//��2�ɑΉ�����s��
PwmOut servo3(p24);//��3�ɑΉ�����s��
PwmOut servo4(p25);//��4�ɑΉ�����s��
PwmOut servo5(p26);//��5�ɑΉ�����s��

//name.baud(9600);
//MG996R�̂ق�

Serial pc(USBTX, USBRX);
LocalFileSystem local("local");

float aOut, bOut, cOut, dOut, eOut, fOut;//���ꂼ��̊p�x
int num[30];//�����i�[�ꏊ�C��{�I��1���̐�����������Ȃ�
int pw0, pw1, pw2, pw3, pw4, pw5;//�o�̓p���X��



//������ƒl���󂯎��Ă��邩�`�F�b�N���邽�߂̏�����
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
//char�^�Ŏ󂯎����������int�^�ɕԂ�
int ctoi(char c)
{
    //1�����̐����ichar�^�j�𐔒l�iint�^�j�ɕϊ�
    if ('0' <= c && c <= '9')
    {
        return (c - '0');
    }
    else
    {
        return -1;
    }
}

//�p�x[��]������̓p���X��[us]�ɕϊ�����֐�
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


//�󂯎�����l��ϊ�
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

       //�����Ă���̂͐���2���{����3���ł͂Ȃ�
    //1000�{���ꂽ�Ȃ��Ƃɒ���
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
    servo0.pulsewidth_us(pw0);//�p���X�ϒ�����1625us�œ���
    servo1.pulsewidth_us(pw1);//�p���X�ϒ�����1420us�œ���
    servo2.pulsewidth_us(pw2); //�p���X�ϒ����� 632us�œ���
    servo3.pulsewidth_us(pw3);//�p���X�ϒ�����1310us�œ���
    servo4.pulsewidth_us(pw4);//�p���X�ϒ�����1368us�œ���
    servo5.pulsewidth_us(pw5);//�p���X�ϒ�����1500us�œ���
}

//�����֐��D���܂Ƃ�
void move()
{
    cal_Out();
    cal_pw();
    send_servo();
}
int main()
{
    float output[500][6];
    float w = 60.0;//========������g��===========
    float PWMperiod = 1.0 / w;    //PWM�����̌v�Z
    char ch;
    //===============�{�[���[�g=============
    pc.baud(115200);

    servo0.period(PWMperiod);
    servo1.period(PWMperiod);
    servo2.period(PWMperiod);
    servo3.period(PWMperiod);
    servo4.period(PWMperiod);
    servo5.period(PWMperiod);

    ///�󂯎�菀���J�n
    //pc.printf("serial starts\n");
    //�V���A���ʐM���X�^�[�g����
    //num�̏�����
    num_ini();
    //while (1)
    for (int counter = 0; counter < 100; counter++)
    {

        //pc.gets(buf, 31);         // 1������M�o�b�t�@�����o��
        for (int i = 0; i < 30; i++)
        {
            ch = pc.getc();
            num[i] = ctoi(ch);//�����𐔒l�ɕϊ�
        }

        move();

        //���������珉��������
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