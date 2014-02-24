#pragma once
#include <carlos_global.h>
#include "../../com.carlos.architecture/modules/class.ModulVykreslovania.hpp"

class DllModulVykreslovania : public Architecture::ModulVykreslovania {
private:


public:
	/// Spusta sa v 2. vlakne !
	void vykresliObrazokSRozsirenouRealitou(In in);
	void init();
	bool isThreaded();
};