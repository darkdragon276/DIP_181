/**********OpenCV lib*********/
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
/**********C lib*************/
#include<iostream>
#include<math.h>
#include <string.h>

using namespace cv;
using namespace std;
Scalar_<double> circles = Scalar(255);
char* name_object[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", };

/**************************fucntion init******************/
void compare_obj( vector <vector <Point>> &objects, vector <vector <Point>> &object_templ , Mat &img);
void bound_obj(Mat &img, vector< vector< Point > > &objects , int thresh_winsize, int k_size);
static void draw_mass_obj( Mat& image, const vector <Point>& Shapes, Scalar_< double> &color , const String& nameshapes);


/* test */
// void test( Mat &img)
// {
//     int lowThreshold = 30;
//     int ratio = 50;
//     Mat temp;
//     Canny( img, temp, lowThreshold, lowThreshold*ratio, 5 );
//     blur( temp , temp, Size(4,4) );
//     imshow("test", temp);
// }
/************main************/

int main(int argc, char** argv)
{
    Mat img, templ;  
    img = imread( "data/bai6_1.png",IMREAD_GRAYSCALE );
    templ = imread( "data/bai6_2.png", IMREAD_GRAYSCALE );
    
    vector < vector <Point>> objects;
    vector < vector <Point>> object_templ;
    bound_obj( img, objects, 51, 3);
    bound_obj( templ, object_templ, 31, 6);

    compare_obj( objects, object_templ, img);
    imshow("img", img);
    for(;;)
    {
        if (waitKey(30) == 27)
        break; 
    }
    return 0;  
}
/***************************function*************************/
static void draw_mass_obj( Mat& image, const vector <Point>& Shapes, Scalar_< double> &color , const String& nameshapes)
{
    //draw mass of obj
    Moments mo_obj = moments( Shapes, false);
    Point p = Point( mo_obj.m10/mo_obj.m00, mo_obj.m01/mo_obj.m00 );
    int n = 1;
    const Point* a = &p;
    //putText(image, nameshapes, p, FONT_HERSHEY_COMPLEX_SMALL, 1, color, 1, LINE_AA);
    polylines(image, &a, &n, 1, true, color, 5, FILLED );

    //draw bounding box
    RotatedRect box = minAreaRect( Shapes );
    Point2f rec_point[4];
    box.points(rec_point);
    for (int i = 0; i < 4; i++)
    line(image, rec_point[i], rec_point[(i+1)%4], Scalar(0,255,0));
}

void bound_obj(Mat &img, vector< vector< Point > > &objects , int thresh_winsize , int k_size)
{
    // threshold adaptive
    Mat temp;
    adaptiveThreshold( img, temp, 255, ADAPTIVE_THRESH_GAUSSIAN_C  , THRESH_BINARY , thresh_winsize, 5);
    blur( temp , temp, Size( k_size, k_size ) );

    // fill defect
    // Mat element = getStructuringElement( MORPH_RECT , Size( 2*k_size + 1, 2*k_size+1 ), Point( k_size, k_size ) );
    // if( strcmp(mor_type,"templ") == 0)
    // {
    //     erode( temp, temp, element );
    // }
    // else if( strcmp(mor_type,"objects") == 0)
    // {
    //     dilate( temp, temp, element );
    //     erode( temp, temp, element );
    // }
    threshold(temp, temp, 190, 255, THRESH_BINARY);
    //find contour
    vector<vector<Point> > contours;
    findContours(temp, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    //filt some small contour
    vector<Point> approx;
    Scalar_<double> color = Scalar(255);
    for( size_t i = 0; i < contours.size(); i++ )
    {
        approxPolyDP( contours[i], approx, arcLength(contours[i], true)*0.01, true );
        if( fabs(contourArea(approx)) > 2500  && fabs(contourArea(approx)) < 20000 ) //&& isContourConvex(approx) ) 
        {
            objects.push_back(approx);
            //rectangle( dst, boundRect.tl(), boundRect.br(), color, 2, 8, 0 );
        }
        
    }
}

void compare_obj( vector <vector <Point>> &objects, vector <vector <Point>> &object_templ , Mat &img)
{
    /************compute humoment************/
    // Moments mu_templ;
    // vector<double> hu_templ;
    // mu_templ = moments( object_templ[0], false);
    // HuMoments( mu_templ, hu_templ );
    // cout << hu_templ[0] << endl;
    // vector<Moments> mu_object( objects.size() );
    // vector<vector<double>> hu_object( objects.size() );

    /************matshape use humoment******/
    vector<double> acurracy;
    for( int i = 0; i < objects.size(); i++ )
    { 
        // mu_object[i] = moments( objects[i], false);
        // HuMoments( mu_object[i], hu_object[i] );
        // cout << hu_object[i][0] << endl;
        acurracy.push_back( matchShapes( object_templ[0], objects[i], CONTOURS_MATCH_I1,  0) );
        cout << acurracy[i] << endl;
        //drawShapes(img, objects[i], circles, name_object[i]);
        if( acurracy[i] > 2.3 && acurracy[i] < 3.4 ) 
        {
            draw_mass_obj(img, objects[i], circles, name_object[i]);
        }   
    }    
}

