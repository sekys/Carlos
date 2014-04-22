#include <windows.h>
#include "class.MojaTrieda.hpp"
#include "../com.carlos.architecture/DllExports.h"
#include "../com.carlos.architecture/modules/class.ModulVypocitaniaPolohy.hpp"
#include "../com.carlos.architecture/configuration/class.Configuration.hpp"
#ifdef _DEBUG
	#pragma comment(lib, "../Debug/com.carlos.architecture.lib")
#else
	#pragma comment(lib, "../Release/com.carlos.architecture.lib")
#endif

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*) {
	printf("Dll nacitane\n");
	return 1;
}

using namespace Architecture;

class DllModulVypocitaniaPolohy :  public ModulVypocitaniaPolohy, public MojaTrieda {
public:
	virtual Out vypocitajPolohuTextu(In in) {
		// Vzpocitanie priamej vydialenosti detegovaneho objektu od auta
		double distanceInM = calculateDistanceInM(in.gps, in.gpsPolohaObjektu);

		// Velkost pixelu v metroch na okne auta
		double sizeOfPixelInMInWidth  = CAR_WINDOW_WIDTH_IN_M/CAR_WINDOW_WIDTH_IN_PX;
		double sizeOfPixelInMInHeight = CAR_WINDOW_HEIGHT_IN_M/CAR_WINDOW_HEIGHT_IN_PX;

		// Poloha objektu v okne pride v pixeloch
		double objPosInWindowInPxX = in.polohaObjektu.center.x-in.polohaObjektu.size.width/2;
		double objPosInWindowInPxY = in.polohaObjektu.center.y-in.polohaObjektu.size.height/2;
		
		// Upravena poloha objektu v okne v metroch
		double objPosInWindowInMX  = objPosInWindowInPxX*sizeOfPixelInMInWidth;
		double objPosInWindowInMY  = objPosInWindowInPxY*sizeOfPixelInMInHeight;

		// Vypocitame cosinus na upravu priamej vzdialenosti medzi autom a objektom
		double cos = calculateCosBtwnCameraAndObj(objPosInWindowInMX);

		// Pozicia detegovaneho objektu v metroch
		Point3f detecObjPosition;
		detecObjPosition.x = objPosInWindowInMX;
		detecObjPosition.y = objPosInWindowInMY;
		detecObjPosition.z = - distanceInM * cos;

		// Pozicia hlavy pride v milimetroch, premenime ju na metre
		Point3f headPosition;
		headPosition.x = in.rotaciaHlavy.x/1000.;
		headPosition.y = in.rotaciaHlavy.y/1000.;
		headPosition.z = in.rotaciaHlavy.z/1000.;

		Point2f textPos = calculateTextPos(headPosition, detecObjPosition);

		Out out;
		out.id = in.id;
		out.najdeny = true;
		out.polohaTextu = normalizeTextPosToInterval(textPos);
		return out;
	}

	virtual double getHeadPosition(Point3f headPosition) {
		// Pozicia hlavy pride v milimetroch, premenime ju na metre
		headPosition.x = headPosition.x/1000.;
		headPosition.y = headPosition.y/1000.;
		headPosition.z = headPosition.z/1000.;

		// Vytvorime bod, ktory reprezentuje polchu na ktoru sa premieta video,
		// aby sme vedeli vypocitat v akom uhle sa nachadza pouzivatelova hlava
		// v ramci osi x
		Point3f projectionAreaPosition;
		projectionAreaPosition.x = CAR_WINDOW_WIDTH_IN_M/2;
		projectionAreaPosition.y = CAR_WINDOW_HEIGHT_IN_M/2;
		projectionAreaPosition.z = - PROJECTION_AREA_DISTANCE_FROM_CAR_WINDOW_IN_M;

		Point2f intersection = calculateTextPos(headPosition, projectionAreaPosition);

		return normalizeHeadPosInWindowToInterval(intersection.x);
	}

	virtual void init() {
		/*
		 * Z configuracie musim ziskat data:
		 *  - PROJECTION_AREA_DISTANCE_FROM_CAR_WINDOW_IN_M
		 *  - CAR_WINDOW_WIDTH_IN_M
		 *  - CAR_WINDOW_HEIGHT_IN_M
		 *  - CAMERA_DISTANCE_FROM_WINDOW_IN_M
		 *  - CAMERA_X_POSITION_IN_WINDOW_IN_M
		 */
		PROJECTION_AREA_DISTANCE_FROM_CAR_WINDOW_IN_M = 
			Configuration::getInstance().getConfigf("projection_area_distance_from_car_window_in_m");

		CAR_WINDOW_WIDTH_IN_M = Configuration::getInstance().getConfigf("CAR_WINDOW_WIDTH_IN_M");
		CAR_WINDOW_HEIGHT_IN_M = Configuration::getInstance().getConfigf("CAR_WINDOW_HEIGHT_IN_M");

		CAMERA_DISTANCE_FROM_WINDOW_IN_M = Configuration::getInstance().getConfigf("CAMERA_DISTANCE_FROM_WINDOW_IN_M");
		CAMERA_X_POSITION_IN_WINDOW_IN_M = Configuration::getInstance().getConfigf("CAMERA_X_POSITION_IN_WINDOW_IN_M");
	}

private:

	Point2f normalizeTextPosToInterval(Point2f textPosition) {
		if (textPosition.x < 0.0) {
			textPosition.x = 0.0;
		}

		if (textPosition.x > CAR_WINDOW_WIDTH_IN_M) {
			textPosition.x = CAR_WINDOW_WIDTH_IN_M;
		}

		if (textPosition.y < 0.0) {
			textPosition.y = 0.0;
		}

		if (textPosition.y > CAR_WINDOW_HEIGHT_IN_M) {
			textPosition.y = CAR_WINDOW_HEIGHT_IN_M;
		}

		textPosition.x /= CAR_WINDOW_WIDTH_IN_M;
		textPosition.y /= CAR_WINDOW_HEIGHT_IN_M;

		return textPosition;
	}

	double normalizeHeadPosInWindowToInterval(double xPosInM) {
		if (xPosInM < CAR_WINDOW_WIDTH_IN_M/2) { // Lava polka okna
			// Ak je to mimo laveho okraju okna
			if (xPosInM < 0.0) {
				xPosInM = 0.0; 
			}

			xPosInM /= (CAR_WINDOW_WIDTH_IN_M/2);

			return xPosInM - 1.0;
		} else if (xPosInM > CAR_WINDOW_WIDTH_IN_M/2) { // Prava polka okna
			// Ak je to mimo praveho okraja okna
			if (xPosInM > CAR_WINDOW_WIDTH_IN_M) {
				xPosInM = CAR_WINDOW_WIDTH_IN_M;
			}

			xPosInM -= (CAR_WINDOW_WIDTH_IN_M/2);
			xPosInM /= (CAR_WINDOW_WIDTH_IN_M/2);

			return xPosInM;
		} else { // Stred okna
			return 0.0;
		}
	}
};

IMPEXP void* callFactory() {
	return static_cast< void* > (new DllModulVypocitaniaPolohy());
}