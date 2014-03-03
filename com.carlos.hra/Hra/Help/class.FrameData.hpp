#pragma once
#include "../../com.carlos.architecture/modules/class.ModulVykreslovania.hpp"

using namespace Architecture;

class FrameData {
private:
	ModulVykreslovania::In* vstup;
	float deltaTime;

public:
	FrameData();
	~FrameData();

	void setVstup(ModulVykreslovania::In* vstup);
	ControllerCommands getCommand();
	float getDeltaTime();
	void setDeltaTime(float f);
	cv::Mat getImage();
	cv::Mat getHorizont();
	bool hasVstup();

	friend ostream& operator<< (ostream& out, FrameData& object);
};