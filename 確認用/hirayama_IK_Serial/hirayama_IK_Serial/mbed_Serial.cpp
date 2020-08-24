#include<stdio.h>
#include<iostream>
#include<tchar.h>
#include<stdlib.h>
#include<windows.h>
#include<conio.h>
#include<windows.h>

#include"mbed_Serial.h"


#define PI 3.14159265358979
HANDLE h;//HANDLE型、ポインタです。(void* )
DCB dcb; //シリアル通信デバイスの制御定義を表す
COMMTIMEOUTS cto;//通信デバイスのタイムアウトパラメータ

//COMPORT *COM;

DWORD nn0, nn1;//32bit符号なし整数、範囲は0~18446744073709551615 10進数
unsigned char buf0[1];
unsigned char buf1[1];
double com;
double rep;

int checknum[4];

/*int ctoi(char c)
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
}*/


HANDLE serial_open()
{
	//HANDLE h = CreateFile(_T("com3"), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE h = CreateFile(_T("COM3"), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	//ポート番号はPCの設定と相談しましょう

	if (h == INVALID_HANDLE_VALUE) {
		//開けなかった場合には
		//開けられへんかったぞ！
		printf("PORT COULD NOT OPEN\n");
		//プログラムの一時停止
		system("PAUSE");
		//プログラムの正常終了
		exit(0);
	}
	printf("PORT COULD OPEN\n");
	return h;
}
HANDLE serial_initialaize(HANDLE h)
{
	BOOL Ret;
	//SetUpComm(HANDLE h, DWORD dwInQueue , DWORD dwOutQueue)
	//h:通信デバイスのハンドル
	//dwInQueue :デバイスの内部入力バッファーの推奨サイズ
	//dwOutQueue:デバイスの内部出力バッファーの推奨サイズ
	Ret = SetupComm(h, 1024, 1024);
	if (!Ret) {
		//設定
		//失敗したよ！
		printf("SET UP FAILED\n");
		//CloseHandle(HANDLE h)
		CloseHandle(h);
		system("PAUSE");
		exit(0);
	}
	printf("SET UP SUCCESSED\n");
	//初期化する
	//PurgeComm(HANDLE h,DWORD Flags)
	//指定されたポートの入力バッファにあるすべての文字を破棄する。
	Ret = PurgeComm(h, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!Ret) {
		//初期化失敗したら
		//失敗したで！
		printf("CLEAR FAILED\n");
		CloseHandle(h);
		exit(0);
	}

	return h;
}				
HANDLE serial_Config(HANDLE h)
{
	//3.基本通信条件の設定
	DCB dcb;
	BOOL Ret;
	//GetCommState(HANDLE h ,LPDCB lpdcb)
	//    h:通信デバイスへのハンドル
	//lpdcb:制御設定情報を受け取るDCB構造へのポインタ
	GetCommState(h, &dcb);
	//dcb.DCBlength = sizeof(DCB);//length  :DCB構造体の大きさ(バイト数)
	dcb.BaudRate = 115200;        //BaudRate:通信デバイスのボーレート
	//dcb.fBinary = TRUE;         //fBinary :バイナリモードの可否
	dcb.ByteSize = 8;           //ByteSize:送受信するバイトデータのビット数
	dcb.fParity = NOPARITY;     //fParity :パリティチェックの可否
	dcb.StopBits = ONESTOPBIT;  //StopBits:使用するストップビット数の指定、1ストップビット
	//SetCommState(HANDLE h ,LPDCB lpdcb)
	cto.ReadIntervalTimeout = 0;
	cto.ReadTotalTimeoutMultiplier = 0;
	cto.ReadTotalTimeoutConstant = 0;
	cto.WriteTotalTimeoutMultiplier = 0;
	cto.WriteTotalTimeoutConstant = 0;
	Ret = SetCommState(h, &dcb);
	SetCommTimeouts(h, &cto);
	if (!Ret) {
		//設定できなかったら
		//失敗したで！
		printf("SetCommState FAILED\n");
		CloseHandle(h);
		system("PAUSE");
		exit(0);
	}
	printf("SetCommState SUCCESSED\n");
	return h;
}
void serial_Write(HANDLE h,double com,int cnt)//const char* data)
{
	//h:シリアル通信の設定のポインタ
	//com:送りたいデータ
	//cnt:カウンタ
	DWORD dwSendSize;
	DWORD dwErrorMask;
	BOOL Ret;
	//int intcom;

	
	/*int num[4];
	num[3] = com * 0.01;
	num[2] = (com - num[3] * 100.0) * 0.1;
	num[1] = (com - num[3] * 100.0 - num[2] * 10.0) * 1;
	num[0] = (com - num[3] * 100.0 - num[2] * 10.0 - num[1]) * 10;

	


	//printf("%d,%d,%d,%d\n", num[3], num[2], num[1], num[0]);
	//if (cnt == 0)	buf0[0] = 's';//開始の合図（mbedプログラムcnt=0）
	//if (cnt == 1)	buf0[0] = num[0] + '0';//0.1の位
	//if (cnt == 2)	buf0[0] = num[1] + '0';//1の位
	//if (cnt == 3)	buf0[0] = num[2] + '0';//10の位
	//if (cnt == 4)	buf0[0] = num[3] + '0';//100の位

	if (cnt == 0) 
	{ 
		buf0[0] = num[0] + '0';//0.1の位 
		checknum[0] = ctoi(buf0[0]);
	}
	if (cnt == 1) 
	{ 
		buf0[0] = num[1] + '0';//1の位
		checknum[1] = ctoi(buf0[0]);
	}	
	if (cnt == 2) 
	{ 
		buf0[0] = num[2] + '0';//10の位
		checknum[2] = ctoi(buf0[0]);
	}	
	if (cnt == 3)
	{
		buf0[0] = num[3] + '0'; //100の位
		//checknum[3] = ctoi(buf0[0]);
		//printf("send data:%d,%d,%d,%d\n", num[3], num[2], num[1], num[0]);
	}


	//printf("%c", buf0[0]);
	//Ret = WriteFile(h, &data, sizeof(data), &dwSendSize, NULL);
	Ret = WriteFile(h, buf0, 1, &dwSendSize, NULL);//1文字送る
	if (!Ret) {
		//送信できなかったら
		//ダメです！
		printf("SEND FAILED\n");
		CloseHandle(h);
		system("PAUSE");
		exit(0);
	}
	if (cnt == 3)
	{
		float mbedOut = 1000 * checknum[3] + 100 * checknum[2] + 10 * checknum[1] + 1 * checknum[0];
		//printf("get data  %4.f\n",mbedOut);

	}*/
	float data = (float)com
	Ret = WriteFile(h, &data, sizeof(data), &dwSendSize, NULL);
	
	if (!Ret) {
		//送信できなかったら
		//ダメです！
		printf("SEND FAILED\n");
		CloseHandle(h);
		system("PAUSE");
		exit(0);
	}



	//printf("SEND OK\n");
}
void serial_close(HANDLE h)
{
	CloseHandle(h);		// シリアル通信を終了
}
