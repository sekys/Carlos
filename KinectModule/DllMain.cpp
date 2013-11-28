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

	virtual Rotation getAktualnaRotaciaHlavy() {
		Rotation rot;
		rot.rotation[0] = 60;
		rot.rotation[1] = 20;
		rot.rotation[2] = 52;
		return rot;
	}

	virtual void init() {
		cout << "Tu sa zapina aplikacia\n";
	}

};

IMPEXP void* callFactory() {
	return static_cast< void* > (new MojaTrieda());
}