/** @file class.DllModulVykreslovania.cpp
* trieda ktora sa stara o inicializaciu a o posielanie obrazkov do buffera
*/
#include <carlos_global.h>
#include "class.DllModulVykreslovania.hpp"
#include <iostream>
#include "..\Hra\opengl.h"
#include "..\Hra\Help\class.FrameData.hpp"

using namespace Architecture;

Scene scene;

cv::Mat dokresliHorizont(cv::Mat bg, cv::Mat horizont) {
	int a = bg.channels();
	int b = horizont.channels();

	for( int j= 0; j < bg.cols; j++ )
	{
		int pocet;
		pocet = 0;
		for( int i = 0; i < bg.rows; i++ )
		{
			if(horizont.at<uchar>(i,j) == 0){
				bg.at<Vec3b>(i,j)[0] = 0;
				bg.at<Vec3b>(i,j)[1] = 255;
				bg.at<Vec3b>(i,j)[2] = 0;
				pocet++;
				if(pocet == 2) break;
			} 
		}
	}
	
	return bg;
}

/** 
* Funkcia ma na vstupe 1 parameter a to je prijaty obrazok, stara sa o ulozenie do buffera
* Spusta sa v 2. vlakne !
* @param in - prijaty obrazok
* @return void 
*/
void DllModulVykreslovania::vykresliObrazokSRozsirenouRealitou(In in) 
{
	// Spracuj obrazok
	in.image.data = in.image.data.clone();
	in.horizont = in.horizont.clone();
	in.image.data = dokresliHorizont(in.image.data, in.horizont); // .clone()
	cv::flip(in.image.data, in.image.data, 0);

	// Posli obrazok dalej
	scene.sendVstup(in);	

	/*Image obrazokNaPozadie = in.image; /// Tento obrazok sa zobrazuje na pozadi
	scene.shaders.setTexture(obrazokNaPozadie.data);

	for(uint i=0; i < in.najdenePozicie.size(); i++) {
	Position poziciaTextu = in.najdenePozicie.at(i);
	}*/
}

void DllModulVykreslovania::init() {
	openGLInit();
}
bool DllModulVykreslovania::isThreaded() { return true; }
