#pragma once
/** @file class.Scene.hpp
* Trieda Scene ktora sa stara o zobrazenie modelu lietadla, textu, framu a ovladanie lietadla
*/
#include <carlos_global.h>
#include <log4cpp.h>

#include "..\Help\class.VisualController.hpp"
#include "..\Help\class.ResourceManager.hpp"
#include "..\Entity\class.Plane.hpp"
#include "..\Entity\class.World.hpp"
#include "class.GameStateController.hpp"
#include "opencv2/opencv.hpp"
#include "..\Help\class.ThreadSafeStack.hpp"
#include "..\Help\class.FrameData.hpp"
#include "..\Help\font.h"
#include "..\Help\class.TextAndPositionFormatter.h"


using namespace Architecture;

class Scene {
public:
	// Pomocne premenne
	Plane* mPlane; /**<  model lietadla */
	World* mWorld; /**< svet */
	cv::Mat blackBackground; /**< cierne pozadie */

	VisualController* mVisualController; 
	ResourceManager* mResManager;
	log4cpp::Category* log;
	TextAndPositionFormatter mFormatter;
	GameStateController* mStates; /**< aktualny stav */
	int aktualnaPozicia; /**< pozicia kde sa ma vykreslit obraz aby sa prekryl so scenou */

	/** 
	* Funkcia na nastavenie textury
	* @param texture 
	* @return void
	*/
	void setBackgroud(CTexture texture);

	/** 
	* Funkcia na nastavenie ciernej textury
	* @return void
	*/
	void setBlackBackground();

	/** 
	* Funkcia na nastavenie textury v inom formate
	* @param img 
	* @return void
	*/
	void setBackgroud(cv::Mat& img);

	/** 
	* Funkcia na ziskanie posledneho prijateho snimku zo zasobnika
	* @param data 
	* @return void
	*/
	void ziskajAktualnyVstup(FrameData* data);

	uint getWindowWidth();
	uint getWindowHeight();

	Scene();
	~Scene();

	/** 
	* Funkcia nema na vstupe ziadne parametre, stara sa o vymazanie vsetkeho
	* @see void DllModulVykreslovania::init()
	* @return void
	*/
	void release();

	/** 
	* Funkcia nema na vstupe ziadne parametre, iba nastavi scenu na inicializacne hodnoty
	* @param 
	* @return void
	*/
	void init();

	/** 
	* Funkcia ma na vstupe 1 parameter a to zmenu casu, stara sa vykreslenie 1 framu
	* @param fDelta - zmena casu
	* @see renderScene()
	* @return void
	*/
	void frame(float fDelta);

	ThreadSafeStack<ModulVykreslovania::In*> zasobnikVstupov; /**< buffer prijatych snimkov */
};