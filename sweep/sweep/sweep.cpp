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
#define f_min 0.1
#define f_max 10.0

//���g���̑����̒�`
//���g����1s������ǂꂾ�����₷��
float delta_f = 0.25;

//��]���a�̒�`[mm]
#define r 20.0  //�v�ύX

//�b���̑����̒�`[s]
//�T�[�{�̐�����g���ƍ��킹��
//#define delta_t 1/60

//�ʒu��\���\���̂̐ݒ�
struct Pos {
	float x;//�X�C�[�v�Ȑ�����ړ�����_�̂��鎞��t�ɂ�����x���W
	float y;//�X�C�[�v�Ȑ�����ړ�����_�̂��鎞��t�ɂ�����y���W
};

int main(void)
{
	Pos pos;//�ꎞ�ۑ�����\����
	FILE* fp;
	errno_t error_fp;
	const char* filename = "sweep.csv";//�t�@�C���̕ۑ���
	printf("%f", delta_f / 60.0);
	//======file���������݃��[�h�ŃI�[�v��=======
	error_fp = fopen_s(&fp, filename, "w");
	if (fp==NULL)
	{
		printf("�t�@�C����Open�o���܂���!\n");
		exit(0);
	}
	else
	{
		//===========-sweep�Ȑ����쐬����========
		//"2458"�̓T�[�{�̐�����g������v�Z(40.96s * 60[1/s]=2457.6�C2458�̓_����邱�Ƃ��ł���)
		for (float t = 0.0; t < 2458.0; t++)
		{
			//(�P�ʉ~�̔��ar)*(PI)*(����t�ɂ�������g��f)*(����t)
			pos.x = r * cos(2.0 * PI * (f_min + delta_f / 60.0 * t) * t/60.0);
			pos.y = r * sin(2.0 * PI * (f_min + delta_f / 60.0 * t) * t/60.0);

			//======�������ݏ������s��=======
			fprintf(fp, "%lf,%lf\n", pos.x, pos.y);
			
		}
		printf("�������݊���");
		fclose(fp);
	}
	return 0;
}