#include "mbed.h"

PwmOut servo0(p21);//��0�ɑΉ�����s��
PwmOut servo1(p22);//��1�ɑΉ�����s��
PwmOut servo2(p23);//��2�ɑΉ�����s��
PwmOut servo3(p24);//��3�ɑΉ�����s��
PwmOut servo4(p25);//��4�ɑΉ�����s��
PwmOut servo5(p26);//��5�ɑΉ�����s��

Serial pc(USBTX, USBRX);
LocalFileSystem local("local");
float aOut, bOut, cOut, dOut, eOut, fOut;//���ꂼ��̊p�x
int num[30];//�����i�[�ꏊ�C��{�I��1���̐�����������Ȃ�

int a_num[8];
int b_num[8];
int c_num[8];
int d_num[8];
int e_num[8];
int f_num[8];



int pw0, pw1, pw2, pw3, pw4, pw5;//�o�̓p���X��

//������ƒl���󂯎��Ă��邩�`�F�b�N���邽�߂̏�����
//�p�x[��]������̓p���X��[us]�ɕϊ�����֐�
//(�t�^���w���̏����ʒu�ɑΉ�����p���X��)�{�ideg->us�j*(�����p���p�{�ϊparg)

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
    //�����Ă���̂͐���3���{����1���ł͂Ȃ�
    //10�{���ꂽ�Ȃ��Ƃɒ���
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
    //cal_Out();
    cal_pw();
    send_servo();
}

int main()
{
    FILE* fp;
    fp = fopen("/local/Out.csv", "w");
    fprintf(fp, "cnt,aOut,bOut,cOut,dOut,eOut,fOut\n");
    float w = 60.0;//������g��,�K���50�ł���ȏ�傫������Ƃ�΂������B
    float PWMperiod = 1.0 / w;    //PWM�����̌v�Z
    pc.baud(115200);
    servo0.period(PWMperiod);          // servo0��Pwm������20ms�Ƃ���
    servo1.period(PWMperiod);          // servo1��Pwm������20ms�Ƃ���
    servo2.period(PWMperiod);          // servo2��Pwm������20ms�Ƃ���
    servo3.period(PWMperiod);          // servo3��Pwm������20ms�Ƃ���
    servo4.period(PWMperiod);          // servo4��Pwm������20ms�Ƃ���
    servo5.period(PWMperiod);          // servo5��Pwm������20ms�Ƃ���

    ///�󂯎�菀���J�n
    //�����ʒu�̐ݒ�
    pw0 = 0.0; pw1 = 0.0; pw2 = 0.0; pw3 = 0.0; pw4 = 0.0; pw5 = 0.0;
    //�V���A���ʐM���X�^�[�g����
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