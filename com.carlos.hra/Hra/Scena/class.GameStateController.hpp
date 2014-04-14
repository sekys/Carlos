#pragma once
#include <log4cpp.h>
#include <boost/bimap.hpp>
#include "states\enum.GameStates.hpp"
#include "states\class.IGameState.hpp"

class Scene;

/** @file class.Plane.cpp
* Trieda ktora kontroluje stavy a prepina medzi nimi
*/
class GameStateController {
protected:
	log4cpp::Category* log; 

	typedef boost::bimap<GameStates, IGameState*> bm_type;
	bm_type bm;
	IGameState* mActual; /**< Aktualny stav */
	Scene* mScene; /**< Scena */


	void install(IGameState* state);

	/** 
	* Funkcia nema na vstupe ziadne parametre, sluzi na nainstalovanie vsetkych druhov stavov
	* @see void GameStateController::loadAll()
	* @return void
	*/
	void installStates(); 

	/** 
	* Funkcia nema na vstupe ziadne parametre, sluzi na realease vsetkych stavov
	* @see GameStateController::~GameStateController()
	* @return void
	*/
	void releaseAll();

public:
	GameStateController(Scene*);
	~GameStateController();

	/** 
	* Funkcia ma na to vstupe jeden parameter a to novy - buduci stav, stara sa o prepinanie medzi stavmi
	* @param state - buduci stav
	* @see virtual void frame(FrameData* frame)
	* @see void PlayingGameState::eventHavaroval(
	* @see void PlayingGameState::frame(FrameData* frame) 
	* @return void
	*/
	void switchTo(GameStates state);

	/** 
	* Funkcia nema na vstupe ziadny parameter, stara sa o nacitanie vsetkeho
	* @see void Scene::init()
	* @return void
	*/
	void loadAll();

	void frame(FrameData* frame);
	
	/** 
	* Funkcia nema na vstupe ziadny parameter, vrati aktualny stav
	* @return IGameState
	*/
	IGameState* getActual();
};