#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int a = 0;
int b = 1;
Mat img, src,gray, dst, edges, img_blur,thresh;
Mat test(Size(576,482), CV_8U, Scalar(0));
static void drawShapes( Mat& image, const vector<vector<Point> >& Shapes, Scalar_< double> &color , const String& nameshapes);
void on_trackbar(int, void*)
{
	// int dilation_type = 0;
	// if( a == 0 ){ dilation_type = MORPH_RECT; }
	// else if( a == 1 ){ dilation_type = MORPH_CROSS; }
	// else if( a == 2) { dilation_type = MORPH_ELLIPSE; }
	Mat element = getStructuringElement( MORPH_ELLIPSE, Size( 2*b + 1, 2*b+1 ), Point( b, b ) );
	dilate( test, dst, element );
	//erode( src, dst, element); 
	imshow("test",dst);
	// threshold(img_blur, edges, a, 255, THRESH_BINARY_INV);
	// imshow("test",edges);
}

int main(int argc, char** argv)
{

	img = imread("data/bt4.png",IMREAD_GRAYSCALE);
	
	medianBlur ( img, img_blur, 3 );
	threshold(img_blur, thresh, 115, 255, THRESH_BINARY);
 	imshow("src", img);
 	//////////////////////////////////
 	
	vector<vector<Point> > Shapes;
	vector<vector<Point> > contours;
	vector<Point> approx;
	findContours(thresh, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
	Scalar_<double> circles = Scalar(255);
	for( size_t i = 0; i < contours.size(); i++ )
	{
		approxPolyDP(contours[i], approx, arcLength(contours[i], true)*0.01, true);
		if( fabs(contourArea(approx)) > 110 && fabs(contourArea(approx)) < 525 )//&& isContourConvex(approx) ) 
		{
			Shapes.push_back(approx);	
		}
	}
	drawShapes(test, Shapes, circles, "circle");
	/////////////////////////////////
	// Mat element = getStructuringElement( MORPH_CROSS, Size( 2*b + 1, 2*b+1 ), Point( b, b ) );
	//cout << test.size() << endl;
	// dilate( test, dst, element );
	// imshow("test", dst);
	createTrackbar( "max", "src", &b, 255, on_trackbar );
	on_trackbar(0,0);
	waitKey(0);
	return 0;
}
static void drawShapes( Mat& image, const vector<vector<Point> >& Shapes, Scalar_< double> &color , const String& nameshapes)
{
	int n = 1;
	Point_<int> p;
	const Point* a = &p;
    for( size_t i = 0; i < Shapes.size(); i++ )
    {
    	p = Point_<int>(0,0);
       for(size_t j = 0; j < Shapes[i].size(); j++)
       	{
       		p += Shapes[i][j];
       	}
        //(int)Shapes[i].size();
        //putText(image, nameshapes, p[0],FONT_HERSHEY_COMPLEX_SMALL, 1, color, 1, LINE_AA);
        p = p/int(Shapes[i].size());
        cout << Shapes.size() <<endl;
    	polylines(image, &a, &n, 1, true, color, 5, FILLED );
    }  
}