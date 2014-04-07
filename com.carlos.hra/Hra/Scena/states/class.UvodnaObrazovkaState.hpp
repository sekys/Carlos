#pragma once

#include "class.IGameState.hpp"
#include "../class.Scene.hpp"

class UvodnaObrazovkaState : public IGameState {
public:
	UvodnaObrazovkaState() : IGameState(GameStates::UVODNA_OBRAZOVKA) { 
 
	}

	virtual void switchOn(IGameState* predchodca) {
		mScene->setBackgroud(mScene->mResManager->bgUvod);
		mScene->zasobnikVstupov.clear();
	}

	virtual void frame(FrameData* frame) {	
		mScene->zasobnikVstupov.clear();

		if(frame->getCommand() == ControllerCommands::UP) { // Ak sa dotkne obrazovky zacina sa hra
			mScene->mStates->switchTo(GameStates::HRAJE_HRU);
		} else if (frame->getCommand() == ControllerCommands::TOURIST_INFO) { // Prepnutie do stavu turistickeho infa
			mScene->mStates->switchTo(GameStates::TOURIST_INFO);
		}
	}
};