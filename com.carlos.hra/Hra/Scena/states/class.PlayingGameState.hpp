#pragma once

#include "class.IGameState.hpp"
#include "../class.Scene.hpp"
#include <log4cpp.h>

class PlayingGameState : public IGameState {
private:
	bool otestujHorizontCiSaDotykaLietadla(cv::Mat horizont, Plane* plain);
	void dokresliHorizont(cv::Mat& bg, cv::Mat& horizont);
	void eventHavaroval();

	log4cpp::Category* log;

public:
	PlayingGameState() : IGameState(GameStates::HRAJE_HRU) { 
		log = CREATE_LOG4CPP();	
	}

	virtual void switchOn(IGameState* predchodca);
	virtual void frame(FrameData* frame);
};