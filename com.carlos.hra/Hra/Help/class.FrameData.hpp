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
	int getpozicia();
	float getDeltaTime();
	void setDeltaTime(float f);
	cv::Mat getImage();
	cv::Mat getHorizont();
	bool hasVstup();
	ModulVykreslovania::In* getVstup();

	friend ostringstream& operator<< (ostringstream& out, FrameData* object);
};