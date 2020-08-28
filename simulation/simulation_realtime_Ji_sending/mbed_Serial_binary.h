// Aug 25, 2020
// Masahiro Furukawa 
// 
// Serial Binary Communication
//
// reference : mbed_Serial.h written by Tomoki Hirayama
// reference : https://stackoverflow.com/questions/14018894/how-to-convert-float-to-byte-array-of-length-4-array-of-char

#include<stdio.h>
#include<iostream>
#include<tchar.h>
#include<stdlib.h>
#include<windows.h>
#include<conio.h>


#define PI 3.14159265358979

class Com_Binary
{
private:
	HANDLE h;			// HANDLE型、ポインタです。(void* )
	DCB dcb;			// シリアル通信デバイスの制御定義を表す
	COMMTIMEOUTS cto;	// 通信デバイスのタイムアウトパラメータ

	double com;

	int checknum[4];
	int size_of_float{ sizeof(float) };

public:
	Com_Binary()
	{
		h = CreateFile(_T("COM6"), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (h == INVALID_HANDLE_VALUE) {
			printf("PORT COULD NOT OPEN\n");
			system("PAUSE");
			exit(0);
		}
		printf("PORT OPEN SUCCESSED\n");

		serial_initialaize();
		serial_Config();
	}

	// シリアル通信を終了
	~Com_Binary()
	{
		CloseHandle(h);
	}

	void float2byte(unsigned char ret[], float f) {
		// 
		// reference : https://stackoverflow.com/questions/14018894/how-to-convert-float-to-byte-array-of-length-4-array-of-char

		unsigned char const* p = reinterpret_cast<unsigned char const*>(&f);

		memcpy(ret, p, size_of_float);

		//for (std::size_t i = 0; i != size_of_float; ++i)
		//{
		//	std::printf("The byte #%zu is 0x%02X\n", i, ret[i]);
		//}
	}
	void byte2float(float* f, unsigned char ret[]) {
		// 

		float const* p = reinterpret_cast<float const*>(ret);

		memcpy(f, p, size_of_float);

		//std::printf("The value is %3.3lf\n", *f);
	}
	void send_one_float(double d) { send_one_float((float)d); } // overload
	void send_one_float(float f)
	{
		BOOL Ret;

		unsigned char sbuf[4];

		float2byte(sbuf, f);

		//// test
		//for (std::size_t i = 0; i != size_of_float; ++i)
		//{
		//	std::printf("The byte #%zu is 0x%02X *\n", i, sbuf[i]);
		//}

		Ret = WriteFile(h, sbuf, size_of_float, NULL, NULL); // 4 文字送る

		if (!Ret) {
			printf("SEND FAILED\n");
			CloseHandle(h);
			system("PAUSE");
			exit(0);
		}
		// sendBytes is required !! 
	}void send_bytes(unsigned char sbuf[], int sendBytes)
	{
		BOOL Ret;
		DWORD writtenBytes;

		Ret = WriteFile(h, sbuf, sendBytes, &writtenBytes, NULL);

		// printf("written %d in %d bytes\n", writtenBytes, sendBytes);

		if (!Ret) {
			printf("SEND FAILED\n");
			CloseHandle(h);
			system("PAUSE");
			exit(0);
		}
	}
private:

	void serial_initialaize()
	{
		BOOL Ret;
		//SetUpComm(HANDLE h, DWORD dwInQueue , DWORD dwOutQueue)
		//h:通信デバイスのハンドル
		//dwInQueue :デバイスの内部入力バッファーの推奨サイズ
		//dwOutQueue:デバイスの内部出力バッファーの推奨サイズ
		Ret = SetupComm(h, 1024, 1024);
		if (!Ret) {
			printf("SET UP FAILED\n");
			CloseHandle(h);
			exit(0);
		}
		printf("SET UP SUCCESSED\n");
		Ret = PurgeComm(h, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
		if (!Ret) {
			printf("CLEAR FAILED\n");
			CloseHandle(h);
			exit(0);
		}
	}

	void serial_Config()
	{
		//3.基本通信条件の設定
		DCB dcb;
		BOOL Ret;
		GetCommState(h, &dcb);
		dcb.DCBlength = sizeof(DCB);//length  :DCB構造体の大きさ(バイト数)
		dcb.BaudRate = 921600;      //BaudRate:通信デバイスのボーレート
		dcb.fBinary = TRUE;         //fBinary :バイナリモードの可否
		dcb.ByteSize = 8;           //ByteSize:送受信するバイトデータのビット数
		dcb.fParity = NOPARITY;     //fParity :パリティチェックの可否
		dcb.StopBits = ONESTOPBIT;  //StopBits:使用するストップビット数の指定、1ストップビット

		Ret = SetCommState(h, &dcb);
		if (!Ret) {
			printf("SetCommState FAILED\n");
			CloseHandle(h);
			system("PAUSE");
			exit(0);
		}

		cto.ReadIntervalTimeout = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
		cto.ReadTotalTimeoutConstant = 0;
		cto.WriteTotalTimeoutMultiplier = 1;
		cto.WriteTotalTimeoutConstant = 10;

		SetCommTimeouts(h, &cto);

		printf("SetCommState SUCCESSED\n");
	}
};