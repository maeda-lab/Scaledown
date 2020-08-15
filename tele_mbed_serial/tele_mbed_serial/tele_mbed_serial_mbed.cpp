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


int aOut, bOut, cOut, dOut, eOut, fOut;//���ꂼ��̊p�x
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
//github����̃R�s�[�����ǁA1�x������̃p���X���͌v�Z���Ȃ����K�v�����邩��
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

//�󂯎�����l��ϊ�
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
    //�����Ă���̂͐���3���{����1���ł͂Ȃ�
    //10�{���ꂽ�Ȃ��Ƃɒ���
    pw0 = cal_input0(0.1 * (float)aOut);
    pw1 = cal_input1(0.1 * (float)bOut);
    pw2 = cal_input2(0.1 * (float)cOut);
    pw3 = cal_input3(0.1 * (float)dOut);
    pw4 = cal_input4(0.1 * (float)eOut);
    pw5 = cal_input5(0.1 * (float)fOut);
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
    int cnt;//�J�E���^
    unsigned char ch;//serial�p�̕����󂯎��ꏊ
    //unsigned char data;
    float w = 50.0;//������g��,�K���50�ł���ȏ�傫������Ƃ�΂������B
    float PWMperiod = 1.0 / w;    //PWM�����̌v�Z

    pc.baud(115200);

    servo0.period(PWMperiod);          // servo0��Pwm������20ms�Ƃ���
    servo1.period(PWMperiod);          // servo1��Pwm������20ms�Ƃ���
    servo2.period(PWMperiod);          // servo2��Pwm������20ms�Ƃ���
    servo3.period(PWMperiod);          // servo3��Pwm������20ms�Ƃ���
    //SG90�̂ق�
    servo4.period(PWMperiod);          // servo4��Pwm������20ms�Ƃ���
    servo5.period(PWMperiod);          // servo5��Pwm������20ms�Ƃ���

    pc.printf("Prease push key:\nr:Moving to FirstPlace\nq:quit\na:SerialTest\ns:SerialStart\n");
    ///�󂯎�菀���J�n
    while (1)
    {
        ch = pc.getc();         // 1������M�o�b�t�@�����o��
        switch (ch)
        {
        case 't':
            pc.putc(1);
            break;
        case 'r':
            //�����ʒu�֓���
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
            //�V���A���ʐM�̃e�X�g
            pc.printf("serial test starts\n");
            cnt = 0;
            //num�̏�����
            num_ini();

            while (1)
            {
                ch = pc.getc();

                num[cnt] = ctoi(ch);//�����𐔒l�ɕϊ�
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
            //�V���A���ʐM���X�^�[�g����
            pc.printf("serial starts\n");
            cnt = 0;
            //num�̏�����
            num_ini();
            while (1)
            {

                ch = pc.getc();         // 1������M�o�b�t�@�����o��
                //data = pc.getc();
                num[cnt] = ctoi(ch);//�����𐔒l�ɕϊ�

                if (cnt >= 23)
                {
                    //������Ƒ���Ă��邩�ǂ����̃`�F�b�N
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