#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;


Mat test(Size(576,482), CV_8U, Scalar(0));
Scalar_<double> circles = Scalar(255);
const string numberTab[] = { "0" , "1" , "2" , "3" , "4" , "5" , "6" };
int cluster = 11 ;  
// int b = 1;
// static void drawPoints( Mat& image, const vector<Point2f >& Points, Scalar_< double> &color , const String& nameshapes);
// void on_trackbar(int, void*);
/*****************************main********************************/
int main(int argc, char** argv)
{
    Mat img, thresh;

    img = imread("data/bt4.png",IMREAD_GRAYSCALE);
    threshold(img, thresh, 115, 255, THRESH_BINARY);
    imshow("image",img);
    /********************************find dot dice with findcontour*********************/
    vector<vector<Point> > contours;
    vector<Point> approx;
    vector<Point2f> points, centers;

    findContours(thresh, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    for( size_t i = 0; i < contours.size(); i++ )
    {
        approxPolyDP(contours[i], approx, arcLength(contours[i], true)*0.01, true);
        if( fabs(contourArea(approx)) > 110 && fabs(contourArea(approx)) < 525 )//&& isContourConvex(approx) ) 
        {
            Point point;
            Point2f point_f;
            for(size_t j = 0; j < approx.size(); j ++)
            {
                point += approx[j];
            }
            point_f = Point2f(point);
            point_f.x /= approx.size();
            point_f.y /= approx.size();
            points.push_back(point_f); 
        }
    }
    /*******************************kmeans - divide into cluster*************************************/
    Mat labels, count;
    kmeans(points, cluster, labels, TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 11, 1), 5, KMEANS_PP_CENTERS, centers);
    //drawPoints(test, centers, circles, "circle");
    count = Mat::zeros(1,cluster,CV_16SC2);
    for(size_t k = 0 ; k < points.size(); k ++)
    {
        count.at<int>(labels.at<int>(k)) ++;
    }
    cout << count << endl;
    for(size_t h = 0; h < centers.size(); h ++)
    {
        putText(img, numberTab[count.at<int>(h)], centers[h], FONT_HERSHEY_COMPLEX_SMALL, 1, circles, 1, LINE_AA);
    }
    // createTrackbar( "max", "src", &b, 30, on_trackbar );
    // on_trackbar(0,0);
    imshow("dot",img);
    waitKey(0);
    return 0;
}
void on_trackbar(int, void*)
{
    test = Scalar::all(0); 
}

static void drawPoints( Mat& image, const vector<Point2f >& Points, Scalar_< double> &color , const String& nameshapes)
{
    int n = Points.size();
    Point p;
    for(size_t i = 0 ; i < Points.size() ; i ++)
    {
        p = Points[i];
        circle( image, p, 2, color, FILLED, LINE_AA );
    }  
    imshow("dot",test);
}