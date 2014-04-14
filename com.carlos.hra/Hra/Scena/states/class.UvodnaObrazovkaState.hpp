#pragma once

#include "class.IGameState.hpp"
#include "../class.Scene.hpp"

class UvodnaObrazovkaState : public IGameState {
public:
	UvodnaObrazovkaState() : IGameState(GameStates::UVODNA_OBRAZOVKA) { 
	}

	/** 
	* Funkcia ma na vstupe jeden parameter, stara sa o nastavenie textury na uvodnu obrazovku 
	* @param predchodca
	* @return void
	*/
	virtual void switchOn(IGameState* predchodca) {
		mScene->setBackgroud(mScene->mResManager->bgUvod);
		mScene->zasobnikVstupov.clear();
	}

	/** 
	* Funkcia ma na vstupe aktualny prichadzajuci frame, stara sa o prepinanie do dalsich stavov ak je v stave uvodna obrazovka
	* @param frame
	* @return void
	*/
	virtual void frame(FrameData* frame) {	
		mScene->zasobnikVstupov.clear();

		if(frame->getCommand() == ControllerCommands::UP || frame->getCommand() == ControllerCommands::TAP) { // Ak sa dotkne obrazovky zacina sa hra
			mScene->mStates->switchTo(GameStates::HRAJE_HRU);
		} else if (frame->getCommand() == ControllerCommands::TOURIST_INFO) { // Prepnutie do stavu turistickeho infa
			mScene->mStates->switchTo(GameStates::TOURIST_INFO);
		}
	}
};