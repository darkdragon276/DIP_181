#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <algorithm>
#include <iostream>
#include <vector>
#include <math.h>
#include <cstdint>
#include <cstdbool>

using namespace cv;
using namespace std;


/*define*/
//find_contour_approx
#define max_area_contour        10000
#define min_area_contour        30//0
//
#define wid_win_detect_mass     160//100
#define deltax_detect_mass      0//100


// if define otsu to use otsu , none to use adaptivethreshold
#define ostu                    1
#define inverse_thresh_otsu     1
//
#define row_otsu_window         1
#define col_otsu_window         3
#define thresh_bi_ostu          140//155//130
//else
#define ksize_thresh_adaptive   101
#define mean_thresh_adaptive    5   

// PID_Controller
#define PID_input_std           1
#define PID_output_std          0

// update_dinamic_win
#define delta_x_pixel			20

/*varaiable*/

/*function*/

void mass_lane( Mat &src, int index_frame, Mat &dst, Rect& current_rect );
void pre_processing( Mat& frame, vector<vector<Point>>& contours );

void find_contour_approx( Mat &img , vector<vector<Point>>& lane_contour);
void thresh_localotsu( Mat &frame, Mat &dst, int row, int col );
void hough( Mat &img , Mat&dst);

void drawContour( Mat& image, const vector<Point>& Contour );
void drawContour( Mat& image, const vector<vector<Point> >& Contour );
bool point_in_rect( Point src, int wid, int hei, Point p);
void get_elements( vector<Point>& vec_point, vector<int>& x, vector<int>& y );

void update_dynamic_win( vector<vector<Point>> pre_mid_lane ,  Rect &rect  );
void get_mid_lane( vector<vector<Point>>& contour, Rect rect, vector<vector<Point>>& mid_lane );


void standardized( vector<double> in, vector<double> &out, double max, double min, bool in_out);
void PID_Controller( vector<double> error, vector<double> &output, vector<double> K, double max_value, double min_value, double Ts );
// vector<Point> mass_Points;
//         Moments mo_obj = moments( approx, false);
    //         Point mass = Point( mo_obj.m10/mo_obj.m00, mo_obj.m01/mo_obj.m00 );
    //         if( mass.x < ( wid/2 - deltax_detect_mass) && mass.x > ( wid/2 - deltax_detect_mass - wid_win_detect_mass ) || 
    //             mass.x > ( wid/2 + deltax_detect_mass) && mass.x < ( wid/2 + deltax_detect_mass + wid_win_detect_mass ) )
    //         {
    //             mass_Points.push_back(mass);
    //             //circle( temp, mass, 3, Scalar(0), 4);
    //         }   
// Rect win_filter = Rect( Point(), Point() );
            // for( size_t j = 0; j < approx.size(); j++ )
            // {
            //     if( point_in_rect( Point(0,0) , wid_rect, hei, approx.at(j) ))
            //     {  
            //         drawContour( temp2, approx );
            //         inrect = true;
            //     }
            //     if( point_in_rect( Point( wid - wid_rect ,0 ), wid_rect, hei, approx.at(j) ) )
            //     {
            //         drawContour( temp1, approx );
            //         inrect = true;
            //     }
            // }
            // if( !inrect )
            // {
            //     drawContour( dst, approx );
            // }
// if( mass_Points.size() == 2 )
    // {
    //     Point middle = ( mass_Points[0] + mass_Points[1] )/2;
    //     //circle( temp, middle, 3, Scalar(255), 4);
    // }
    // Point focus_point = Point( int( wid/2 ), int( hei/2 ) );
    //circle( temp, focus_point, 3, Scalar(255), 2);