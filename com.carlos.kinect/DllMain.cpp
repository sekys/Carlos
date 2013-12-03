#include <windows.h>
#include "../Carlos/architecture/DllExports.h"
#include "../Carlos/architecture/modules/class.ModulKinect.hpp"
#include <iostream>

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*) {
	printf("Dll nacitane\n");
	return 1;
}

using namespace Architecture;

class MojaTrieda : public ModulKinect {
	// TU SI DATE SVOJ KOD

	virtual vec3 getAktualnaRotaciaHlavy() {
		vec3 rot;
		rot.x = 60.0f;
		rot.y= 20.0f;
		rot.z = 52.0f;
		return rot;
	}

	virtual void init() {
		cout << "Tu sa zapina aplikacia\n";
	}

};

IMPEXP void* callFactory() {
	return static_cast< void* > (new MojaTrieda());
}