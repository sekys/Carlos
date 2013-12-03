#pragma once
#include <windows.h>
#include "../Carlos/architecture/DllExports.h"
#include "../Carlos/architecture/modules/class.ModulVykreslovania.hpp"
#include <iostream>
#include "Hra\opengl.h"

Scene scene;

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*) {
	printf("Dll nacitane.\n");
	return 1;
}

using namespace Architecture;

class RealModulVykreslovania : public ModulVykreslovania {
public:
	void vykresliObrazokSRozsirenouRealitou(In in) {
		// http://r3dux.org/2012/01/how-to-convert-an-opencv-cvmat-to-an-opengl-texture/
		// Set incoming texture format to:
		// GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
		// GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
		// Work out other mappings as required ( there's a list in comments in main() )
		GLenum inputColourFormat = GL_BGR;
		if (in.image.data.channels() == 1) {
			inputColourFormat = GL_LUMINANCE;
		}
		//scene.shaders.setBackgroud(in.image.data.ptr(), in.image.data.cols, in.image.data.rows, GL_RGB);	
		/*Image obrazokNaPozadie = in.image; /// Tento obrazok sa zobrazuje na pozadi
		scene.shaders.setTexture(obrazokNaPozadie.data);

		for(uint i=0; i < in.najdenePozicie.size(); i++) {
		Position poziciaTextu = in.najdenePozicie.at(i);
		}*/
	}

	void init() {
		openGLInit();
	}
	virtual bool isThreaded() { return true; }
};

IMPEXP void* callFactory() {
	return static_cast< void* > (new RealModulVykreslovania());
}

/** 
* Hlavna funkcia main
* @return int
*/
int main(int argc, char **argv) {
	RealModulVykreslovania moja;
	moja.init();
	return 0;
}