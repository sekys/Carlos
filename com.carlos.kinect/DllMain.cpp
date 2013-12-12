#include <windows.h>
#include "../Carlos/architecture/DllExports.h"
#include <iostream>
#include "DllKinect.h"

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*) {
	printf("Dll nacitane\n");
	return 1;
}

IMPEXP void* callFactory() {
	return static_cast< void* > (new DllKinect());
}

/** 
*	Hlavná funkcia modulu. Na zaèiatku spustí inicializáciu a kinect, ktorý potom do ukonèenia programu zisuje súradnice bodu pozerania
*/


int main(int argc, char **argv) {
	DllKinect* dll = new DllKinect();
	dll->init();
	delete dll;
}