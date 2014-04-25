#pragma once

#include "class.IGameState.hpp"
#include "../class.Scene.hpp"

template <class T>
string convertToStr(T *var) {
	ostringstream ss;
	ss << *var;
	return ss.str();
}

class GameOverState : public IGameState {
private :
	double skore;

public:
	GameOverState() : IGameState(GameStates::OBRAZOVKA_PREHRAL) { 
		skore = 0;
	}

	float pozicia_skore; /**< Pozicia kde sa ma vypisat skore */
	
	/** 
	* Funkcia ma na vstupe jeden parameter, stara sa o nastavenie pozadia na gameover
	* @param predchodca
	* @return void
	*/
	virtual void switchOn(IGameState* predchodca) {
		if(predchodca != NULL && predchodca->getType() == GameStates::HRAJE_HRU) {
			skore = predchodca->getCasBehu();
		}
		
		pozicia_skore =  -0.35 + (mScene->aktualnaPozicia/100.0 * 0.3125);
		mScene->setBackgroud(mScene->mResManager->bgGameOver);
		mScene->zasobnikVstupov.clear();
	}

	/** 
	* Funkcia ma na vstupe jeden parameter, stara sa o prepinanie do dalsich stavov ak je v stave gameover - po 5s sa prepne na skore
	* @param predchodca
	* @return void
	*/
	virtual void frame(FrameData* frame) {	
		mScene->zasobnikVstupov.clear();

		// Hud
		glUseProgram(0);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glLoadIdentity();
		
		glTranslatef(pozicia_skore, -0.4f ,0.0f);
		glColor3f(0.0f, 0.f, 0.f);
		glRasterPos2f(0.0f, 0.0f); 
		string str = "Aktualny cas letu: \n" + convertToStr<double>(&skore);
		glPrint( str.c_str());
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);

		if(this->getCasBehu() > 5.0) {
			/// presiel cas a zmeni sa stav
			mScene->mStates->switchTo(GameStates::OBRAZOVKA_SKORE);
		}
	}
};