#pragma once
#include "../../Carlos/architecture/modules/class.ModulVykreslovania.hpp"

using namespace Architecture;

class DllModulVykreslovania : public ModulVykreslovania {
private:


public:
	/// Spusta sa v 2. vlakne !
	void vykresliObrazokSRozsirenouRealitou(In in);
	void init();
	bool isThreaded();
};