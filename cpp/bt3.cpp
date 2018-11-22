
//code mau kiem tren trang chu opencv example_drawShapes.cpp
//link bang mau
//http://idesign.vn/graphic-design/bang-mau-cmyk-rgb-thong-dng-1908.html
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
using namespace cv;
using namespace std;

#define 	thresh_level	225
#define		area_min		1000
#define		area_max		50000
#define		accurency		0.01
const char* wndname = "multi_verticals Detection Demo";
const char* cir = "circle ";
const char* ell = "ellipe ";
const char* tri = "triangle_3edges" ;
const char* rect = "rectangle_4edges";
const char* quad = "quadrilateral_4edges";
const char* squ = "square_4edges" ;
const char* pent = "pentagon_5edges" ;
const char* hexa = "hexagon_6edges";
Scalar_<double> circles = Scalar(243,197,70), ellises = Scalar(138,0,22), triangles = Scalar(148,83,5), rectangles = Scalar(156,153,0), 
				quadrilaterals = Scalar(54,117,23), squares = Scalar(23,90,167), pentagons = Scalar(162,0,124), hexagons = Scalar(137,137,137);
				
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
static double angle( Point pt1, Point pt2, Point ptmid )
{
    double dx1 = pt1.x - ptmid.x;
    double dy1 = pt1.y - ptmid.y;
    double dx2 = pt2.x - ptmid.x;	
    double dy2 = pt2.y - ptmid.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}
static double compareLength( Point pt1, Point pt2, Point ptmid )
{
	double dx1 = pt1.x - ptmid.x;
    double dy1 = pt1.y - ptmid.y;
    double dx2 = pt2.x - ptmid.x;
    double dy2 = pt2.y - ptmid.y;
    return ( sqrt(dx1*dx1 + dy1*dy1 + 1e-10) - sqrt(dx2*dx2 + dy2*dy2) );
}
static void drawShapes( Mat& image, const vector<vector<Point> >& Shapes, Scalar_< double> &color , const String& nameshapes)
{
    for( size_t i = 0; i < Shapes.size(); i++ )
    {
        const Point* p = &Shapes[i][0];
        int n = (int)Shapes[i].size();
        polylines(image, &p, &n, 1, true, color, 2, LINE_AA);
        putText(image, nameshapes, p[0],FONT_HERSHEY_COMPLEX_SMALL, 1, color, 1, LINE_AA);
    }
    
    imshow(wndname, image);
}
// returns sequence of Shapes detected on the image.
static uint findShapes( Mat& image, vector<vector<Point> >& Shapes)
{
    
    Mat gray;
    vector<vector<Point> > contours;
	vector<Point> approx;

    cvtColor(image, gray, COLOR_RGB2GRAY);
    threshold(gray, gray, thresh_level, 255, THRESH_BINARY);
    findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    
    // test each contour
    for( size_t i = 0; i < contours.size(); i++ )
    {
    	Shapes.clear();
        approxPolyDP(contours[i], approx, arcLength(contours[i], true)*accurency, true);
        // kiem tra area va convex cho nhung hinh co do phuc tap cao hon
        //if( approx.size() == 4 && fabs(contourArea(approx)) > 1000 && isContourConvex(approx) ) 
        // fabs de lay tri tuyet doi, phong truong hop ham
        // contourArea tinh ra gia tri sai voi nhung diem
        // cat nhau
        if(fabs(contourArea(approx)) > area_min && fabs(contourArea(approx)) < area_max)
        {	
        	Shapes.push_back(approx);
        	if(approx.size() == 3)
        	{
        		drawShapes(image, Shapes, triangles , tri);
        	}
        	else if(approx.size() == 4 )
        	{
	            double maxCosine = 0, Difference_Length = 0;
	            for( int j = 2; j < 5; j++ )
	            {
	                // find the maximum cosine of the angle between joint edges
	                double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
	                maxCosine = MAX(maxCosine, cosine);
	            }
	            Difference_Length = fabs(compareLength(approx[0], approx[2], approx[1]));
	            // push_back cat diem vao vector
	            if( maxCosine < 0.3 && Difference_Length < 0.3)
	                drawShapes(image, Shapes, squares, squ);
	            else if(maxCosine < 0.3)
	           		drawShapes(image, Shapes, rectangles, rect);
	           	else
	           		drawShapes(image, Shapes, quadrilaterals, quad);   
	        }
	        else if(approx.size() == 5)
	        {
	        	drawShapes(image, Shapes, pentagons, pent);
	        }
	        else if(approx.size() == 6)
	        {
	        	drawShapes(image, Shapes, hexagons, hexa);
	        }
	        else if(approx.size() == 16)
	        {
	        	drawShapes(image, Shapes, circles, cir);
	        }
	        else if( 10 < approx.size() && approx.size() < 16)
	        {
	        	drawShapes(image, Shapes, ellises, ell);
	        	//cout << approx.size() << endl;
	        }
	        
        }
    }
   
}
// the function draws all the Shapes in the image

int main(int argc, char** argv)
{
	Mat image = imread(argv[1],IMREAD_COLOR);
	vector<vector<Point> > Shapes;
    findShapes(image, Shapes);
    imwrite("bai3_.png",image);
    waitKey();
    return 0;
}

 // find Shapes in every color plane of the image
    // for( int c = 0; c < 3; c++ )
    // {
    //     int ch[] = {c, 0};
    //     mixChannels(&image, 1, &gray0, 1, ch, 1);
    //     // try several threshold levels
    //     for( int l = 0; l < N; l++ )
    //     {
    //         // hack: use Canny instead of zero threshold level.
    //         // Canny helps to catch Shapes with gradient shading
    //         if( l == 0 )
    //         {
    //             // apply Canny. Take the upper threshold from slider
    //             // and set the lower to 0 (which forces edges merging)
    //             Canny(gray0, gray, 0, thresh, 5);
    //             // dilate canny output to remove potential
    //             // holes between edge segments
    //             //dilate(gray, gray, Mat());
    //         }
    //         else
    //         {
    //             // apply threshold if l!=0:
    //             //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
    //             gray = gray0 >= (l+1)*255/N;
    //         }
    //         // find contours and store them all as a list
    /////////////////////////////////////////////////////////
    //     }
    // }