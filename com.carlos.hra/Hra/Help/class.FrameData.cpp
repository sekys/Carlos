#include "class.FrameData.hpp"

FrameData::FrameData() {
	vstup = NULL;
}
FrameData::~FrameData() {
	SAFE_DELETE(vstup);
}
void FrameData::setVstup(ModulVykreslovania::In* vstup) {
	this->vstup = vstup;
}

ControllerCommands FrameData::getCommand() {
	if(vstup == NULL) return ControllerCommands::NO_ACTION;
	return vstup->command;
}

float FrameData::getDeltaTime() {
	return deltaTime;
}
void FrameData::setDeltaTime(float f) {
	deltaTime = f;
}

ostringstream& operator<< (ostringstream& out, FrameData* object) {
	out << "FrameData(";
	if(object->vstup == NULL) {
		out << "vstup: NULL, ";
	} else {
		out << "vstup: ( " << *(object->vstup) << " ), ";
	}
	out << "deltaTime: " << object->deltaTime << ", ";
	out << "));";
	return out;
}
/*
cv::Mat FrameData::getImage() {
	return vstup->image.data;
}*/
cv::Mat FrameData::getHorizont() {
	return vstup->horizont;
}
bool FrameData::hasVstup() {
	return vstup != NULL;
}

int FrameData::getpozicia(){

	if (vstup->position*100 < -100 || vstup->position*100 > 100){
		//out << "Zla poslana pozicia: " << vstup->position*100;
	return 0;
	}
	return vstup->position*100;
}

ModulVykreslovania::In* FrameData::getVstup() {
	return vstup;
}

