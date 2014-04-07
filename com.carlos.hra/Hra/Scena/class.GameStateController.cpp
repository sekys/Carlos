#include "class.GameStateController.hpp"
#include "class.Scene.hpp"
#include "states\enum.GameStates.hpp"
#include "states\class.GameOverState.hpp"
#include "states\class.UvodnaObrazovkaState.hpp"
#include "states\class.PlayingGameState.hpp"
#include "states\class.SkoreObrazovkaState.hpp"
#include "states\class.TouristInfoState.hpp"

GameStateController::GameStateController(Scene* scene) {
	log = CREATE_LOG4CPP();	
	mActual = NULL;
	mScene = scene;
}
GameStateController::~GameStateController() {
	releaseAll();
	mActual = NULL;
	mScene = NULL;
}

void GameStateController::install(IGameState* state) {
	bm.insert( bm_type::value_type(state->mType, state) );
	state->mScene = mScene;
}

void GameStateController::installStates() {
	install( new GameOverState() );
	install( new UvodnaObrazovkaState() );
	install( new PlayingGameState() );
	install( new SkoreObrazovkaState() );
	install( new TouristInfoState() );
}

void GameStateController::loadAll() {
	this->installStates();
	bm_type::left_const_iterator it = bm.left.begin();
	for (;it != bm.left.end(); ++it) {
		it->second->load();
	}
	if(log != NULL) {
		log->debugStream() << "Nacital som vsetky sceny.";
	}
}
void GameStateController::releaseAll() {
	if(log != NULL) {
		log->debugStream() << "Spustil som realeaseAll.";
	}
	bm_type::left_const_iterator it = bm.left.begin();
	for (;it != bm.left.end(); ++it) {
		IGameState* state = it->second;
		state->release();
		delete state;

	}
	bm.clear();

	if(log != NULL) {
		log->debugStream() << "Ukoncil som realeaseAll.";
	}
}
void GameStateController::switchTo(GameStates newState) {
	bm_type::left_const_iterator it = bm.left.find(newState);
	if(it == bm.left.end() ) {
		throw new exception("state not installed");
	}

	// Vypinam stary stav
	if(mActual != NULL) {
		if(log != NULL) {
			log->debugStream() << "Spustam switch off na " << mActual->mType;
		}
		mActual->switchOff();
	}

	// Zapinam novy stav
	IGameState* buduci = it->second;
	if(log != NULL) {
		log->debugStream() << "Switch on na " << buduci->mType;
	}
	buduci->switchOn(mActual);

	// Nastav aktualne hodnoty
	mActual = buduci;
	mActual->mRunningTime = 0;
}

void GameStateController::frame(FrameData* frame) {
	mActual->mRunningTime += frame->getDeltaTime();
	mActual->frame(frame);
}

IGameState* GameStateController::getActual() {
	return mActual;
}