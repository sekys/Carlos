#pragma once
#include "../com.carlos.architecture/entities/entities.h"
#include <iostream>
#include <math.h>

#define PI	3.14159265 /// cislo PI
#define R	6378.1 /// Radius of the Earth in km

using namespace Architecture;

class MojaTrieda {
protected:
	double CAMERA_DISTANCE_FROM_WINDOW_IN_M; /// Vdialenost kamery, ktora natacala scenu od okna v metroch
	double CAMERA_X_POSITION_IN_WINDOW_IN_M; /// Pozicia kamery v ramci sirky okna v metroch
	
	/**
	 * Funkcia ma na vstupe uhol v stupnoch a vrati uhol v radianoch
	 * @param deg uhol v stupnoch
	 * @return uhol v riadianoch
	 */
	double inline deg2rad(double deg) {
		return deg * (PI / 180.);
	}	
	
	/**
	 * Funkcia vrati vzdy kladne cislo
	 * @param n cislo
	 * @return vzdy kladne cislo
	 */
	double inline positive(double n) {
		return n < 0 ? (-1)*n : n;
	}

	/**
	 * Funkcia vypocita priamu vzdialenost medzi dvoma gps suradnicami v metroch
	 * @param gps1 jedna gps suradnica
	 * @param gps2 druha gps suradnica
	 * @return vzdialenost v metroch
	 */
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

	/**
	 * Funckia vypocita rychlost zo vzdialenosti a casu v km/h
	 * @param distanceInM prejdena vzdialenost v metroch
	 * @param timeInMillis ubehnuty cas v milisekundach
	 * @return rychlost v km/h
	 */
	double calculateSpeedInKmForHr(double distanceInM, double timeInMillis) {
		return (distanceInM / 1000.) / (timeInMillis / 1000. / 3600.);
	}

	/**
	 * Funckia vypocita rychlost zo vzdialenosti a casu v m/s
	 * @param distanceInM prejdena vzdialenost v metroch
	 * @param timeInMillis ubehnuty cas v milisekundach
	 * @return rychlost v m/s
	 */
	double calculateSpeedInMForS(double distanceInM, double timeInMillis) {
		return distanceInM / (timeInMillis / 1000.);
	}

	/**
	 * Funkcia vypocita cosinus uhla medzi kamerou a bodom na okne
	 * @param objPosInWindowInM pozicia bodu na okne v ramci sirky okna v m
	 * @return cosinus uhla
	 */
	double calculateCosBtwnCameraAndObj(double objPosInWindowInM) {
		double objDstFromCam = objPosInWindowInM > CAMERA_X_POSITION_IN_WINDOW_IN_M ?
			objPosInWindowInM - CAMERA_X_POSITION_IN_WINDOW_IN_M :
		CAMERA_X_POSITION_IN_WINDOW_IN_M - objPosInWindowInM;

		double hyp = sqrt(objDstFromCam*objDstFromCam + CAMERA_DISTANCE_FROM_WINDOW_IN_M*CAMERA_DISTANCE_FROM_WINDOW_IN_M);
		double cos = CAMERA_DISTANCE_FROM_WINDOW_IN_M / hyp;

		return cos;
	}

	/**
	 * Funkcia vypocita vzdialenost objektu z dvoch uhlov a prejdenej vzdialenosti po priamke (vzdialenost objektu od priamky)
	 * @param alphaInDeg prvy uhol v stupnoch
	 * @param betaInDeg druhy uhol v stupnoch
	 * @param length prejdena vzdialenost
	 * @return vzdialenost objektu od priamky
	 */
	double calculateObjDistance(double alphaInDeg, double betaInDeg, double length) {
		double d1 = length * sin(deg2rad(alphaInDeg)) * sin(deg2rad(betaInDeg));
		double d2 = sin(deg2rad(alphaInDeg + betaInDeg));
		return d1 / d2;
	}

	/**
	 * Funkcia vypocita poziciu textu na okne
	 * @param headPos pozicia hlavy v priestore
	 * @param detecObjPos pozicia objektu v priestore
	 * @return x,y poloha textu na okne (stred, kde sa ma nachadzat)
	 */
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