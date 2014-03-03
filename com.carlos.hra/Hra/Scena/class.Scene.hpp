#pragma once
/** @file class.Scene.hpp
* Trieda Scene ktora sa stara o zobrazenie modelu lietadla, textu, framu a ovladanie lietadla
*/

#include "..\Help\class.VisualController.hpp"
#include "..\Help\class.ResourceManager.hpp"
#include "..\Entity\class.Plain.hpp"
#include "..\Entity\class.World.hpp"
#include "StavyHry.h"
#include "opencv2/opencv.hpp"
#include "..\Help\class.ThreadSafeStack.hpp"
#include "../../../com.carlos.architecture/modules/class.ModulVykreslovania.hpp"
#include "..\Help\class.FrameData.hpp"

using namespace Architecture;

class Scene {
private:
	// Prepinanie stavov
	void prepniStavNaObrazovku();
	void prepniStavNaGameOver();
	void prepniStavNaScore();
	void prepniStavNaHrania();
	void havaroval();

	// Volanie stavu
	void stavHrania(FrameData* fDelta);
	void stavUvodnaObrazovka(FrameData* frame);
	void stavGameOver(FrameData* fDelta);
	void stavSkore(FrameData* fDelta);

	// Pomocne premenne
	float casPrejdenyNaGameOver;
	//unsigned char pressedKey; /**< premenna so stlacenou klavesou */
	Plain* plain; /**<  model lietadla */
	World* world; /**< svet */ 
	StavyHry aktualnyStav; /**< aktualny stav v akom sa hra nachadza */

	VisualController* visualController; 
	ResourceManager* resManager; 

	void delenieStavov(FrameData* fDelta);
	void setBackgroud(CTexture texture);
	void ziskajAktualnyVstup(FrameData* data);
	void nastavPozadieZoVstupu(cv::Mat& img);
public:
	Scene();
	~Scene();

	void release();
	void init();
	void frame(float fDelta);
	//void keyboard(unsigned char key, int x, int y);

	ThreadSafeStack<ModulVykreslovania::In*> zasobnikVstupov; /**< buffer prijatych snimkov */
};