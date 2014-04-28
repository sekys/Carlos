#include <carlos_global.h>
#include "../com.carlos.kinect/DllKinect.h"

uint16_t *depth;
bool m_new_rgb_frame;
bool m_new_depth_frame;
pthread_mutex_t gl_backbuf_mutex;
uint8_t *rgb_back, *rgb_mid;


Point3f DllKinect::getAktualnaRotaciaHlavy() { 
	return rot;
}

float vzdialenost(Point3f c, Point3f d){
	return sqrt((c.x-d.x)*(c.x-d.x)+(c.y-d.y)*(c.y-d.y)+(c.z-d.z)*(c.z-d.z));
}

void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp)
{
	pthread_mutex_lock(&gl_backbuf_mutex);
	depth = (uint16_t*)v_depth;

	m_new_depth_frame=true;
	pthread_mutex_unlock(&gl_backbuf_mutex);
}

void rgb_cb(freenect_device *dev, void *rgb, uint32_t timestamp)
{
	pthread_mutex_lock(&gl_backbuf_mutex);

	// swap buffers
	assert (rgb_back == rgb);
	rgb_back = rgb_mid;
	freenect_set_video_buffer(dev, rgb_back);
	rgb_mid = (uint8_t*)rgb;

	//memcpy(rgbMat.ptr(), rgb_mid, 640*480*3);
	m_new_rgb_frame = true;
	pthread_mutex_unlock(&gl_backbuf_mutex);
}

void DllKinect::getRGB(){
	pthread_mutex_lock(&gl_backbuf_mutex);
	if(m_new_rgb_frame){
		memcpy(rgbMat.ptr(), rgb_mid, 640*480*3);
		cv::cvtColor(rgbMat, rgbMat, CV_BGR2RGB);
		m_new_rgb_frame=false;
	}
	pthread_mutex_unlock(&gl_backbuf_mutex);
}

void DllKinect::getDepth(){
	pthread_mutex_lock(&gl_backbuf_mutex);
	if(m_new_depth_frame){
		memcpy(depthMat.ptr(), depth, 640*480*2);
		depthMat.convertTo(depthf, CV_8UC1, 255.0/2048.0);
		m_new_depth_frame=false;
	}
	pthread_mutex_unlock(&gl_backbuf_mutex);
}

/**	Funkcia m· na vstupe s˙radnice x a y videa z RGB kamery a vr·ti re·lne s˙radnice x a y v re·lnom svete z pohæadu kinectu
*	@param x x-ov· s˙radnica vo videu
*	@param y y-ov· s˙radnica vo videu
*	@return s˙radnice prepoËÌtanÈ na re·lny svet z pohæadu kinectu
*/

Vec3f DllKinect::DepthToWorld(int x, int y, int depthValue)
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

/** 
*	Hlavn· funkcia modulu. Na zaËiatku spustÌ inicializ·ciu a kinect, ktor˝ potom do ukonËenia programu zisùuje s˙radnice bodu pozerania
*/

void DllKinect::freenect_threadfunc()
{
	int accelCount = 0;
	int iter(0);
	Point pt1, pt2, pt3;
	cv::vector<Rect> faces;
	float d1, d2, koef;
	Point3f p;

	face_cascade.load("../data/haarcascade_frontalface_alt.xml");

	//freenect_set_tilt_degs(f_dev,freenect_angle);
	freenect_set_led(f_dev,LED_RED);
	freenect_set_depth_callback(f_dev, depth_cb);
	freenect_set_video_callback(f_dev, rgb_cb);
	freenect_set_video_mode(f_dev, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB));
	freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_REGISTERED));
	freenect_set_video_buffer(f_dev, rgb_back);

	freenect_start_depth(f_dev);
	freenect_start_video(f_dev);

	//printf("'w'-tilt up, 's'-level, 'x'-tilt down, '0'-'6'-select LED mode, 'f'-video format\n");
	//printf("'e' - auto exposure, 'b' - white balance, 'r' - raw color, 'm' - mirror\n");

	while (!die && freenect_process_events(f_ctx) >= 0) {


		//get rgbMat 
		getRGB();

		//get depthMat
		getDepth();

		//face recognition start
		cvtColor(rgbMat, grayMat, CV_BGR2GRAY);
		equalizeHist(grayMat, grayMat);

		face_cascade.detectMultiScale(grayMat, faces, 1.1, 3, CV_HAAR_FIND_BIGGEST_OBJECT|CV_HAAR_SCALE_IMAGE, Size(30,30)); //heap corruption problem!!!!!!!
		if(faces.size()>0){
			pt1.x = faces[0].x + faces[0].width;
			pt1.y = faces[0].y + faces[0].height;
			pt2.x = faces[0].x;
			pt2.y = faces[0].y;

			rectangle(rgbMat, pt1, pt2, cvScalar(0, 255, 0, 0), 1, 8, 0);
		}
		faces.clear();

		//face recognition end
		pt3.x=((pt1.x-pt2.x)/2)+pt2.x;
		pt3.y=((pt1.y-pt2.y)/3)+pt2.y;

		//pt3.x=320;
		//pt3.y=240;

		if(pt3.x!=0)
		{
			rgbMat.at<Vec3b>(pt3.y, pt3.x)[0]=0;
			rgbMat.at<Vec3b>(pt3.y, pt3.x)[1]=255;
			rgbMat.at<Vec3b>(pt3.y, pt3.x)[2]=0;
		}



		cv::imshow("rgb", rgbMat);
		cv::imshow("depth",depthf);
		char k = cvWaitKey(5);
		if( k == 27 ){
			cvDestroyWindow("rgb");
			cvDestroyWindow("depth");
			break;
		}
			if(pt3.x!=0)
			{
				double wx, wy, wz = depthMat.at<ushort>(pt3.y, pt3.x);
				Vec3d result = DepthToWorld(pt3.x, pt3.y, wz);

				p.x	= result[0];
				p.y = result[1];
				p.z = wz;

				d1 = vzdialenost(a, p);
				d2 = vzdialenost(b, p);

				koef = d2/(d1+d2);
				rot.x = ((a.x-b.x)-((a.x-b.x)-540)/2)*koef;
			}

	}

	printf("\nshutting down streams...\n");

	freenect_stop_depth(f_dev);
	freenect_stop_video(f_dev);

	freenect_close_device(f_dev);
	freenect_shutdown(f_ctx);


	printf("-- done!\n");
}


void DllKinect::NacitajBody(){
	ifstream file;
	int i, j, k;

	file.open("../data/kinect_points.txt");
	file >> a.x;
	file >> a.y;
	file >> a.z;
	file >> b.x;
	file >> b.y;
	file >> b.z;

	file.close();
}

void DllKinect::init()
{
	rgb_back = (uint8_t*)malloc(640*480*3);
	rgb_mid = (uint8_t*)malloc(640*480*3);

	die = 0;
	gl_backbuf_mutex = PTHREAD_MUTEX_INITIALIZER;

	rot.x=0;
	rot.y= 320.0f; //vyska obrazovky/2
	rot.z = 250.0f;

	depthMat = Mat(Size(640,480),CV_16UC1);
	depthf = Mat (Size(640,480),CV_8UC1);
	rgbMat= Mat(Size(640,480),CV_8UC3,Scalar(0));
	grayMat= Mat(Size(640,480), CV_8UC1);

	freenect_angle = 0;

	printf("Kinect camera test\n");

	int i;
	for (i=0; i<2048; i++) {
		float v = i/2048.0;
		v = powf(v, 3)* 6;
	}

	if (freenect_init(&f_ctx, NULL) < 0) {
		printf("freenect_init() failed\n");
	}

	freenect_set_log_level(f_ctx, FREENECT_LOG_DEBUG);
	freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA));

	int nr_devices = freenect_num_devices (f_ctx);
	printf ("Number of devices found: %d\n", nr_devices);

	int user_device_number = 0;

	if (nr_devices < 1) {
		freenect_shutdown(f_ctx);
	}

	if (freenect_open_device(f_ctx, &f_dev, user_device_number) < 0) {
		printf("Could not open device\n");
		freenect_shutdown(f_ctx);
	}

	NacitajBody();

	freenect_threadfunc();

}