#pragma once

#include "class.IGameState.hpp"
#include "../class.Scene.hpp"

class ChooseDialogState : public IGameState {
public:
	ChooseDialogState() : IGameState(GameStates::CHOOSE_DIALOG) { 

	}

	virtual void switchOn(IGameState* predchodca) {
		mScene->setBackgroud(mScene->mResManager->bgChooseDialog);
		mScene->zasobnikVstupov.clear();
	}

	virtual void frame(FrameData* frame) {	
		mScene->zasobnikVstupov.clear();

		if (frame->getCommand() == ControllerCommands::GAME) {
			mScene->mStates->switchTo(GameStates::UVODNA_OBRAZOVKA);
		} else if (frame->getCommand() == ControllerCommands::TOURIST_INFO) {
			mScene->mStates->switchTo(GameStates::TOURIST_INFO);
		}
	}
};