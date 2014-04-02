#pragma once
#define NOMINMAX
#include "../com.carlos.architecture/modules/class.ModulKinect.hpp"
#include "../libs/include/freenect/libfreenect.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "libfreenect.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>

#include <pthread.h>

#include <math.h>

using namespace cv;
using namespace std;

using namespace Architecture;

class DllKinect : public ModulKinect {
public:

	int die;


Mat M;

pthread_mutex_t gl_backbuf_mutex;

uint8_t *rgb_back, *rgb_mid;

Mat depthMat;
Mat depthf;
Mat rgbMat;
Mat grayMat;

CascadeClassifier face_cascade;

freenect_context *f_ctx;
freenect_device *f_dev;
int freenect_angle;
int freenect_led;

bool m_new_rgb_frame;
bool m_new_depth_frame;

uint16_t *depth;

	virtual Point3f getAktualnaRotaciaHlavy();
	virtual void init();

	/** 
	*	@brief funkcia slúžiaca na inicializáciu modulu
	*	
	*	Táto funkcia detekuje 4 body kinectom, ktoré sa nachádzajú v preddefinovaných bodoch v priestore.
	*	Tieto body potom sú spracované do matíc a je z nich vypoèítaná transformaèná matica.
	*/

	virtual void NacitajMaticu();
	virtual void freenect_threadfunc();
	virtual Vec3f DepthToWorld(int x, int y, int depthValue);
	virtual void getDepth();
	virtual void getRGB();
	virtual void rgb_cb(freenect_device *dev, void *rgb, uint32_t timestamp);
	virtual void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp);
};

