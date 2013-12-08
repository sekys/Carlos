/** @file class.DllModulVykreslovania.cpp
* trieda ktora sa stara o inicializaciu a o posielanie obrazkov do buffera
*/
#include "class.DllModulVykreslovania.hpp"
#include <iostream>
#include "..\Hra\opengl.h"
#include "..\Hra\class.SpracovanieSnimku.hpp"
#include "..\Hra\Help\class.FrameData.hpp"

Scene scene;

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
	in.image.data = SpracovanieSnimku::vypocetHorizontu(in.image.data, in.image.data); // .clone()
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
