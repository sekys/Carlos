#pragma once
#include "baseTypes.h"
#include "opencv2/opencv.hpp"
#include <opencv2\core\core.hpp>

namespace Architecture
{
	using namespace std;
	using namespace cv;

	/* Pre urcenie polohy pouzivaj:
	
	typedef Point_<int> Point2i;
	typedef Point2i Point;
	typedef Size_<int> Size2i;
	typedef Size2i Size;
	typedef Rect_<int> Rect;
	typedef Point_<float> Point2f;
	typedef Point_<double> Point2d;
	typedef Size_<float> Size2f;
	typedef Point3_<int> Point3i;
	typedef Point3_<float> Point3f;
	typedef Point3_<double> Point3d;

	A dalsie zakladne struktury pouzivaj z: opencv2\core\core.hpp !
	*/
	inline ostream& operator<< (ostream& out, Point2f& object) {
		out << "Point2f(x:" << object.x << ", y:" << object.y << ")";
		return out;
	}


	inline ostream& operator<< (ostream& out, Point3f& object) {
		out << "Point3f(x:" << object.x << ", y:" << object.y << ", z:" << object.z << ")";
		return out;
	}
}