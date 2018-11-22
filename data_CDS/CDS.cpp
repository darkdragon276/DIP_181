#include "build.hpp"

void test_img(void)
{
    Mat img = imread( "V01.png", IMREAD_COLOR);
    Mat temp;
    uint32_t i = 0; 
    Rect win;

    mass_lane( img, i, temp, win );

    namedWindow( "dst", WINDOW_AUTOSIZE );
    imshow( "img", img );
    imshow( "dst", temp );
    waitKey(0);
}

void test_vid(void)
{
    VideoCapture video( "data_CDS/live_1.avi" );   //0 is the id of video device.0 if you have only one camera.
    if ( !video.isOpened() ) 
    { //check if video device has been initialised
        cout << "cannot open camera";
        return ;
    }
    uint32_t i = 0; 
    Rect win;
    Mat src_frame, temp;
    for(;;)
    {
        video.read( src_frame );
        if( ++i == video.get( CAP_PROP_FRAME_COUNT ) )
        {
            i = 0;
            video.set( CAP_PROP_POS_FRAMES, 0 );
        }

        mass_lane( src_frame, i, temp, win );
        rectangle( temp, win, Scalar(0, 255, 0) );
        namedWindow( "temp", WINDOW_AUTOSIZE );
        imshow( "src", src_frame );
        imshow( "temp", temp ) ;
        if ( waitKey(30) >= 0 )
        {
            imwrite( "V01.png", src_frame );
            break;
        } 
    }
}
/**********************************main*******************************///////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    
    //test_img();
    test_vid();
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mass_lane( Mat &src, int index_frame, Mat &dst, Rect& current_rect )
{
    Mat frame, pre_frame;
    cvtColor( src, frame, COLOR_BGR2GRAY );
    /*get roi of frame*/
    int x = 0;
    int y = 2*frame.size().height/3;
    int wid = frame.size().width;
    int hei = frame.size().height/3;
    Rect rect = Rect( x, y, wid, hei );
    Mat Roi = Mat( frame, rect );
    /*threshold + findcontour*/
    vector<vector<Point>> contours;
    pre_processing( Roi, contours );
    Mat temp_color = Mat::zeros( Roi.size(), CV_8UC3 );
  
    /*get mid lane*/
    vector<vector<Point>> midlane;
    if( index_frame <= 20 )
    {
        current_rect = Rect( Point( wid/2 - 100, 2 ), Point( wid/2 + 100, hei ) );
        get_mid_lane( contours, current_rect, midlane );
        update_dynamic_win( midlane, current_rect );
    }
    else
    {
        if( contours.size() != 0)
        {
            get_mid_lane( contours, current_rect, midlane );
            cout << midlane.size() << "  " << contours.size() << endl;
            if ( midlane.size() >= 1 && midlane.size() <= 4)
            {
                update_dynamic_win( midlane, current_rect );
            }
            else if( midlane.size() > 4 )
            {

            }
            else  
                current_rect = Rect( Point( 30, 2 ), Point( wid - 30, hei - 2 ) ); 
        }

    }
    drawContour( temp_color, midlane );
    temp_color.copyTo(dst);  
}
//
void pre_processing( Mat& frame, vector<vector<Point>>& contours )
{
    Mat temp = Mat::zeros( frame.size(), CV_8U );
    #if defined(ostu)
    thresh_localotsu( frame, temp, row_otsu_window, col_otsu_window );
    blur( temp , temp, Size(5,5) );
    threshold( temp, temp, 80, 255, THRESH_BINARY);
    #else
    // adaptive threshold
    GaussianBlur( Roi, temp, Size( 3, 3), 0, 0 );
    adaptiveThreshold( temp, temp, 255, ADAPTIVE_THRESH_GAUSSIAN_C  , THRESH_BINARY , ksize_thresh_adaptive, mean_thresh_adaptive);
    blur( temp , temp, Size(5,5) );
    threshold( temp, temp, 200, 255, THRESH_BINARY );
    #endif
    find_contour_approx( temp, contours );
}
//
void find_contour_approx( Mat &img , vector<vector<Point>>& lane_contour)
{
    /*find contour*/
    vector<vector<Point>> contours;
    vector< Vec4i > hierarchy;
    findContours( img, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    vector<Point> approx;;
    for( size_t i = 0; i < contours.size(); i++ )
    {
        approxPolyDP( contours[i], approx, arcLength(contours[i], true)*0.001, true );
    
        if( fabs(contourArea(approx)) > min_area_contour  && fabs(contourArea(approx)) < max_area_contour )//&& ( hierarchy[i][2] >= 0 ) )
        {
            if( approx.size() > 10)
            {
                lane_contour.push_back( approx );
            }
        }
    }    
}
// function get thresh by otsu with dividing frame into row x col part
void thresh_localotsu( Mat &frame, Mat &dst, int row, int col )
{
    Mat temp;
    GaussianBlur( frame, temp, Size( 3, 3 ), 0, 0 );

    Mat frame_thresh;
    int wid = frame.size().width;
    int hei = frame.size().height;
    int win_wid = wid/col;
    int win_hei = hei/row;
    for( size_t i = 0 ; i < row ; i ++)
    {
        for( size_t j = 0 ; j < col ; j++)
        {
            int x = j*win_wid;
            int y = i*win_hei;
            Rect rect;
            if( j == col - 1 )
            {
                rect = Rect( x, y, wid - x, win_hei );
            }
            else if( i == row - 1 )
            {
                rect = Rect( x, y, win_wid, hei - y );
            }
            else
            {
                rect = Rect( x, y, win_wid, win_hei );
            }
            Mat frame_clone = Mat( temp, rect );
            //threshold otsu
            threshold( frame_clone, frame_thresh, thresh_bi_ostu, 255, THRESH_OTSU || THRESH_BINARY);
            #if defined(inverse_thresh_otsu)
            bitwise_not( frame_thresh, frame_thresh );
            #endif
            frame_thresh.copyTo( dst( rect ) );
        }
    }
}
// HoughLine detect
void hough( Mat &img , Mat&dst)
{
    vector<Vec2f> lines;
    Mat temp;
    cvtColor( img, temp, COLOR_BGR2GRAY );
    //Mat temp = img.clone();
    HoughLines( temp, lines, 1, CV_PI/180, 35, 0, 0 ); // runs the actual detection
    // Draw the lines
    for( size_t i = 0; i < lines.size(); i++ )
    {
        // distance from Point0 to Point1 or Point2
        int distance = 1000;
        float rho = lines[i][0], theta = lines[i][1];
        if( theta < 70.0*CV_PI/180.0 || theta > 100.0*CV_PI/180.0 )
        {
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            // u (a , b) is normal vector => v( -b, a ) is direction vector
            pt1.x = cvRound(x0 + distance*(-b));
            pt1.y = cvRound(y0 + distance*(a));
            pt2.x = cvRound(x0 - distance*(-b));
            pt2.y = cvRound(y0 - distance*(a));
            line( dst, pt1, pt2, Scalar(120,0,120), 1, LINE_AA);
        }
    }
}
//draw contour
void drawContour( Mat& image, const vector<Point>& Contour )
{
    const Point* p = &Contour[0];
    int n = (int)Contour.size();
    polylines(image, &p, &n, 1, true, Scalar(0,255,0), 1, LINE_AA);
}
void drawContour( Mat& image, const vector<vector<Point> >& Contour )
{
    for( size_t i = 0; i < Contour.size(); i++ )
    {
        const Point* p = &Contour[i][0];
        int n = (int)Contour[i].size();
        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 1, LINE_AA);
    }
}

// PID Controller
void PID_Controller( vector<double> error, vector<double> &output, vector<double> K, double max_value, double min_value, double Ts )
{
    vector<double> error_st;
    standardized( error, error_st, max_value, min_value, PID_input_std);
    //PID algorithm
    double P = K.at(0) ;
    double I = K.at(1)*Ts/2;
    double D = K.at(2)/Ts;
    double a = P + I + D;
    double b = -P + I - 2*D;
    //output
    output.at(0) = output.at(1);
    output.at(1) = output.at(0) + a*error_st[2] + b*error_st[1] + D*error_st[0];   
}
//standardized to [0:100]
void standardized( vector<double> in, vector<double> &out, double max, double min, bool in_out)
{
    if( in_out )
    {
        for( size_t i = 0; i < in.size(); i++ )
        {
            double value = max - min;
            value = (in.at(i) - min)/value;
            value *= 100;
            out.push_back( value );
        }
    }
    else
    {
        for( size_t i = 0; i < in.size(); i++ )
        {
            double value = in.at(i)/100;
            value *= (max - min);
            value += min;
            out.push_back( value );
        }
    }
}
//point in rect ?
bool point_in_rect( Rect rect_win, Point p)
{
    int x_min = rect_win.tl().x;
    int y_min = rect_win.tl().y;
    int x_max = rect_win.br().x;
    int y_max = rect_win.br().y;
    bool in_rect = ( p.x >= x_min );
    in_rect = in_rect & ( p.x <= x_max );
    in_rect = in_rect & ( p.y >= y_min );
    in_rect = in_rect & ( p.y <= y_max );
    return in_rect;
}
//process to create dynamic windown
//
void update_dynamic_win( vector<vector<Point>> pre_mid_lane ,  Rect &rect  )
{
    vector<int> x, y;
    for( size_t i = 0 ; i < pre_mid_lane.size() ; i ++)
    {
        get_elements( pre_mid_lane.at(i), x, y);
    }
    int max_x = (int)*max_element( x.begin(), x.end() ); 
    int max_y = (int)*max_element( y.begin(), y.end() );
    int min_x = (int)*min_element( x.begin(), x.end() );
    int min_y = (int)*min_element( y.begin(), y.end() );

    rect = Rect( Point( min_x - delta_x_pixel , rect.tl().y ), Point( max_x + delta_x_pixel, rect.br().y ) );
    
}
void get_elements( vector<Point>& vec_point, vector<int>& x, vector<int>& y )
{
    for( size_t i = 0 ; i < vec_point.size() ; i ++)
    {
        x.push_back( vec_point.at(i).x );
        y.push_back( vec_point.at(i).y );
    }
}

void get_mid_lane( vector<vector<Point>>& contour, Rect rect, vector<vector<Point>>& mid_lane )
{
    
    for( size_t i = 0 ; i < contour.size() ; i ++)
    {
        bool flag = true;
        for( size_t j = 0 ; j < contour[i].size() ; j++ )
        {
            if( !point_in_rect ( rect, contour[i].at(j) ) )
            {
                flag = false;
                break;
            }
        }
        // if point in rect
        if(flag)
        {
            mid_lane.push_back( contour.at(i) );
        }
    }
}