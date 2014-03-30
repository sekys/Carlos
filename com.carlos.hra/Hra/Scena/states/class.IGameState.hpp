#pragma once

#include "..\..\Help\class.FrameData.hpp"
#include "enum.GameStates.hpp"

class Scene;
class GameStateController;

// Interface pre stav hry
class IGameState {
protected:
	Scene* mScene; // cez toto mozte pristupovat k vseobecnym zdrojom;
	float mRunningTime; // celkovy cas kolko bezi akutalny stav;
	const GameStates mType;

public:
	IGameState(GameStates type) : mType(type) { 
		mScene = NULL;
	}
	~IGameState() { 
		mScene = NULL; 
	}

	virtual void load() {} // hra sa spusta, tu je priestor pre nacitavanie specifickych zdrojov
	virtual void switchOn(IGameState* predchodca) {} // stav bol zapnuty, z ineho stavu (predchodca) sa preslo sem, prechodca mzoe byt null
	virtual void switchOff() {} // stav je prepnuty prec
	virtual void frame(FrameData* fDelta) {} // v tejto metoda sa ma renderovat 3D scena
	// neimplementovane virtual void hudFrame(FrameData* fDelta) {} // v tejto metode sa ma renderov 2D HUD, teda vsetko nad scenou
	virtual void release() {} // vsetky zdroje, ktore ste alokovali tu vymazte

	friend class GameStateController;
	GameStates getType() { return mType; }
	double getCasBehu() { return mRunningTime; }
};