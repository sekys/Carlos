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

	/** 
	* Funkcia nema na vstupe ziadne parametre, sluzi na pocitanie casu a vytvorenie framu
	* Vola sa uz v druhom vlakne !!!
	* @see void init()
	* @return nil
	*/
	void render();

	SDL_Window* window; /**< Okno na hru */
	Scene scene; /**< Scena v okne */
	uint32_t oldTimeSinceStart;
	boost::atomic<bool> should_stop;

	log4cpp::Category* log; /**< Logovanie */

public:
	DllModulVykreslovania();

	/** 
	* Funkcia ma na vstupe 1 parameter a to je prijaty obrazok, stara sa o ulozenie do buffera
	* Spusta sa v 2. vlakne !
	* @param in - prijaty obrazok
	* @return void 
	*/
	void vykresliObrazokSRozsirenouRealitou(In* in);

	/** 
	* Funkcia ktora sa stara o inicializovanie kniznice openGl, nastavuje sa tu
	* vsetko potrebne od nazvu okna az po rozmery okna
	* @see void DllModulVykreslovania::init()
	* @return void
	*/
	void init();


	bool isThreaded();
	void doExit();
};