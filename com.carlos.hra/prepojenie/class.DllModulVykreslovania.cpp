#include <carlos_global.h>
#include "class.DllModulVykreslovania.hpp"
#include <iostream>
#include <gl/glew.h>
#include <gl/wglew.h>
#include "..\Hra\Help\class.FrameData.hpp"


#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

using namespace Architecture;

void dokresliHorizont(cv::Mat& bg, cv::Mat& horizont) {
	//int a = bg.channels();
	//int b = horizont.channels();
	
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
	cv::Mat black(480, 640, CV_8UC3, Scalar(0,0,0));
	//in->image.data = in->image.data.clone();
	in->image.data =black; 
	in->horizont = in->horizont.clone();
	dokresliHorizont(in->image.data, in->horizont);
	cv::flip(in->image.data, in->image.data, 0);

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
		throw std::exception(SDL_GetError());
	}
	/*
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	*/
	window = SDL_CreateWindow("Carlos game", 100, 100, 600, 480, SDL_WINDOW_OPENGL);
	if (window == NULL) {
		throw std::exception("Failed to initialize SDL_CreateWindow\n");
	}
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (!context){
		throw std::exception("Failed to initialize SDL_GL_CreateContext\n");
	}
	if (glewInit() != GLEW_OK) {
		throw std::exception("Failed to initialize GLEW\n");
	}
	/*
	SDL_SysWMinfo sdlinfo;
	SDL_version sdlver;
	SDL_VERSION(&sdlver);
	sdlinfo.version = sdlver;
	SDL_GetWindowWMInfo(window, &sdlinfo);

	HGLRC hRC;
	HDC hDC;
	hDC = GetDC( sdlinfo.info.win.window );
	PIXELFORMATDESCRIPTOR pfd;

	const int iPixelFormatAttribList[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0 // End of attributes list
	};
	int iContextAttribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0 // End of attributes list
	};

	int iPixelFormat, iNumFormats;
	wglChoosePixelFormatARB(hDC, iPixelFormatAttribList, NULL, 1, &iPixelFormat, (UINT*)&iNumFormats);

	// PFD seems to be only redundant parameter now
	if(!SetPixelFormat(hDC, iPixelFormat, &pfd)) {
		throw std::exception("Failed to initialize SetPixelFormat\n");
	}

	hRC = wglCreateContextAttribsARB(hDC, 0, iContextAttribs);
	// If everything went OK
	if(hRC) wglMakeCurrent(hDC, hRC);
	else {
		throw std::exception("Failed to initialize wglMakeCurrent\n");
	}
	*/
	cout << "Vendor: " <<  glGetString( GL_VENDOR ) << "\n";
	cout << "Renderer: " << glGetString( GL_RENDERER )  << "\n";
	cout << "Version: " << glGetString( GL_VERSION   )  << "\n";
	cout << "Shading ver: " << glGetString ( GL_SHADING_LANGUAGE_VERSION )  << "\n";

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
