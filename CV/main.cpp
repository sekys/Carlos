#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

using namespace cv;

int main() {
	IplImage* img = cvLoadImage( "Janka.jpg" );
	cvNamedWindow( "Kocka", CV_WINDOW_AUTOSIZE );
	cvShowImage( "Kocka", img );
	cvWaitKey(0);
	cvReleaseImage( &img );
	cvDestroyWindow( "Kocka" );
}