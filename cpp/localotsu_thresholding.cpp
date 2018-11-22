/**********OpenCV lib*********/
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
/**********C lib*************/
#include<iostream>
#include<math.h>


using namespace cv;
using namespace std;


Mat img, templ; 
Mat temp1, temp2 ;
/************main************/

void thresh_localotsu(Mat &img, Mat &dst, int row, int col)
{
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
            Rect rect = Rect( x, y, win_wid, win_hei);
            Mat img_clone = Mat(img,rect);
            //threshold otsu
            threshold(img_clone, img_thresh, 150, 255, THRESH_OTSU);
            img_thresh.copyTo( dst( Rect( x, y, win_wid, win_hei ) ) );
        }
    }
}
int main(int argc, char** argv)
{
    
    img = imread( "data/bai6_1.png",IMREAD_GRAYSCALE );
    templ = imread( "data/bai6_2.png", IMREAD_GRAYSCALE );
    Mat dst = Mat( img.size().height, img.size().width, CV_8U);
    cout << img.size() << dst.size() << endl ;

    int i = 3;
    GaussianBlur( img, temp1, Size( i, i ), 0, 0 );
    thresh_localotsu( temp1, dst, 2, 3);

    imshow("img", img);
    imshow("src", dst);
    // createTrackbar( "max", "src", &b, 255, on_trackbar );
    // on_trackbar(0,0);
    waitKey(0);
    return 0;  
}
