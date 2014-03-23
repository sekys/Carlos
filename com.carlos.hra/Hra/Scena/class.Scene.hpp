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
#include <log4cpp.h>

#include <map>
#include "..\Help\font.h"
#include "..\..\..\com.carlos.architecture\db\entities.hpp"
#include "..\..\com.carlos.architecture\db\class.DBService.hpp"
#include "..\Help\class.TextAndPositionFormatter.h"


using namespace Architecture;

class Scene {
private:
	// Prepinanie stavov
	void prepniStavNaObrazovku();
	void prepniStavNaGameOver();
	void prepniStavNaScore();
	void prepniStavNaHrania();
	void havaroval();

	void switchStateToTouristInfo();

	// Volanie stavu
	void stavHrania(FrameData* fDelta);
	void stavUvodnaObrazovka(FrameData* frame);
	void stavGameOver(FrameData* fDelta);
	void stavSkore(FrameData* fDelta);

	void stateTouristInfo(FrameData * frame);
	void showTouristInfo(DB::Object *, Point2f &);

	// Pomocne premenne
	float casPrejdenyNaGameOver;
	//unsigned char pressedKey; /**< premenna so stlacenou klavesou */
	Plain* plain; /**<  model lietadla */
	World* world; /**< svet */
	World* world2; /**< vacsi svet */
	StavyHry aktualnyStav; /**< aktualny stav v akom sa hra nachadza */

	VisualController* visualController; 
	ResourceManager* resManager;
	log4cpp::Category* log;
	TextAndPositionFormatter formatter;

	void delenieStavov(FrameData* fDelta);
	void setBackgroud(CTexture texture);
	void ziskajAktualnyVstup(FrameData* data);
	void nastavPozadieZoVstupu(cv::Mat& img);

	uint getWindowWidth();
	uint getWindowHeight();

	map<uint, DB::Object *> objectInfos;
public:
	int typ_lietadla;
	std::clock_t start;
	double duration;

	Scene();
	~Scene();

	void release();
	void init();
	void frame(float fDelta);


	//void keyboard(unsigned char key, int x, int y);

	ThreadSafeStack<ModulVykreslovania::In*> zasobnikVstupov; /**< buffer prijatych snimkov */
};