#pragma once
#include <log4cpp.h>
#include <boost/bimap.hpp>
#include "states\enum.GameStates.hpp"
#include "states\class.IGameState.hpp"

class Scene;

// Trieda kontroluje stavy, prepina medzi nimy
class GameStateController {
protected:
	log4cpp::Category* log;

	typedef boost::bimap<GameStates, IGameState*> bm_type;
	bm_type bm;
	IGameState* mActual;
	Scene* mScene;


	void install(IGameState* state);
	void installStates();
	void releaseAll();

public:
	GameStateController(Scene*);
	~GameStateController();

	void switchTo(GameStates state);
	void loadAll();
	void frame(FrameData* frame);
	IGameState* getActual();
};