#pragma once
#include "../libs/include/freenect/libfreenect.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

extern Mat M;
/// Trieda pre pr�cu s kinectom
/** MyFreenectDevice je trieda kni�nice freenect obstar�vaj�ca v�etk� pr�cu s d�tami kinectu */

class Mutex {
public:
	Mutex() {
		pthread_mutex_init( &m_mutex, NULL );
	}
	void lock() {
		pthread_mutex_lock( &m_mutex );
	}
	void unlock() {
		pthread_mutex_unlock( &m_mutex );
	}
private:
	pthread_mutex_t m_mutex;
};

class MyFreenectDevice : public Freenect::FreenectDevice {
public:
	MyFreenectDevice(freenect_context *_ctx, int _index);
	// Do not call directly even in child
	void VideoCallback(void* _rgb, uint32_t timestamp);
	// Do not call directly even in child
	void DepthCallback(void* _depth, uint32_t timestamp);
	bool getVideo(Mat& output);

	bool getDepth(Mat& output);

private:
	std::vector<uint8_t> m_buffer_depth;
	std::vector<uint8_t> m_buffer_rgb;
	std::vector<uint16_t> m_gamma;
	Mat depthMat;
	Mat rgbMat;
	Mat ownMat;
	::Mutex m_rgb_mutex;
	::Mutex m_depth_mutex;
	bool m_new_rgb_frame;
	bool m_new_depth_frame;
};

extern Freenect::Freenect freenect;
extern MyFreenectDevice& device;


/**	Funkcia m� na vstupe s�radnice x a y videa z RGB kamery a vr�ti re�lne s�radnice x a y v re�lnom svete z poh�adu kinectu
*	@param x x-ov� s�radnica vo videu
*	@param y y-ov� s�radnica vo videu
*	@return s�radnice prepo��tan� na re�lny svet z poh�adu kinectu
*/

Vec3f DepthToWorld(int x, int y, int depthValue);
void UlozMaticu();
bool NacitajMaticu();