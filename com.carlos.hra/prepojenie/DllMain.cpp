#pragma once
#include <carlos_global.h>
#include <windows.h>
#include "../../com.carlos.architecture/DllExports.h"
#include "class.DllModulVykreslovania.hpp"

#ifdef _DEBUG
	#pragma comment(lib, "../Debug/com.carlos.architecture.lib")
#else
	#pragma comment(lib, "../Release/com.carlos.architecture.lib")
#endif

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