// Aug 25, 2020
// Masahiro Furukawa 
// 
// Serial Binary Communication
//
// reference : mbed_Serial.cpp written by Tomoki Hirayama

#include<stdio.h>
#include<iostream>
#include<tchar.h>
#include<stdlib.h>
#include<conio.h>
#include "mbed_Serial_binary.h"

#define PI 3.14159265358979
HANDLE h;//HANDLE�^�A�|�C���^�ł��B(void* )
DCB dcb; //�V���A���ʐM�f�o�C�X�̐����`��\��
COMMTIMEOUTS cto;//�ʐM�f�o�C�X�̃^�C���A�E�g�p�����[�^

//COMPORT *COM;

DWORD nn0, nn1;//32bit�����Ȃ������A�͈͂�0~18446744073709551615 10�i��
char buf0[1];
unsigned char buf1[1];
double com;
double rep;

int checknum[4];


HANDLE serial_open()
{
	//HANDLE h = CreateFile(_T("com3"), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE h = CreateFile(_T("COM1"), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	//�|�[�g�ԍ���PC�̐ݒ�Ƒ��k���܂��傤

	if (h == INVALID_HANDLE_VALUE) {
		//�J���Ȃ������ꍇ�ɂ�
		//�J�����ւ񂩂������I
		printf("PORT COULD NOT OPEN\n");
		//�v���O�����̈ꎞ��~
		system("PAUSE");
		//�v���O�����̐���I��
		exit(0);
	}
	printf("PORT COULD OPEN\n");
	return h;
}
HANDLE serial_initialaize(HANDLE h)
{
	BOOL Ret;
	//SetUpComm(HANDLE h, DWORD dwInQueue , DWORD dwOutQueue)
	//h:�ʐM�f�o�C�X�̃n���h��
	//dwInQueue :�f�o�C�X�̓������̓o�b�t�@�[�̐����T�C�Y
	//dwOutQueue:�f�o�C�X�̓����o�̓o�b�t�@�[�̐����T�C�Y
	Ret = SetupComm(h, 1024, 1024);
	if (!Ret) {
		//�ݒ�
		//���s������I
		printf("SET UP FAILED\n");
		//CloseHandle(HANDLE h)
		CloseHandle(h);
		system("PAUSE");
		exit(0);
	}
	printf("SET UP SUCCESSED\n");
	//����������
	//PurgeComm(HANDLE h,DWORD Flags)
	//�w�肳�ꂽ�|�[�g�̓��̓o�b�t�@�ɂ��邷�ׂĂ̕�����j������B
	Ret = PurgeComm(h, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!Ret) {
		//���������s������
		//���s�����ŁI
		printf("CLEAR FAILED\n");
		CloseHandle(h);
		exit(0);
	}

	return h;
}
HANDLE serial_Config(HANDLE h)
{
	//3.��{�ʐM�����̐ݒ�
	DCB dcb;
	BOOL Ret;
	//GetCommState(HANDLE h ,LPDCB lpdcb)
	//    h:�ʐM�f�o�C�X�ւ̃n���h��
	//lpdcb:����ݒ�����󂯎��DCB�\���ւ̃|�C���^
	GetCommState(h, &dcb);
	//dcb.DCBlength = sizeof(DCB);//length  :DCB�\���̂̑傫��(�o�C�g��)
	dcb.BaudRate = 115200;        //BaudRate:�ʐM�f�o�C�X�̃{�[���[�g
	//dcb.fBinary = TRUE;         //fBinary :�o�C�i�����[�h�̉�
	dcb.ByteSize = 8;           //ByteSize:����M����o�C�g�f�[�^�̃r�b�g��
	dcb.fParity = NOPARITY;     //fParity :�p���e�B�`�F�b�N�̉�
	dcb.StopBits = ONESTOPBIT;  //StopBits:�g�p����X�g�b�v�r�b�g���̎w��A1�X�g�b�v�r�b�g
	//SetCommState(HANDLE h ,LPDCB lpdcb)
	cto.ReadIntervalTimeout = 0;
	cto.ReadTotalTimeoutMultiplier = 0;
	cto.ReadTotalTimeoutConstant = 0;
	cto.WriteTotalTimeoutMultiplier = 0;
	cto.WriteTotalTimeoutConstant = 0;
	Ret = SetCommState(h, &dcb);
	SetCommTimeouts(h, &cto);
	if (!Ret) {
		//�ݒ�ł��Ȃ�������
		//���s�����ŁI
		printf("SetCommState FAILED\n");
		CloseHandle(h);
		system("PAUSE");
		exit(0);
	}
	printf("SetCommState SUCCESSED\n");
	return h;
}
void serial_Write(HANDLE h, double com, int cnt)//const char* data)
{
	//h:�V���A���ʐM�̐ݒ�̃|�C���^
	//com:���肽���f�[�^
	//cnt:�J�E���^
	DWORD dwSendSize;
	DWORD dwErrorMask;
	BOOL Ret;
	//int intcom;
	//float data = (float)com;
	//buf0[0] = '\n';

	//int num[4];
	//num[3] = com * 0.01;
	//num[2] = (com - num[3] * 100.0) * 0.1;
	//num[1] = (com - num[3] * 100.0 - num[2] * 10.0) * 1;
	//num[0] = (com - num[3] * 100.0 - num[2] * 10.0 - num[1]) * 10;

	int num[5];
	num[4] = com * 0.1;
	num[3] = (com - num[4] * 10.0) * 1;
	num[2] = (com - num[4] * 10.0 - num[3] * 1.0) * 10;
	num[1] = (com - num[4] * 10.0 - num[3] * 1.0 - num[2] * 0.1) * 100;
	num[0] = (com - num[4] * 10.0 - num[3] * 1.0 - num[2] * 0.1 - num[1] * 0.01) * 1000;




	//printf("%d,%d,%d,%d\n", num[4],num[3], num[2], num[1], num[0]);
	//if (cnt == 0)	buf0[0] = 's';//�J�n�̍��}�imbed�v���O����cnt=0�j
	//if (cnt == 1)	buf0[0] = num[0] + '0';//0.1�̈�
	//if (cnt == 2)	buf0[0] = num[1] + '0';//1�̈�
	//if (cnt == 3)	buf0[0] = num[2] + '0';//10�̈�
	//if (cnt == 4)	buf0[0] = num[3] + '0';//100�̈�

	if (cnt == 0)
	{
		buf0[0] = num[0] + '0';//0.001�̈� 
		//checknum[0] = ctoi(buf0[0]);
	}
	if (cnt == 1)
	{
		buf0[0] = num[1] + '0';//0.01�̈�
		//checknum[1] = ctoi(buf0[0]);
	}
	if (cnt == 2)
	{
		buf0[0] = num[2] + '0';//0.1�̈�
		//checknum[2] = ctoi(buf0[0]);
	}
	if (cnt == 3)
	{
		buf0[0] = num[3] + '0'; //1�̈�
		//checknum[3] = ctoi(buf0[0]);
		//printf("send data:%d,%d,%d,%d\n", num[3], num[2], num[1], num[0]);
	}
	if (cnt == 4)
	{
		buf0[0] = num[4] + '0'; //10�̈�
		//checknum[3] = ctoi(buf0[0]);
		//printf("send data:%d,%d,%d,%d,%d\n", num[4],num[3], num[2], num[1], num[0]);
	}

	//printf("%c", buf0[0]);

	Ret = WriteFile(h, buf0, 1, &dwSendSize, NULL);//1��������
	if (!Ret) {
		//���M�ł��Ȃ�������
		//�_���ł��I
		printf("SEND FAILED\n");
		CloseHandle(h);
		system("PAUSE");
		exit(0);
	}

	//printf("SEND OK\n");
}
void serial_close(HANDLE h)
{
	CloseHandle(h);		// �V���A���ʐM���I��
}
