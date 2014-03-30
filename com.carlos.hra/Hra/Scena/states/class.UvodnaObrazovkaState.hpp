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

		// Ak sa dotkne obrazovky zacina sa hra
		if(frame->getCommand() == ControllerCommands::UP) {
			mScene->mStates->switchTo(HRAJE_HRU);
		}
	}
};