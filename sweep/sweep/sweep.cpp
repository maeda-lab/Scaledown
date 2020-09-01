//2020/08/31 TomokiHirayama �X�V
//2020/08/25�@TomokiHirayama
//�X�C�[�v�Ȑ����쐬����v���O�����ł��D
//�v�Z���ʂ��ꎞ�I�ɕۑ�����z�񂪃X�^�b�N�T�C�Y�I�[�o�[�̂���
//�p�ӂł��Ȃ������̂ŁC�v�Z���ʂ����̂܂܏�������ł��܂��D


#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define PI 3.14159265358979

//���g���̒�`[Hz]
//0.1Hz(=���10�b�Ȃ�A�[���͒ǂ����Ă��邾�낤)
// 10Hz(=���0.1�b�Ȃ�A�[���͒ǂ����Ȃ����낤)
#define f_min 0.2
#define f_max 8.0

//�T���v�����O���[�g
#define Sampling_rate 60
//�T���v�����O��
#define Point 8192
//���g���̑����̒�`
//���g����1s������ǂꂾ�����₷��
double delta_f = (f_max - f_min) / (Point - 1); //0.25;

//��]���a�̒�`[mm]
#define r 200.0  //�v�ύX

//�b���̑����̒�`[s]
//�T�[�{�̐�����g���ƍ��킹��
//#define delta_t 201/60

//�ʒu��\���\���̂̐ݒ�
struct Pos {
	double x;//�X�C�[�v�Ȑ�����ړ�����_�̂��鎞��t�ɂ�����x���W
	double y;//�X�C�[�v�Ȑ�����ړ�����_�̂��鎞��t�ɂ�����y���W
};

int main(void)
{
	Pos pos;//�ꎞ�ۑ�����\����
	FILE* fp;
	errno_t error_fp;
	const char* filename = "r200_f80_f02_P81newsweep.csv";//�t�@�C���̕ۑ���
	//printf("%f", delta_f / 60.0);
	//======file���������݃��[�h�ŃI�[�v��=======
	error_fp = fopen_s(&fp, filename, "w");
	if (fp==NULL)
	{
		printf("�t�@�C����Open�o���܂���!\n");
		exit(0);
	}
	else
	{
		//===========�ŏ���0.1Hz��10�b(=1��)��==========
		for (int cnt = 0; cnt < int(1.0/f_min)*Sampling_rate*5; cnt++)
		{
			//(�P�ʉ~�̔��ar)*(PI)*(����t�ɂ�������g��f)*(����t)
			pos.x = r * cos(2.0 * PI * f_min * cnt / Sampling_rate);
			pos.y = r * sin(2.0 * PI * f_min * cnt / Sampling_rate);

			//======�������ݏ������s��=======
			fprintf(fp, "%lf,%lf\n", pos.x, pos.y);

		}
		//===========-sweep�Ȑ����쐬����========
		for (int sn = 0; sn < Point; sn++)
		{
			//(�P�ʉ~�̔��ar)*(PI)*(����t�ɂ�������g��f)*(����t)
			pos.x = r * cos(2.0 * PI * ( f_min + delta_f * sn ) * sn / Sampling_rate);
			pos.y = r * sin(2.0 * PI * ( f_min + delta_f * sn ) * sn / Sampling_rate);

			//======�������ݏ������s��=======
			fprintf(fp, "%lf,%lf\n", pos.x, pos.y);
			
		}
		printf("�������݊���");
		fclose(fp);
	}
	return 0;
}