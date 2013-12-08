#pragma once
#include <windows.h>
#include "../../Carlos/architecture/DllExports.h"
#include "class.DllModulVykreslovania.hpp"

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*) {
	printf("Dll nacitane.\n");
	return 1;
}

using namespace Architecture;

IMPEXP void* callFactory() {
	return static_cast< void* > (new DllModulVykreslovania());
}

/** 
* Hlavna funkcia main
* @return int
*/
int main(int argc, char **argv) {
	DllModulVykreslovania moja;
	moja.init();
	return 0;
}