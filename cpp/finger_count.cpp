#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <algorithm>
using namespace cv;
using namespace std;

Mat img, hsv , range;
/*********************funciton for test*************************/
//int b = 1;
// void on_trackbar(int, void*)
// {
//     int erosion_type = MORPH_ELLIPSE;
//     int erosion_size = b;
//     Mat element = getStructuringElement( erosion_type, Size( 2*erosion_size + 1, 2*erosion_size+1 ), Point( erosion_size, erosion_size ) );
//     Mat dilate_el = getStructuringElement( MORPH_ELLIPSE, Size( 7, 7 ), Point( 3, 3 ) );
//     dilate( range, range_1, dilate_el );
//     erode( range_1, dst, element);
//     imshow( "test", dst );
// }

// static void drawShapes( Mat& image, const vector<vector<Point> >& Shapes, Scalar_< double> &color , const String& nameshapes , int i)
// {
//         const Point* p = &Shapes[i][0];
//         int n = (int)Shapes[i].size();
//         polylines(image, &p, &n, 1, true, color, 0.5, LINE_AA);
//         putText(image, nameshapes, p[0],FONT_HERSHEY_COMPLEX_SMALL, 1, color, 1, LINE_AA);
// }
/**********************main****************************/
int main( int argc, char** argv )
{
    
    img = imread( argv[1], IMREAD_COLOR );
    /******************threshold hsv ******************/
    cvtColor( img, hsv, COLOR_RGB2HSV);
    Scalar  hsv_min = Scalar( 90, 20, 60) , hsv_max = Scalar( 125, 150, 255);
    inRange( hsv, hsv_min, hsv_max, range );

    /******************findcontour********************/
    vector<vector<Point> > Shapes;
    vector<vector<Point> > contours;
    vector<Point> approx;
    findContours(range, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    Scalar_<double> blue = Scalar(255);
    for( size_t i = 0; i < contours.size(); i++ )
    {
        approxPolyDP(contours[i], approx, arcLength(contours[i], true)*0.004, true);
        if( fabs(contourArea(approx)) > 2000)//&& fabs(contourArea(approx)) < 525 )//&& isContourConvex(approx) ) 
        {
            Shapes.push_back(approx);   
        }
    }
    //drawShapes( img, Shapes, blue, "0" , 0);

    /*******************convex_hull******************/
    vector<int> hull;
    vector<Point> Points;
    vector<Vec4i> defect;
    //vector<int> fingers;
    for( size_t i = 0 ; i < Shapes.size() ; i++ )
    {
        hull.clear();
        Points.clear();
        defect.clear();
        // covex hull oriented clockwise
        convexHull( Shapes[i], hull, true );
        // output defect include [start_idx_contour , end_inx_contour, point_depthmax_contour , value_depthmax]
        convexityDefects( Shapes[i], hull, defect);

        /***************processing value of depth_defect**************/
        vector<int> depth_hull;
        for( size_t j = 0; j < defect.size(); j++)
        {
            depth_hull.push_back(defect[j][3]);  
        }
        int max = *max_element( depth_hull.begin(), depth_hull.end() );
        int finger = 1;
        for( size_t j = 0; j < depth_hull.size(); j ++)
        {
            if( (max > 17000) && ( depth_hull[j] > max*0.7 ) )
            {
                finger ++;
            }
        }
        if( max < 10000)
            finger = 0;
        //fingers.push_back(finger);
        // draw contuour and convex hull
        for( size_t j = 0; j < hull.size(); j++ )
        {
            Points.push_back( Shapes[i][hull[j]] );
        }
        int n = hull.size() ;
        const Point* p = &Points[0]; 
        polylines( img, &p, &n, 1, true, blue, 1, LINE_AA );
        // draw number of finger 
        const char* cir[6] = { "0", "1", "2", "3", "4", "5" };
        putText( img, cir[finger], Points[5],FONT_HERSHEY_COMPLEX_SMALL, 1, blue, 1, LINE_AA);
    }
    imshow( "max", img );
    
    // namedWindow( "max", WINDOW_AUTOSIZE );
    // createTrackbar( "src", "max", &b, 255, on_trackbar );
    // on_trackbar(0,0);
    waitKey(0);
    return 0;
}

