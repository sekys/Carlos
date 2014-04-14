#pragma once

#include "class.IGameState.hpp"
#include "../class.Scene.hpp"
#include <log4cpp.h>

class PlayingGameState : public IGameState {
private:

	/** 
	* Funkcia ma na vstupe tri parametre, stara sa o otestovanie ci sa lietadlo dotklo horizontu
	* @param plain - lietadlo na scene
	* @param horizont - detegovany horizont ktory sa pouzije na otestovanie
	* @param pozicia - aktualna pozicia okna
	* @see void PlayingGameState::frame(FrameData* frame)
	* @return bool
	*/
	bool otestujHorizontCiSaDotykaLietadla(cv::Mat horizont, Plane* plain, int pozicia);
	
	/** 
	* Funkcia ma na vstupe dva parametre, stara sa o dokreslenie horizontu na obrazovku
	* @param bg - poziadie ktore sa ma vykreslit
	* @param horizont - detegovany horizont ktory sa dokresli
	* @see void PlayingGameState::frame(FrameData* frame)
	* @return void
	*/
	void dokresliHorizont(cv::Mat& bg, cv::Mat& horizont);
	
	/** 
	* Funkcia nema na vstupe ziadny parameter, ak narazi do horizontu tak sa stav prepne na stav obrazovka prehral
	* @see void PlayingGameState::frame(FrameData* frame)
	* @return void
	*/
	void eventHavaroval();

	log4cpp::Category* log;

public:
	PlayingGameState() : IGameState(GameStates::HRAJE_HRU) { 
		log = CREATE_LOG4CPP();	
	}
	/** 
	* Funkcia ma na vstupe jeden parameter, stara sa o nastavenie pozicie lietadla na uvodnu poziciu
	* @param predchodca
	* @return void
	*/
	virtual void switchOn(IGameState* predchodca);

	/** 
	* Funkcia ma na vstupe jeden parameter, stara sa o prepinanie do dalsich stavov ak je v stave hrania hry lietadlo
	* @param predchodca
	* @return void
	*/
	virtual void frame(FrameData* frame);
};