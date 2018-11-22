
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int r1 = 86, s1 = 2, r2 = 191, s2 = 215;
Mat image ,new_image, canny_img;


int computeOutput(int, int, int, int, int);
void on_trackbar(int, void*)
{
	for(size_t y = 0; y < image.rows; y++){
        for(size_t x = 0; x < image.cols; x++){
            for(size_t c = 0; c < 3; c++){
                int output = computeOutput(image.at<Vec3b>(y,x)[c], r1, s1, r2, s2);
                new_image.at<Vec3b>(y,x)[c] = saturate_cast<uchar>(output);
            }
        }
    }
    //canny( new_image, canny_img, )
    imwrite( "bt4_con.png", new_image);
    imshow( "New Image", new_image );
}
int main()
{
   	Mat img;
	img = imread( "data/bt4.png", IMREAD_GRAYSCALE );
	//medianBlur(img, image, 3);
	GaussianBlur( img, image, Size( 3, 3 ), 3, 0 );
    new_image = image.clone();
    // cout<<"Enter r1: "<<endl; cin>>r1;
    // cout<<"Enter s1: "<<endl; cin>>s1;
    // cout<<"Enter r2: "<<endl; cin>>r2;
    // cout<<"Enter s2: "<<endl; cin>>s2;

    namedWindow( "Original Image", 1 );
    imshow( "Original Image", image );
    namedWindow( "New Image", 1 );

	createTrackbar( "r1", "Original Image", &r1, 255, on_trackbar );
	createTrackbar( "s1", "Original Image", &s1, 255, on_trackbar );
	createTrackbar( "r2", "Original Image", &r2, 255, on_trackbar );
	createTrackbar( "s2", "Original Image", &s2, 255, on_trackbar );
	on_trackbar(0, 0);
    waitKey();

    return 0;
}

int computeOutput(int x, int r1, int s1, int r2, int s2)
{
    float result;
    if(0 <= x && x <= r1){
        result = s1/r1 * x;
    }else if(r1 < x && x <= r2){
        result = ((s2 - s1)/(r2 - r1)) * (x - r1) + s1;
    }else if(r2 < x && x <= 255){
        result = ((255 - s2)/(255 - r2)) * (x - r2) + s2;
    }
    return (int)result;
}