
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

const int max_hue = 180;
int hue_min, hue_max;
Mat src,dst,imgHSV;

void on_trackbar( int, void* )
{
	Mat imgThresholded, temp;
	inRange(imgHSV, Scalar(hue_min, 0, 0), Scalar(hue_max, 255, 255), imgThresholded);

	bitwise_or(imgHSV, imgHSV, temp, imgThresholded );
	cvtColor(temp, dst, COLOR_HSV2RGB);
	imshow( "filtered", dst );
}
int main(int argc , char** argv)
{
	Mat temp, temp1;
	src = imread(argv[1],IMREAD_COLOR);
	namedWindow("original",WINDOW_GUI_NORMAL);
	resizeWindow("original",480, 360);
	imshow("original", src);

	cvtColor(src, temp, COLOR_RGB2HSV);
	medianBlur(temp, imgHSV, 9);
	// cvtColor(temp1, imgHSV, COLOR_GRAY2RGB );
	// imshow("f",temp1);

	namedWindow("filtered",WINDOW_GUI_NORMAL);
	resizeWindow("filtered",720, 480);

	createTrackbar( "hue_max", "filtered", &hue_max, max_hue, on_trackbar );
	createTrackbar( "hue_min", "filtered", &hue_min, max_hue, on_trackbar );
	on_trackbar( hue_min, 0 );
	waitKey(0);
 	return 0;
}
