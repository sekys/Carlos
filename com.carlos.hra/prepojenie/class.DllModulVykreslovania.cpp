#include <carlos_global.h>
#include "class.DllModulVykreslovania.hpp"
#include <iostream>
#include <gl/glew.h>
#include <gl/wglew.h>
#include "..\Hra\Help\class.FrameData.hpp"
#include <log4cpp/PropertyConfigurator.hh>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

using namespace Architecture;

DllModulVykreslovania::DllModulVykreslovania() {
	log = CREATE_LOG4CPP();
	window = NULL;
}

/** 
* Funkcia ma na vstupe 1 parameter a to je prijaty obrazok, stara sa o ulozenie do buffera
* Spusta sa v 2. vlakne !
* @param in - prijaty obrazok
* @return void 
*/
void DllModulVykreslovania::vykresliObrazokSRozsirenouRealitou(In* in) 
{
	// Spracuj obrazok
	//cv::Mat black(480, 640, CV_8UC3, Scalar(0,0,0));
	//in->image.data = in->image.data.clone();
	//in->image.data =black; 
	in->horizont = in->horizont.clone();
	//dokresliHorizont(in->image.data, in->horizont);
	cv::flip(in->image.data, in->image.data, 0);

	// Posli obrazok dalej
	scene.zasobnikVstupov.push(in);	
}

/** 
* Funkcia nema na vstupe ziadne parametre, sluzi na pocitanie casu a vytvorenie framu
* Vola sa uz v druhom vlakne !!!
* @see void openGLInit()
* @return nil
*/
void DllModulVykreslovania::render() {
	uint32_t timeSinceStart = (float)SDL_GetTicks(); ;
	int deltaTime;
	float fDelta;
	if(oldTimeSinceStart == 0) {
		fDelta = 0.0;
	} else {
		deltaTime = timeSinceStart - oldTimeSinceStart;
		fDelta = (float) deltaTime / 1000.f;
	}
	oldTimeSinceStart = timeSinceStart;
	try {
		scene.frame(fDelta);
	} catch (std::exception e) {
		if(log != NULL) {
			log->debugStream() << e.what();
		}
	} 
}

/** 
* Funkcia ktora sa stara o inicializovanie kniznice openGl, nastavuje sa tu
* vsetko potrebne od nazvu okna az po rozmery okna
* @see void DllModulVykreslovania::init()
* @return void
*/
void DllModulVykreslovania::init() {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		throw std::exception(SDL_GetError());
	}
	/*
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	*/
	//window = SDL_CreateWindow("Carlos App", 100, 100, 640, 480, SDL_WINDOW_OPENGL);
	window = SDL_CreateWindow("Carlos App", 100, 100, 1024, 480, SDL_WINDOW_OPENGL);
	//window = SDL_CreateWindow("Carlos game", 100, 100, 1024, 768, SDL_WINDOW_OPENGL);
	if (window == NULL) {
		throw std::exception("Failed to initialize SDL_CreateWindow");
	}
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (!context){
		throw std::exception("Failed to initialize SDL_GL_CreateContext");
	}
	if (glewInit() != GLEW_OK) {
		throw std::exception("Failed to initialize GLEW");
	}


	if(log != NULL) {
		log->debugStream() << "Vendor: " <<  glGetString( GL_VENDOR );
		log->debugStream() << "Renderer: " << glGetString( GL_RENDERER );
		log->debugStream() << "Version: " << glGetString( GL_VERSION   );
		log->debugStream() << "Shading ver: " << glGetString ( GL_SHADING_LANGUAGE_VERSION );
	}

	should_stop = false;
	oldTimeSinceStart = 0;
	//scene = new Scene();
	scene.init();

	SDL_Event windowEvent;
	while (true)
	{
		if(should_stop) break;
		if (SDL_PollEvent(&windowEvent)) {
			if (windowEvent.type == SDL_QUIT) break;
		}
		render();
		SDL_GL_SwapWindow(window);
	}

	if(log != NULL) {
		log->debugStream() << "Vypinam cyklus hry";
	}

	scene.release();
	SDL_GL_DeleteContext(context);
	SDL_Quit();
}

bool DllModulVykreslovania::isThreaded() { 
	return true; 
}

void DllModulVykreslovania::doExit() {
	should_stop = true;
}
