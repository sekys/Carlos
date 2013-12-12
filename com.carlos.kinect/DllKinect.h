#pragma once
#define NOMINMAX
#include "../Carlos/architecture/modules/class.ModulKinect.hpp"

using namespace Architecture;

class DllKinect : public ModulKinect {
public:
	virtual Point3f getAktualnaRotaciaHlavy();
	virtual void init();

	/** 
	*	@brief funkcia slúžiaca na inicializáciu modulu
	*	
	*	Táto funkcia detekuje 4 body kinectom, ktoré sa nachádzajú v preddefinovaných bodoch v priestore.
	*	Tieto body potom sú spracované do matíc a je z nich vypoèítaná transformaèná matica.
	*/

	virtual void spustiKalibraciu();
};

