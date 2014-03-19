#include "MyFreenectDevice.h"
#include <fstream>

Mat M;

MyFreenectDevice::MyFreenectDevice(freenect_context *_ctx, int _index)
	: Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(FREENECT_DEPTH_REGISTERED),m_buffer_rgb(FREENECT_VIDEO_RGB), m_gamma(2048), m_new_rgb_frame(false), m_new_depth_frame(false),
	depthMat(Size(640,480),CV_16UC1), rgbMat(Size(640,480),CV_8UC3,Scalar(0)), ownMat(Size(640,480),CV_8UC3,Scalar(0))
{
	for( unsigned int i = 0 ; i < 2048 ; i++) {
		float v = i/2048.0;
		v = std::pow(v, 3)* 6;
		m_gamma[i] = v*6*256;
	}
}

void MyFreenectDevice::VideoCallback(void* _rgb, uint32_t timestamp) {
	//std::cout << "RGB callback" << std::endl;
	m_rgb_mutex.lock();
	uint8_t* rgb = static_cast<uint8_t*>(_rgb);
	rgbMat.data = rgb;
	m_new_rgb_frame = true;
	m_rgb_mutex.unlock();
};

void MyFreenectDevice::DepthCallback(void* _depth, uint32_t timestamp) {
	//std::cout << "Depth callback" << std::endl;
	m_depth_mutex.lock();
	uint16_t* depth = static_cast<uint16_t*>(_depth);
	depthMat.data = (uchar*) depth;
	m_new_depth_frame = true;
	m_depth_mutex.unlock();
}

bool MyFreenectDevice::getVideo(Mat& output) {
	m_rgb_mutex.lock();
	if(m_new_rgb_frame) {
		cv::cvtColor(rgbMat, output, CV_RGB2BGR);
		m_new_rgb_frame = false;
		m_rgb_mutex.unlock();
		return true;
	} else {
		m_rgb_mutex.unlock();
		return false;
	}
}

bool MyFreenectDevice::getDepth(Mat& output) {
	m_depth_mutex.lock();
	if(m_new_depth_frame) {
		depthMat.copyTo(output);
		m_new_depth_frame = false;
		m_depth_mutex.unlock();
		return true;
	} else {
		m_depth_mutex.unlock();
		return false;
	}
}

Freenect::Freenect freenect;
MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);

Vec3f DepthToWorld(int x, int y, int depthValue)
{
	static const double FovH = 1.0144686707507438;
	static const double FovV = 0.78980943449644714;
	static const double XtoZ = tan(FovH/2)*2 ;
	static const double YtoZ = tan(FovV/2)*2 ;

	Vec3f result;
	result[0] = (x/640-.5) * depthValue * XtoZ;
	result[1] = (0.5-y/480) * depthValue * YtoZ;
	result[2] = depthValue;
	return result;
}

void UlozMaticu(){
ofstream file;
int j, k;

file.open("matica.txt");
file << "1" << endl;
for(j = 0; j<4; j++){
for(k = 0; k<4; k++){
file << M.at<double>(j, k) << endl;
}
}
}

bool NacitajMaticu(){
ifstream file;
int i, j, k;

file.open("matica.txt");
file >> i;
if(i == 1){
M = Mat(4, 4, CV_64F);
for(j = 0; j<4; j++){
for(k = 0; k<4; k++){
file >> M.at<double>(j, k);
}
}
file.close();
return true;
} else
return false;
}
