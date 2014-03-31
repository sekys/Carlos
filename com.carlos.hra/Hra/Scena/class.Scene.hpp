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

	VisualController* mVisualController; 
	ResourceManager* mResManager;
	log4cpp::Category* log;
	TextAndPositionFormatter mFormatter;
	GameStateController* mStates;

	void setBackgroud(CTexture texture);
	void setBlackBackground();
	void nastavPozadieZoVstupu(cv::Mat& img);
	void ziskajAktualnyVstup(FrameData* data);

	uint getWindowWidth();
	uint getWindowHeight();

	Scene();
	~Scene();

	void release();
	void init();
	void frame(float fDelta);

	ThreadSafeStack<ModulVykreslovania::In*> zasobnikVstupov; /**< buffer prijatych snimkov */
};