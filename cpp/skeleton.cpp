
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
using namespace cv;
using namespace std;



int main( int argc, char** argv )
{
    Mat img, hsv , range, dst , range_1;
    img = imread( argv[1], IMREAD_COLOR );
    /******************threshold hsv ******************/
    cvtColor( img, hsv, COLOR_RGB2HSV);
    Scalar  hsv_min = Scalar( 90, 20, 60) , hsv_max = Scalar( 125, 150, 255);
    inRange( hsv, hsv_min, hsv_max, range );


    cv::Mat skel(img.size(), CV_8UC1, cv::Scalar(0));
    cv::Mat temp;
    cv::Mat eroded;
     
    cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
     
    bool done;      
    do
    {
      cv::erode(range, eroded, element);
      cv::dilate(eroded, temp, element); // temp = open(img)
      cv::subtract(range, temp, temp);
      cv::bitwise_or(skel, temp, skel);
      eroded.copyTo(range);
     
      done = (cv::countNonZero(range) == 0);
    } while (!done);
    imshow( "img", skel );