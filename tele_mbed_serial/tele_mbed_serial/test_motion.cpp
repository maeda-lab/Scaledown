#include<stdio.h>
#include<stdlib.h>
#include <conio.h>
#include<math.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define PI 3.14159265358979

cv::Mat img = cv::Mat::zeros(1080,1920 , CV_8UC3);


int nx = 5;//17;
double px = 2.0;//1.25;
double py = 2.0;//1.25;
double f0x = 0.3326;//(Hz)
double f0y = 0.3842;
double a0x = 500.0;
double a0y = 250.0;
double fix[3][5] = { { 0.32, 1.29, 1.50, 0.43, 0.60 },
					 { 0.04, 0.51, 0.24, 0.52, 2.00 },
					 { 0.38, 1.22, 1.95, 0.00, 1.66 }
};
double fiy[3][5] = { { 0.41, 0.44, 0.88, 1.71, 1.43 },
					 { 1.32, 1.28, 1.38, 0.40, 0.91 },
					 { 1.29, 1.54, 1.77, 0.86, 1.13 }

};
//実験用のプログラム．モニター内で点が動く関数
void test(int set)
{
	printf("実験%dです.sを押すとスタートします．\n", set);
	char ch = _getch();
	if (ch == 's')
	{
		int t = 0;
		while (1) {
			img = cv::Mat::zeros(1080, 1920, CV_8UC3);
			double x = 1000.0;
			double y = 500.0;

			for (int i = 0; i < nx; i++)
			{
				x = x + a0x * pow(px, -i) * sin(2 * PI * f0x * pow(px, i) * t * 0.01 + fix[set][i] * PI);
				y = y + a0y * pow(py, -i) * sin(2 * PI * f0y * pow(py, i) * t * 0.01 + fiy[set][i] * PI);
			}

			cv::circle(img, cv::Point(x, y), 20, cv::Scalar(200, 0, 0), -1, CV_AA);
			cv::imshow("drawing", img);
			cv::waitKey(10);
			if (_kbhit() != 0 && _getch() == 0x1b)
			{
				break;
			}
			t++;
		}
	}


}
int main(void)
{
	//cv::namedWindow("drawing", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	//cv::namedWindow("drawing", CV_WINDOW_FULLSCREEN | CV_WINDOW_FREERATIO);
	//
	printf("それでは実験を始めます\n");
	test(0);
	test(1);
	test(2);
	printf("終了します\n");
	return 0;
}