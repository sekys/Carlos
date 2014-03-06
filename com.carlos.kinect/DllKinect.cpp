#include "DllKinect.h"
#include "MyFreenectDevice.h"

Point3f DllKinect::getAktualnaRotaciaHlavy() {
	Point3f rot;
	rot.x = 60.0f;
	rot.y= 20.0f;
	rot.z = 52.0f;
	return rot;
}

void DllKinect::init() {
	bool die(false);
	int iter(0);
	Point pt1, pt2, pt3;

	this->spustiKalibraciu();

	CascadeClassifier face_cascade;
	face_cascade.load("haarcascade_frontalface_alt.xml");

	Mat depthMat(Size(640,480),CV_16UC1);
	Mat depthf  (Size(640,480),CV_8UC1);
	Mat rgbMat(Size(640,480),CV_8UC3,Scalar(0));
	Mat ownMat(Size(640,480),CV_8UC3,Scalar(0));
	Mat grayMat(Size(640,480), CV_8UC1);

	Freenect::Freenect freenect;
	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);

	namedWindow("rgb",CV_WINDOW_AUTOSIZE);
	namedWindow("depth",CV_WINDOW_AUTOSIZE);
	device.setLed(LED_BLINK_RED_YELLOW);
	device.startVideo();
	device.startDepth();
	device.setDepthFormat(FREENECT_DEPTH_REGISTERED);
	device.setLed(LED_GREEN);
	while (!die) {
		device.getVideo(rgbMat);
		device.getDepth(depthMat);

		//face recognition start
		cvtColor(rgbMat, grayMat, CV_BGR2GRAY);
		equalizeHist(grayMat, grayMat);

		std::vector<Rect> faces;

		face_cascade.detectMultiScale(grayMat, faces, 1.1, 3, CV_HAAR_FIND_BIGGEST_OBJECT|CV_HAAR_SCALE_IMAGE, Size(30,30));
		if(faces.size()>0){
			pt1.x = faces[0].x + faces[0].width;
			pt1.y = faces[0].y + faces[0].height;
			pt2.x = faces[0].x;
			pt2.y = faces[0].y;

			rectangle(rgbMat, pt1, pt2, cvScalar(0, 255, 0, 0), 1, 8, 0);
		}

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
		depthMat.convertTo(depthf, CV_8UC1, 255.0/2048.0);
		cv::imshow("depth",depthf);

		char k = cvWaitKey(5);
		if( k == 27 ){
			cvDestroyWindow("rgb");
			cvDestroyWindow("depth");
			break;
		}
		if(iter >= 500){
			iter=0;
			if(pt3.x!=0)
			{
				double wx, wy, wz = depthMat.at<ushort>(pt3.y, pt3.x);
				Vec3d result = DepthToWorld(pt3.x, pt3.y, wz);
				wx = result[0];
				wy = result[1];
				double mi[4];
				mi[0] = wx;
				mi[1] = wy;
				mi[2] = wz;
				mi[3] = 1;
				Mat i = Mat(4, 1, CV_64F, mi);

				Mat j = M*i;
				printf("%.0f %.0f %.0f\n", wx, wy, wz);
			}
		}
		iter++;
	}

	device.stopVideo();
	device.stopDepth();
	device.setLed(LED_OFF);
}


void DllKinect::spustiKalibraciu() {
	bool die(false);
	int iter = 0, j = 0, token = 0;
	vector<Vec3f> circles[4];
	Vec3d result;

	double mx[4][4] = {{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};
	//mx treba zistovat z kinectu
	double my[4][4] = {{50, 100, 200, 300}, {50, 100, 200, 300}, {750, 750, 750, 800}, {1, 1, 1, 1}};

	//nacitavanie bodov pre mx
	Mat depthMat(Size(640,480),CV_16UC1);
	Mat depthf  (Size(640,480),CV_8UC1);
	Mat rgbMat(Size(640,480),CV_8UC3,Scalar(0));
	Mat ownMat(Size(640,480),CV_8UC3,Scalar(0));
	Mat grayMat(Size(640,480), CV_8UC1);

	Freenect::Freenect freenect;
	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);
	//nastav LED na zapnute
	device.setLed(LED_BLINK_RED_YELLOW);

	device.startVideo();
	device.startDepth();
	device.setDepthFormat(FREENECT_DEPTH_REGISTERED);
	device.setLed(LED_RED);
	while (j<4) {
		device.getVideo(rgbMat);
		device.getDepth(depthMat);
		cvtColor(rgbMat, grayMat, CV_BGR2GRAY);
		if(iter >= 100){
			token=0;
			device.setLed(LED_YELLOW);
			//circle recognition

			GaussianBlur( grayMat, grayMat, Size(9, 9), 2, 2);

			HoughCircles( grayMat, circles[j], CV_HOUGH_GRADIENT, 1, grayMat.rows/8, 200, 100, 0, 0);
			for(size_t i =0; i<circles[j].size(); i++){
				if(token == 0){
					token=1;
					result = DepthToWorld(cvRound(circles[j][i][0]), cvRound(circles[j][i][1]), depthMat.at<ushort>(cvRound(circles[j][i][1]), cvRound(circles[j][i][0])));
					mx[0][j] = result[0];
					mx[1][j] = result[1];
					mx[2][j] = result[2];
					j++;
					device.setLed(LED_RED);
					iter=0;
				}
			}
		}
		iter++;
	}

	device.stopVideo();
	device.stopDepth();
	device.setLed(LED_OFF);

	//vypocet
	Mat x = Mat(4, 4, CV_64F, mx);
	Mat y = Mat(4, 4, CV_64F, my);

	M = y*(x.inv());
	//cout << "M = "<< endl << " "  << M << endl << endl;
	//double mi[4] = {{51}, {36}, {87}, {1}};
	//Mat i = Mat(4, 1, CV_64F, mi);
	//Mat j = M*i;
	//out << "M = "<< endl << " "  << j << endl << endl;
}