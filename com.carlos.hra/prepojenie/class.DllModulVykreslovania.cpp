#include <carlos_global.h>
#include "class.DllModulVykreslovania.hpp"
#include <iostream>
#include <gl/glew.h>
#include "..\Hra\Help\class.FrameData.hpp"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

using namespace Architecture;

cv::Mat dokresliHorizont(cv::Mat bg, cv::Mat horizont) {
	int a = bg.channels();
	int b = horizont.channels();

	for( int j= 0; j < bg.cols; j++ )
	{
		int pocet;
		pocet = 0;
		for( int i = 0; i < bg.rows; i++ )
		{
			if(horizont.at<uchar>(i,j) == 0){
				bg.at<Vec3b>(i,j)[0] = 0;
				bg.at<Vec3b>(i,j)[1] = 255;
				bg.at<Vec3b>(i,j)[2] = 0;
				pocet++;
				if(pocet == 2) break;
			} 
		}
	}

	return bg;
}

/** 
* Funkcia ma na vstupe 1 parameter a to je prijaty obrazok, stara sa o ulozenie do buffera
* Spusta sa v 2. vlakne !
* @param in - prijaty obrazok
* @return void 
*/
void DllModulVykreslovania::vykresliObrazokSRozsirenouRealitou(In in) 
{
	// Spracuj obrazok
	in.image.data = in.image.data.clone();
	in.horizont = in.horizont.clone();
	in.image.data = dokresliHorizont(in.image.data, in.horizont); // .clone()
	cv::flip(in.image.data, in.image.data, 0);

	// Posli obrazok dalej
	/*if(scene == NULL) {
	throw std::exception("scene is null\n");
	}*/
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
		cout << e.what();
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
		// Unrecoverable error, exit here.
		printf("SDL_Init failed: %s\n", SDL_GetError());
	}

	/*SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);*/

	window = SDL_CreateWindow("Carlos game", 100, 100, 600, 480, SDL_WINDOW_OPENGL);
	if (window == NULL) {
		throw std::exception("Failed to initialize SDL_CreateWindow\n");
	}
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (glewInit() != GLEW_OK) {
		throw std::exception("Failed to initialize GLEW\n");
	}

	/*AllocConsole();
	freopen("CONIN$", "r",stdin);
	freopen("CONOUT$", "w",stdout);
	freopen("CONOUT$", "w",stderr);*/
	printf("%s\n", glGetString( GL_VENDOR ));
	printf("%s\n", glGetString( GL_RENDERER ));
	printf("%s\n", glGetString( GL_VERSION   ));
	printf("%s\n", glGetString ( GL_SHADING_LANGUAGE_VERSION ));


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
