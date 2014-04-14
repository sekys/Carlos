#pragma once

#include "class.IGameState.hpp"
#include "../class.Scene.hpp"

class SkoreObrazovkaState : public IGameState {
public:
	SkoreObrazovkaState() : IGameState(GameStates::OBRAZOVKA_SKORE) { 
 
	}
	/** 
	* Funkcia ma na vstupe jeden parameter, stara sa o nastavenie textury na skore
	* @param predchodca
	* @return void
	*/
	virtual void switchOn(IGameState* predchodca) {
		mScene->setBackgroud(mScene->mResManager->bgScore);
		mScene->zasobnikVstupov.clear();
	}

	/** 
	* Funkcia ma na vstupe aktualny prichadzajuci frame, stara sa o prepinanie do dalsich stavov ak je v stave skore
	* @param frame
	* @return void
	*/
	virtual void frame(FrameData* frame) {	
		mScene->zasobnikVstupov.clear();

		double runningTime = this->getCasBehu();

		///Ak sa dotkne obrazovky zacina sa hra
		if((frame->getCommand() == ControllerCommands::UP || frame->getCommand() == ControllerCommands::TAP) && runningTime > 2.0) {
			mScene->mStates->switchTo(HRAJE_HRU);
		} else if (frame->getCommand() == ControllerCommands::TOURIST_INFO && runningTime > 2.0) {
			mScene->mStates->switchTo(GameStates::TOURIST_INFO);
			return;
		} else if (runningTime > 30.0) { // Cas vacsi ako 30s
			mScene->mStates->switchTo(GameStates::TOURIST_INFO);
		}
	}
};