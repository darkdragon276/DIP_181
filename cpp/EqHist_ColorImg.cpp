
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream> 
#include <stdio.h>
using namespace cv;
using namespace std;
/** @function main */
int main( int argc, char** argv )
{ 
	Mat img, src, dst;
	const char* source_window = "Source image";
	const char* equalized_window = "Equalized Image";
	/// Load image
	img = imread( argv[1], IMREAD_COLOR );
	cvtColor( img, src, COLOR_RGB2HSV);

	Mat hsv[3], v, v_equal;
	split(src,hsv);
	v = hsv[2];
	equalizeHist( v, v_equal );
	vector<Mat> ch;
	ch.push_back(hsv[0]);
	ch.push_back(hsv[1]);
	ch.push_back(v_equal);
	merge(ch,dst);
	cvtColor(dst, dst, COLOR_HSV2RGB);
	/// Display results
	namedWindow( source_window, WINDOW_AUTOSIZE );
	namedWindow( equalized_window, WINDOW_AUTOSIZE );
	imshow( source_window, img );
	imshow( equalized_window, dst );
	/// Wait until user exits the program
	waitKey(0);
	return 0; 
}
