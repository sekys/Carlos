#pragma once
#include <carlos_global.h>
#include "../../com.carlos.architecture/modules/class.ModulVykreslovania.hpp"
#include "../Hra/Scena/class.Scene.hpp"
#include <boost/atomic.hpp>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_syswm.h>
#include <log4cpp.h>

class DllModulVykreslovania : public Architecture::ModulVykreslovania {
private:
	void render();

	SDL_Window* window;
	Scene scene;
	uint32_t oldTimeSinceStart;
	boost::atomic<bool> should_stop;

	log4cpp::Category* log;

public:
	DllModulVykreslovania();

	/// Spusta sa v 2. vlakne !
	void vykresliObrazokSRozsirenouRealitou(In* in);
	void init();
	bool isThreaded();
	void doExit();
};