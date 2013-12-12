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
*	Hlavn� funkcia modulu. Na za�iatku spust� inicializ�ciu a kinect, ktor� potom do ukon�enia programu zis�uje s�radnice bodu pozerania
*/


int main(int argc, char **argv) {
	DllKinect* dll = new DllKinect();
	dll->init();
	delete dll;
}