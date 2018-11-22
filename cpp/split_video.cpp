#include <iostream>
#include <string>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;


int main()
{
    string filename = "/home/nam/Desktop/CDS/live.avi";
    VideoCapture capture(filename);
    if( !capture.isOpened() )
        throw "Error when reading steam_avi";
    namedWindow( "video", WINDOW_AUTOSIZE );

    Mat frame;
    int index = 0;
    vector<int> compression_params;
    compression_params.push_back( IMWRITE_PNG_COMPRESSION );
    compression_params.push_back(9);
    for( ; ; )
    {
        index++;
        capture >> frame;
        if(frame.empty())
            break;
        if( index%25 == 0 )
        {
            string number = to_string( index );
            string direction( "/home/nam/Desktop/CDS/data_img/live/image_" );
            string filetype( ".jpg" );
            
            string filename;

            filename.insert( static_cast<int>( filename.length() ), direction );
            filename.insert( static_cast<int>( filename.length() ), number );
            filename.insert( static_cast<int>( filename.length() ), filetype );
            

            char *cstr = new char[filename.length() + 1];
            strcpy(cstr, filename.c_str());

            cout << cstr << endl;
            imwrite( cstr, frame, compression_params );
            imshow("video", frame);
        }
        if (waitKey(30) >= 0)
        break;
    }
    return 0;
    // releases and window destroy are automatic in C++ interface
}