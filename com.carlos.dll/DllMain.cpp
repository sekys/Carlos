#include <windows.h>
#include "DllExports.h"
#include "modules\class.ModulSpracovania.hpp"
#include <iostream>

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*) {
	printf("Dll nacitane\n");
	return 1;
}

using namespace Architecture;

class MojaTrieda : public ModulSpracovania {
	// TU SI DATE SVOJ KOD
	virtual Out detekujObjekty(In in) {
		//cout << "Dostal som\n\n" << in<< "\n\n";

		// Hladam na obrazovky a ked najdem objekt ulozim ho + jeho poziciu
		DetekovanyObjekt najdenyObjekt;
		najdenyObjekt.position.x = 100.f;
		najdenyObjekt.position.y = 100.f;
		if(in.recepts.size() > 1 ) {
			najdenyObjekt.objekt = in.recepts.at(0);
		}

		// Poslem vysledok dalej
		Out out;
		out.objects.push_back(najdenyObjekt);
		return out;
	}

	virtual void init() {
		cout << "Tu sa zapina aplikacia\n";
	}
};

IMPEXP void* callFactory() {
	return static_cast< void* > (new MojaTrieda());
}