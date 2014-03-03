#include <windows.h>
#include "class.MojaTrieda.hpp"
#include "../com.carlos.architecture/DllExports.h"
#include "../com.carlos.architecture/modules/class.ModulVypocitaniaPolohy.hpp"
#include "../com.carlos.architecture/configuration/class.Configuration.hpp"
#pragma comment(lib, "../Debug/com.carlos.architecture.lib")

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*) {
	printf("Dll nacitane\n");
	return 1;
}

using namespace Architecture;

class DllModulVypocitaniaPolohy : public MojaTrieda, public ModulVypocitaniaPolohy {
public:
	virtual Out vypocitajPolohuTextu(In in) {	
		double distance = calculateDistanceInM(in.gps, in.gpsPolohaObjektu);

		// TODO: Sekerak: poloha objektu sa urcuje ako boundary, cize obdlznik.... 
		double cos = calculateCosBtwnCameraAndObj(in.polohaObjektu.center.x-in.polohaObjektu.size.width/2);

		// TODO: Sekerak: poloha objektu sa urcuje ako boundary, cize obdlznik.... 
		Point3f detecObjPos;
		detecObjPos.x = in.polohaObjektu.center.x-in.polohaObjektu.size.width/2;
		detecObjPos.y = in.polohaObjektu.center.y-in.polohaObjektu.size.height/2;
		detecObjPos.z = distance * cos;

		Point2f textPos = calculateTextPos(in.rotaciaHlavy, detecObjPos);

		Out out;
		out.id = in.id;
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