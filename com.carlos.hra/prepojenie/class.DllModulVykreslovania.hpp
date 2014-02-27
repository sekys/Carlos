#pragma once
#include <carlos_global.h>
#include "../../com.carlos.architecture/modules/class.ModulVykreslovania.hpp"
#include "../Hra/Scena/class.Scene.hpp"
#include <boost/atomic.hpp>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#define SDL_OPENGLVC9	 2 /**< Create an OpenGL rendering context */
#define SDL_FULLSCREENVC9	2147483648
#define SDL_DOUBLEBUFVC9	1073741824
#define SDL_NOFRAMEVC9	 32
#define SDL_RESIZEBLEVC9	16

class DllModulVykreslovania : public Architecture::ModulVykreslovania {
private:
	void render();

	SDL_Window* window;
	Scene scene;
	uint32_t oldTimeSinceStart;
	boost::atomic<bool> should_stop;

public:
	/// Spusta sa v 2. vlakne !
	void vykresliObrazokSRozsirenouRealitou(In in);
	void init();
	bool isThreaded();
	void doExit();
};