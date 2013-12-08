#pragma once
#include "../../../Carlos/architecture/modules/class.ModulVykreslovania.hpp"

using namespace Architecture;

class FrameData {
public:
	ModulVykreslovania::In vstup;
	bool hasVstup;
	ControllerCommands command;
	float deltaTime;
};