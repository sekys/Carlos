#pragma once
#include "../Carlos/architecture/modules/class.ModulVypocitaniaPolohy.hpp"
#include <iostream>
#include <math.h>

#define PI	3.14159265
#define R	6378.1 // Radius of the Earth in km

using namespace Architecture;
using namespace cv;

class MojaTrieda {
protected:
	double CAMERA_DISTANCE_FROM_WINDOW_IN_M;
	double CAMERA_X_POSITION_IN_WINDOW_IN_M;

	double inline deg2rad(double deg) {
		return deg * (PI / 180.);
	}

	double inline positive(double n) {
		return n < 0 ? (-1)*n : n;
	}

	double calculateDistanceInM(GPS gps1, GPS gps2) {
		double dLat = deg2rad(gps2.latitude - gps1.latitude);
		double dLon = deg2rad(gps2.longitude - gps1.longitude);

		double a = 
			sin(dLat / 2.) * sin(dLat / 2.) +
			cos(deg2rad(gps1.latitude)) * cos(deg2rad(gps2.latitude)) * 
			sin(dLon / 2.) * sin(dLon / 2.);

		double c = 2 * atan2(sqrt(a), sqrt(1 - a));
		return R * c * 1000.; // Distance in m
	}

	double calculateSpeedInKmForHr(double distanceInM, double timeInMillis) {
		return (distanceInM / 1000.) / (timeInMillis / 1000. / 3600.);
	}

	double calculateSpeedInMForS(double distanceInM, double timeInMillis) {
		return distanceInM / (timeInMillis / 1000.);
	}

	double calculateCosBtwnCameraAndObj(double objPosInWindowInM) {
		double objDstFromCam = objPosInWindowInM > CAMERA_X_POSITION_IN_WINDOW_IN_M ?
			objPosInWindowInM - CAMERA_X_POSITION_IN_WINDOW_IN_M :
		CAMERA_X_POSITION_IN_WINDOW_IN_M - objPosInWindowInM;

		double hyp = sqrt(objDstFromCam*objDstFromCam + CAMERA_DISTANCE_FROM_WINDOW_IN_M*CAMERA_DISTANCE_FROM_WINDOW_IN_M);
		double cos = CAMERA_DISTANCE_FROM_WINDOW_IN_M / hyp;

		return cos;
	}

	double calculateObjDistance(double alphaInDeg, double betaInDeg, double length) {
		double d1 = length * sin(deg2rad(alphaInDeg)) * sin(deg2rad(betaInDeg));
		double d2 = sin(deg2rad(alphaInDeg + betaInDeg));
		return d1 / d2;
	}

	Point2f calculateTextPos(Point3f headPos, Point3f detecObjPos) {
		Point2f textPos;
		textPos.x = 0.;
		textPos.y = 0.;

		double distance = positive(detecObjPos.z) + positive(headPos.z);

		int height = headPos.y > detecObjPos.y ? headPos.y - detecObjPos.y : detecObjPos.y - headPos.y;
		int width  = headPos.x > detecObjPos.x ? headPos.x - detecObjPos.x : detecObjPos.x - headPos.x;

		if (height > 0.) {
			double hyp = sqrt(distance*distance + height*height);
			double sin = height / hyp;
			double cos = distance / hyp;
			double res = (positive(detecObjPos.z) / cos) * sin;

			if (headPos.y > detecObjPos.y) textPos.y = detecObjPos.y + res;
			else textPos.y = detecObjPos.y - res;
		}

		if (width > 0.) {
			double hyp = sqrt(distance*distance + width*width);
			double sin = width / hyp;
			double cos = distance / hyp;
			double res = (positive(detecObjPos.z) / cos) * sin;

			if (headPos.x > detecObjPos.x) textPos.x = detecObjPos.x + res;
			else textPos.x = detecObjPos.x - res;
		}

		return textPos;
	}

public:
	MojaTrieda() {
		CAMERA_DISTANCE_FROM_WINDOW_IN_M = 0.4;
		CAMERA_X_POSITION_IN_WINDOW_IN_M = 0.3;
	}
};