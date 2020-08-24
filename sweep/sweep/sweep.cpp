//2020/08/25　TomokiHirayama
//スイープ曲線を作成するプログラムです．
//計算結果を一時的に保存する配列がスタックサイズオーバーのため
//用意できなかったので，計算結果をそのまま書き込んでいます．


#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define PI 3.14159265358979

//周波数の定義[Hz]
//0.1Hz(=一周10秒ならアームは追いついているだろう)
// 10Hz(=一周0.1秒ならアームは追いつかないだろう)
#define f_min 0.1
#define f_max 10.0

//周波数の増分の定義
//周波数を1sあたりどれだけ増やすか
float delta_f = 0.25;

//回転半径の定義[mm]
#define r 20.0  //要変更

//秒数の増分の定義[s]
//サーボの制御周波数と合わせる
//#define delta_t 1/60

//位置を表す構造体の設定
struct Pos {
	float x;//スイープ曲線上を移動する点のある時刻tにおけるx座標
	float y;//スイープ曲線上を移動する点のある時刻tにおけるy座標
};

int main(void)
{
	Pos pos;//一時保存する構造体
	FILE* fp;
	errno_t error_fp;
	const char* filename = "sweep.csv";//ファイルの保存名
	printf("%f", delta_f / 60.0);
	//======fileを書き込みモードでオープン=======
	error_fp = fopen_s(&fp, filename, "w");
	if (fp==NULL)
	{
		printf("ファイルをOpen出来ません!\n");
		exit(0);
	}
	else
	{
		//===========-sweep曲線を作成する========
		//"2458"はサーボの制御周波数から計算(40.96s * 60[1/s]=2457.6個，2458個の点を取ることができる)
		for (float t = 0.0; t < 2458.0; t++)
		{
			//(単位円の半径r)*(PI)*(時刻tにおける周波数f)*(時刻t)
			pos.x = r * cos(2.0 * PI * (f_min + delta_f / 60.0 * t) * t/60.0);
			pos.y = r * sin(2.0 * PI * (f_min + delta_f / 60.0 * t) * t/60.0);

			//======書き込み処理を行う=======
			fprintf(fp, "%lf,%lf\n", pos.x, pos.y);
			
		}
		printf("書き込み完了");
		fclose(fp);
	}
	return 0;
}