#pragma once
#include "../../com.carlos.architecture/modules/class.ModulVykreslovania.hpp"

using namespace Architecture;

class FrameData {
public:
	ModulVykreslovania::In vstup;
	bool hasVstup;
	ControllerCommands command;
	float deltaTime;

	FrameData() {
		
	}
	~FrameData() {
		if(hasVstup) {
			vstup.horizont.release();
			vstup.image.data.release();
		}
	}
};