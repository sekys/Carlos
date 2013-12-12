#pragma once
#define NOMINMAX
#include "../Carlos/architecture/modules/class.ModulKinect.hpp"

using namespace Architecture;

class DllKinect : public ModulKinect {
public:
	virtual Point3f getAktualnaRotaciaHlavy();
	virtual void init();

	/** 
	*	@brief funkcia sl��iaca na inicializ�ciu modulu
	*	
	*	T�to funkcia detekuje 4 body kinectom, ktor� sa nach�dzaj� v preddefinovan�ch bodoch v priestore.
	*	Tieto body potom s� spracovan� do mat�c a je z nich vypo��tan� transforma�n� matica.
	*/

	virtual void spustiKalibraciu();
};

