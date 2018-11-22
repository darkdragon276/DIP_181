/**********OpenCV lib*********/
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
/**********C lib*************/
#include<iostream>
#include<math.h>


using namespace cv;
using namespace std;



/************main************/

void thresh_localotsu(Mat &img, Mat &dst, int row, int col)
{
    Mat temp;
    GaussianBlur( img, temp, Size( 3, 3 ), 0, 0 );

    Mat img_thresh;
    int wid = img.size().width;
    int hei = img.size().height;
    int win_wid = wid/col;
    int win_hei = hei/row;
    for( size_t i = 0 ; i < row ; i ++)
    {
        for( size_t j = 0 ; j < col ; j++)
        {
            int x = j*win_wid;
            int y = i*win_hei;
            Rect rect = Rect( x, y, win_wid, win_hei );
            Mat img_clone = Mat( temp, rect );
            //threshold otsu
            threshold( img_clone, img_thresh, 128, 255, THRESH_BINARY | THRESH_OTSU);
            img_thresh.copyTo( dst( rect ) );
        }
    }
}
bool compareContourAreas ( vector<Point> contour1, vector<Point> contour2 ) 
{
    double i = fabs( contourArea(cv::Mat(contour1)) );
    double j = fabs( contourArea(cv::Mat(contour2)) );
    return ( i > j );
}
void bounding_rect(Mat &img, Mat& dst)
{
    Mat temp;
    adaptiveThreshold( img, temp, 255, ADAPTIVE_THRESH_MEAN_C , THRESH_BINARY , 51, 0);
    blur( temp , temp, Size(3,3) );

    vector<vector<Point> > contours;
    findContours(temp, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    sort(contours.begin(), contours.end(), compareContourAreas);

    vector<Point> approx;
    Rect boundRect;
    Scalar_<double> color = Scalar(255);
    for( size_t i = 0; i < contours.size(); i++ )
    {
        approxPolyDP( contours[i], approx, arcLength(contours[i], true)*0.01, true );
        if( fabs(contourArea(approx)) > 2500  && fabs(contourArea(approx)) < 10000 ) //&& isContourConvex(approx) ) 
        {
            boundRect = boundingRect( approx );
            Mat img_thresh;
            Mat img_clone = Mat( img, boundRect );
            //threshold otsu
            threshold( img_clone, img_thresh, 128, 255, THRESH_OTSU );
            cout << boundRect << endl ;
            img_thresh.copyTo( dst( boundRect ) );
            //rectangle( dst, boundRect.tl(), boundRect.br(), color, 2, 8, 0 );
        }
        
    }
}

int main(int argc, char** argv)
{
    Mat img, templ;  
    img = imread( "data/bai6_1.png",IMREAD_GRAYSCALE );
    templ = imread( "data/bai6_2.png", IMREAD_GRAYSCALE );
    Mat temp1 = Mat( img.size(), CV_8U );
    Mat temp2 = Mat( img.size(), CV_8U );
    
    thresh_localotsu( img, temp1, 2, 3 );
    bounding_rect( img, temp2 );
    imshow("img", temp2);
    imshow("src", temp1);
    // createTrackbar( "max", "src", &b, 255, on_trackbar );
    // on_trackbar(0,0);
    waitKey(0);
    return 0;  
}
