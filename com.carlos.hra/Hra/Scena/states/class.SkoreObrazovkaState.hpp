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

		double runningTime = this->getCasBehu();

		///Ak sa dotkne obrazovky zacina sa hra
		if(frame->getCommand() == ControllerCommands::UP && runningTime > 2.0) {
			mScene->mStates->switchTo(HRAJE_HRU);
		} else if (frame->getCommand() == ControllerCommands::TOURIST_INFO && runningTime > 2.0) {
			mScene->mStates->switchTo(GameStates::TOURIST_INFO);
			return;
		} else if (runningTime > 30.0) { // Cas vacsi ako 30s
			mScene->mStates->switchTo(GameStates::TOURIST_INFO);
		}
	}
};