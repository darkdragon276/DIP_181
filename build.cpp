#include <iostream>
#include <string.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <math.h>

using namespace std;
using namespace cv;

// string intToString(int number)
// {
//  std::stringstream ss;
//  ss << number;
//  return ss.str();
// }

const string Window1="video";
bool playVideo = true;

int H_min = 0;
int S_min = 0;
int V_min = 179;
int H_max = 176;
int S_max = 191;
int V_max = 255;
int gr_thresh = 114;

int H_shd_min = 0;
int S_shd_min = 0;
int V_shd_min = 0;
int H_shd_max = 255;
int S_shd_max = 255;
int V_shd_max = 255;

Scalar temp_thresh_min = Scalar(0,0,107);
Scalar temp_thresh_max = Scalar(255,255,255);
Scalar temp_2_min = Scalar(0,0,179);
Scalar temp_2_max = Scalar(176,191,255);
Scalar temp_3_min = Scalar(0,1,101);
Scalar temp_3_max = Scalar(255,255,255);
Scalar min_laneInShadow = Scalar(0,0,179);
Scalar max_laneInShadow = Scalar(176,191,255);
Scalar min_shadow = Scalar(0,0,0);
Scalar max_shadow = Scalar(255,255,255);
int low_canny_thresh = 100;
int max_low_thresh = 80;
int ratio_1 = 3;

void Trackbar(void){
    createTrackbar("H_min", Window1, &H_min, 255);
    createTrackbar("S_min", Window1, &S_min, 255);
    createTrackbar("V_min", Window1, &V_min, 255);
    createTrackbar("H_max", Window1, &H_max, 255);
    createTrackbar("S_max", Window1, &S_max, 255);
    createTrackbar("V_max", Window1, &V_max, 255);

    createTrackbar("H_shd_min", Window1, &H_shd_min, 255);
    createTrackbar("S_shd_min", Window1, &S_shd_min, 255);
    createTrackbar("V_shd_min", Window1, &V_shd_min, 255);
    createTrackbar("H_shd_max", Window1, &H_shd_max, 255);
    createTrackbar("S_shd_max", Window1, &S_shd_max, 255);
    createTrackbar("V_shd_max", Window1, &V_shd_max, 255);

    createTrackbar("gray thresh", Window1, &gr_thresh, 255);
    createTrackbar("canny low",Window1,&low_canny_thresh,max_low_thresh);
}

void GetParamTrackbar(string winname){
    getTrackbarPos("H_min", winname);
    getTrackbarPos("S_min", winname);
    getTrackbarPos("V_min", winname);
    getTrackbarPos("H_max", winname);
    getTrackbarPos("S_max", winname);
    getTrackbarPos("V_max", winname);

    getTrackbarPos("H_shd_min", winname);
    getTrackbarPos("S_shd_min", winname);
    getTrackbarPos("V_shd_min", winname);
    getTrackbarPos("H_shd_max", winname);
    getTrackbarPos("S_shd_max", winname);
    getTrackbarPos("V_shd_max", winname);

    getTrackbarPos("gray thresh", winname);
    getTrackbarPos("canny low", winname);
}

// void thresh_callback(int, void* ){
    
// }

Mat Sobel_transform(Mat gray_frame){
    int ddepth = CV_8U;
    int scale = 1;
    int delta = 0;
    Mat grad_x, grad_y, abs_grad_x, abs_grad_y, grad, thresh_grad_y;
    Sobel(gray_frame, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs( grad_x, abs_grad_x );
    Sobel(gray_frame, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );

    
    addWeighted( abs_grad_x, 0.2, abs_grad_y, 0.8, 0, grad );
    threshold(grad,thresh_grad_y, 80, 255,THRESH_BINARY);
    // imshow("grad_x", abs_grad_x);
    // imshow("grad_y",abs_grad_y);
    return thresh_grad_y;
}

Mat Morphological(Mat src_frame){

}

Mat birdViewTranform(const Mat &src, const Point2f &center)
{
    int cut_out_width = 90;
    Point2f src_vertices[4];

    int width = src.size().width;
    int height = src.size().height;
    // int skyline  = 85;

    src_vertices[0] = Point(0, 85);
    src_vertices[1] = Point(width, 85);
    src_vertices[2] = Point(width, height);
    src_vertices[3] = Point(0, height);

    Point2f dst_vertices[4];
    dst_vertices[0] = Point(0, 0);
    dst_vertices[1] = Point(240, 0);
    dst_vertices[2] = Point(240 - cut_out_width, 320);
    dst_vertices[3] = Point(cut_out_width, 320);

    Mat M = getPerspectiveTransform(src_vertices, dst_vertices);

    Mat dst(320, 240, CV_8UC3);
    warpPerspective(src, dst, M, dst.size(), INTER_LINEAR, BORDER_CONSTANT);

    vector<Point2f> dstPoints, srcPoints;
    dstPoints.push_back(Point2f(160,160-85));
    perspectiveTransform(dstPoints,srcPoints, M.inv());
    circle( dst, dstPoints.at(0), 4, Scalar(255,255,255), -1, 8, 0 );

    
    return dst;
}

Mat laneInShadow(const Mat &src_HSV)
{
    Mat shadowMask, shadowHSV, laneShadow;
    // cvtColor(src, imgHSV, COLOR_BGR2HSV);

    inRange(src_HSV, Scalar(H_shd_min,S_shd_min,V_shd_min), Scalar(H_shd_max, S_shd_max, V_shd_max), shadowMask);

    src_HSV.copyTo(shadowHSV, shadowMask);
    //Nhung muc nguong 255 (mau trang) co trong shadowMask thi trong src_HSV se duoc copy vao shadowHSV

    inRange(shadowHSV, min_laneInShadow, max_laneInShadow, laneShadow);
    imshow("laneShadow",laneShadow);
    return laneShadow;
}

Point MassOfRoad(const Mat &src_RGB, Scalar tmp_min, Scalar tmp_max){
    Mat src_HSV, src_gray, road_thresh, road_gr_thresh, canny_output, road_thresh_inv;
    Point center = Point(0,0);
    int canny_thresh = 100;
    vector<vector<Point> > contours_1; //Tao 1 vector 2 chieu voi moi phan tu la Point
    vector<Vec4i> hierarchy_1;

    vector<vector<Point> > contours_2; //Tao 1 vector 2 chieu voi moi phan tu la Point
    vector<Vec4i> hierarchy_2;

    vector<vector<Point> > contours_3; //Tao 1 vector 2 chieu voi moi phan tu la Point
    vector<Vec4i> hierarchy_3;

    int max_cnt_index = 0;
    double max_cnt_size = 0;
    int max_cnt_index_hsv = 0;
    double max_cnt_size_hsv = 0;

    cvtColor(src_RGB, src_HSV, COLOR_BGR2HSV);
    cvtColor(src_RGB, src_gray, COLOR_BGR2GRAY);

    inRange(src_HSV, tmp_min, tmp_max, road_thresh);
    bitwise_not(road_thresh,road_thresh_inv);

    threshold(src_gray,road_gr_thresh, 100, 255,THRESH_BINARY_INV);
    Canny( src_gray, canny_output, canny_thresh, canny_thresh*2, 3 );

    /// Find contours
    findContours( canny_output, contours_1, hierarchy_1, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
    findContours( road_thresh_inv, contours_2, hierarchy_2, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
    findContours( road_gr_thresh, contours_3, hierarchy_3, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

    vector<Point > max_contours_3;
    for (int i=0; i<contours_3.size();i++){
        double area = contourArea(contours_3[i]);
        if (area > max_cnt_size){
            max_cnt_size = area;
            max_cnt_index = i;
            if (max_cnt_size >45000)
                max_contours_3 = contours_3[max_cnt_index];
        }
    }

    vector<Point > max_contours_2;
    for (int i=0; i<contours_2.size();i++){
        double area = contourArea(contours_2[i]);
        if (area > max_cnt_size_hsv){
            max_cnt_size_hsv = area;
            max_cnt_index_hsv = i;
            if (max_cnt_size_hsv >45000)
                max_contours_2 = contours_2[max_cnt_index_hsv];
        }
    }

    Moments mu = moments(max_contours_3, false);
    Point2f mc = Point2f(mu.m10/mu.m00 , mu.m01/mu.m00 );

    Moments mu_hsv = moments(max_contours_2, false);
    Point2f mc_hsv = Point2f(mu_hsv.m10/mu_hsv.m00 , mu_hsv.m01/mu_hsv.m00 );
    // vector<Moments> mu(contours_3.size());
    // for( int i = 0; i<contours_3.size(); i++ )
    // {
    //     mu[i] = moments( contours_3[i], false );
    // }
    // vector<Point2f> mc(contours_3.size());
    // for( int i = 0; i<contours_3.size(); i++)
    // {
    //         mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); 
    // }

    /// Draw contours
    Mat drawing_1 = Mat::zeros( canny_output.size(), CV_8UC3 );
    for( int i = 0; i< contours_1.size(); i++ )
    {
       Scalar color_1 = Scalar( 0, 0, 255 );
       drawContours( drawing_1, contours_1, i, color_1, 2, 8, hierarchy_1, 0, Point() );
    }
    Mat drawing_2 = Mat::zeros( road_thresh.size(), CV_8UC3 );
    for( int i = 0; i< contours_2.size(); i++ )
    {
       Scalar color_2 = Scalar( 0, 255, 0);
       Scalar color_3_1 = Scalar (0,50,128);
       drawContours( drawing_2, contours_2, i, color_2, 2, 8, hierarchy_2, 0, Point() );
       circle( drawing_2, mc_hsv, 4, color_3_1, -1, 8, 0 );
    }
    Mat drawing_3 = Mat::zeros( road_gr_thresh.size(), CV_8UC3 );
    for( int i = 0; i< contours_3.size(); i++ )
    {
       Scalar color_3 = Scalar( 255, 0, 0 );
       drawContours( drawing_3, contours_3, i, color_3, 2, 8, hierarchy_3, 0, Point() );
       Scalar color_3_1 = Scalar (0,50,128);
       circle( drawing_3, mc, 4, color_3_1, -1, 8, 0 );
    }
    // imshow("contours_canny", drawing_1);
    // imshow("thresh_abc", road_thresh_inv);
    imshow("contours_HSV", drawing_2);
    imshow("contours_gr", drawing_3);
    // cout << mc_hsv.x << ' ' << mc_hsv.y;
    return mc_hsv;
}

int main(int argc, char const *argv[])
{
    string filename = "/home/nam/Desktop/unity_src/test_unity.mp4";
    VideoCapture capture(filename);
    if( !capture.isOpened() )
        throw "Error when reading steam_avi";
    namedWindow( "video", WINDOW_AUTOSIZE );
    Trackbar();

    Mat frame, HSV_frame, gray, thresh_HSV,thresh_gray,edge, and_gray_hsv, and_edge_gr, laneShadow; //ma tran de luu tung frame anh
    // vector<int> compression_params;
    // compression_params.push_back( IMWRITE_PNG_COMPRESSION );
    // compression_params.push_back(9);
    while(1)
    {
        if (playVideo){
            capture >> frame;
            GaussianBlur(frame, frame, Size(3,3), 0, 0, BORDER_DEFAULT );
        }    

        if(frame.empty())
            break;
        cvtColor(frame, HSV_frame, COLOR_BGR2HSV);
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        GetParamTrackbar(Window1);

        Point2f center = MassOfRoad(frame, temp_3_min, temp_3_max);
        laneShadow = laneInShadow(HSV_frame);
        inRange(HSV_frame, Scalar(H_min, S_min, V_min), Scalar(H_max, S_max, V_max), thresh_HSV);
        // inRange(HSV_frame, temp_2_min, temp_2_max, thresh_HSV);
        threshold(gray,thresh_gray, gr_thresh, 255,THRESH_BINARY_INV);
        Canny(gray, edge, low_canny_thresh, low_canny_thresh*ratio_1,3 );
        // bitwise_and(thresh_HSV, gray, and_gray_hsv);
        // bitwise_xor(thresh_gray,edge,and_edge_gr);
        // Mat grad = Sobel_transform(gray);
        Mat grad = Sobel_transform(thresh_HSV);
        //erode( grad, grad, Mat() );
        Mat birdView = birdViewTranform(grad, center);
        // Mat birdView = birdViewTranform(gray);
        // Mat grad = Sobel_transform(birdView);
        
        // 0 71 22 0 255 255s
        cout << "lower thresh " << Scalar(H_min, S_min, V_min) << " higher thresh " << Scalar(H_max, S_max, V_max) << endl;
        imshow("video", frame);
        imshow("hsv",HSV_frame);
        // imshow("thresh_HSV",thresh_HSV);
        // imshow("graythresh",thresh_gray);
        // imshow("edge",edge);
        // imshow("and",and_gray_hsv);
        // imshow("and_edge",and_edge_gr);
        imshow("grad",grad);
        imshow("birdView",birdView);
        // imshow("gray",gray);

        char key = waitKey(20);
        if(key == 'p')
            playVideo = !playVideo; 
        else if (key == 27)
            break;
    }
    // releases and window destroy are automatic in C++ interface
    return 0;
}