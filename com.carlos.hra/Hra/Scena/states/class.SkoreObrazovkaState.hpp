#pragma once

#include "class.IGameState.hpp"
#include "../class.Scene.hpp"

class SkoreObrazovkaState : public IGameState {
public:
	SkoreObrazovkaState() : IGameState(GameStates::OBRAZOVKA_SKORE) { 
 
	}

	virtual void switchOn(IGameState* predchodca) {
		mScene->setBackgroud(mScene->mResManager->bgScore);
		mScene->zasobnikVstupov.clear();
	}

	virtual void frame(FrameData* frame) {	
		mScene->zasobnikVstupov.clear();

		///Ak sa dotkne obrazovky zacina sa hra
		if(frame->getCommand() == ControllerCommands::UP) {
			mScene->mStates->switchTo(HRAJE_HRU);
		} else if (frame->getCommand() == ControllerCommands::TOURIST_INFO) {
			mScene->mStates->switchTo(GameStates::TOURIST_INFO);
			return;
		}
	}
};