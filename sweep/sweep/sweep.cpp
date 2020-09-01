//2020/08/31 TomokiHirayama 更新
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
#define f_min 0.2
#define f_max 8.0

//サンプリングレート
#define Sampling_rate 60
//サンプリング数
#define Point 8192
//周波数の増分の定義
//周波数を1sあたりどれだけ増やすか
double delta_f = (f_max - f_min) / (Point - 1); //0.25;

//回転半径の定義[mm]
#define r 200.0  //要変更

//秒数の増分の定義[s]
//サーボの制御周波数と合わせる
//#define delta_t 201/60

//位置を表す構造体の設定
struct Pos {
	double x;//スイープ曲線上を移動する点のある時刻tにおけるx座標
	double y;//スイープ曲線上を移動する点のある時刻tにおけるy座標
};

int main(void)
{
	Pos pos;//一時保存する構造体
	FILE* fp;
	errno_t error_fp;
	const char* filename = "r200_f80_f02_P81newsweep.csv";//ファイルの保存名
	//printf("%f", delta_f / 60.0);
	//======fileを書き込みモードでオープン=======
	error_fp = fopen_s(&fp, filename, "w");
	if (fp==NULL)
	{
		printf("ファイルをOpen出来ません!\n");
		exit(0);
	}
	else
	{
		//===========最初は0.1Hzで10秒(=1周)回す==========
		for (int cnt = 0; cnt < int(1.0/f_min)*Sampling_rate*5; cnt++)
		{
			//(単位円の半径r)*(PI)*(時刻tにおける周波数f)*(時刻t)
			pos.x = r * cos(2.0 * PI * f_min * cnt / Sampling_rate);
			pos.y = r * sin(2.0 * PI * f_min * cnt / Sampling_rate);

			//======書き込み処理を行う=======
			fprintf(fp, "%lf,%lf\n", pos.x, pos.y);

		}
		//===========-sweep曲線を作成する========
		for (int sn = 0; sn < Point; sn++)
		{
			//(単位円の半径r)*(PI)*(時刻tにおける周波数f)*(時刻t)
			pos.x = r * cos(2.0 * PI * ( f_min + delta_f * sn ) * sn / Sampling_rate);
			pos.y = r * sin(2.0 * PI * ( f_min + delta_f * sn ) * sn / Sampling_rate);

			//======書き込み処理を行う=======
			fprintf(fp, "%lf,%lf\n", pos.x, pos.y);
			
		}
		printf("書き込み完了");
		fclose(fp);
	}
	return 0;
}