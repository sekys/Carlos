#include <windows.h>
#include "../Carlos/architecture/DllExports.h"
#include "../Carlos/architecture/entities/entities.h"
#include "../Carlos/architecture/modules/class.ModulVypocitaniaPolohy.hpp"
#include "../Carlos/architecture/configuration/class.Configuration.h"
#include "class.MojaTrieda.hpp"

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*) {
	printf("Dll nacitane\n");
	return 1;
}

using namespace Architecture;
using namespace cv;

class DllModulVypocitaniaPolohy : public MojaTrieda, public ModulVypocitaniaPolohy {
public:
	virtual Out vypocitajPolohuTextu(In in) {	
		double distance = calculateDistanceInM(in.gps, in.gpsPolohaObjektu);

		// TODO: Sekerak: poloha objektu sa urcuje ako boundary, cize obdlznik.... 
		double cos = calculateCosBtwnCameraAndObj(in.polohaObjektu.x);

		// TODO: Sekerak: poloha objektu sa urcuje ako boundary, cize obdlznik.... 
		Point3f detecObjPos;
		detecObjPos.x = in.polohaObjektu.x;
		detecObjPos.y = in.polohaObjektu.y;
		detecObjPos.z = distance * cos;

		Point2f textPos = calculateTextPos(in.rotaciaHlavy, detecObjPos);

		Out out;
		out.najdeny = true;
		out.polohaTextu = textPos;
		return out;
	}

	virtual void init() {
		/*
		* Z configuracie musim ziskat data:
		*  - CAMERA_DISTANCE_FROM_WINDOW_IN_M
		*  - CAMERA_X_POSITION_IN_WINDOW_IN_M
		*/
		CAMERA_DISTANCE_FROM_WINDOW_IN_M = Configuration::getInstance().getConfigf("CAMERA_DISTANCE_FROM_WINDOW_IN_M");
		CAMERA_X_POSITION_IN_WINDOW_IN_M = Configuration::getInstance().getConfigf("CAMERA_X_POSITION_IN_WINDOW_IN_M");
	}
};

IMPEXP void* callFactory() {
	return static_cast< void* > (new DllModulVypocitaniaPolohy());
}