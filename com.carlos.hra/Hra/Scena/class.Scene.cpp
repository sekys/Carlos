/** @file class.Scene.cpp
* Trieda ktora sa stara o zobrazenie samotnej sceny, po jednotlivych framoch
*/
#include <carlos_global.h>
#include "class.Scene.hpp"
#include "..\Help\font.h"
#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdarg.h>
#include <stdexcept>

void testGL() {
	if(CARLOS_DEBUG_OPENGL) {
		GLenum error = glGetError();
		if( error != GL_NO_ERROR ) {
			const GLubyte * sprava =  gluErrorString( error );
			throw new exception("Error initializing OpenGL! ");
		}
	}
}

Scene::Scene()  {
	log = CREATE_LOG4CPP();
	plain = new Plain(glm::vec2(10.0, 10.0), typ_lietadla);
	world = new World(glm::vec2(320.0, 240.0));
}

Scene::~Scene() {
	// Vsetko by malo byt uz aktualne uvolnene
	SAFE_DELETE(plain);
	SAFE_DELETE(world);
	killFont();
}
/** 
* Funkcia nema na vstupe ziadne parametre, iba nastavi scenu na inicializacne hodnoty
* @param 
* @return void
*/
void Scene::init() {
	testGL();
	if(log != NULL) {
		log->debugStream() << "Spustam Scene::init()";
	}
	visualController  = new VisualController();
	resManager = new ResourceManager();

	srand(time(NULL));
	int randNum = (rand() % 3) + 1;
	typ_lietadla = randNum;
	resManager->load(randNum);
	testGL();
	visualController->load(resManager->shaders);
	testGL();
	//switchStateToTouristInfo();
	prepniStavNaObrazovku();
	testGL();

	testGL();
	glEnable(GL_DEPTH_TEST);

	buildFont();

	if(log != NULL) {
		log->debugStream() << "Koncim Scene::init()";
	}
	testGL();
}


void Scene::release() {
	zasobnikVstupov.clear();
	SAFE_DELETE( visualController );
	SAFE_DELETE( resManager );
	SAFE_DELETE( plain );
	SAFE_DELETE( world );
}

/** 
* Funkcia ma na vstupe 1 parameter a to zmenu casu, stara sa vykreslenie 1 framu
* @param fDelta - zmena casu
* @see renderScene()
* @return void
*/
void Scene::frame(float fDelta) {
	testGL();
	glClearColor(0,0,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	testGL();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	testGL();

	FrameData frame;
	frame.setDeltaTime(fDelta);
	ziskajAktualnyVstup(&frame);
	testGL();
	visualController->setPerspektive();
	testGL();
	visualController->renderObject(resManager->square, world->getMatrix());
	testGL();
	delenieStavov(&frame);
	testGL();
	glFlush();
	testGL();
}


/** 
* Funkcia na overenie v akom stave sa nachadza aktualny frame, ma na vstupe jeden parameter a to aktualny frame
* @param FrameData* frame - aktualny frame 
* @return void
*/
void Scene::delenieStavov(FrameData* frame) {
	switch(aktualnyStav) {
	case StavyHry::HRAJE_HRU: {
		stavHrania(frame);
		break;
							  }
	case StavyHry::OBRAZOVKA_PREHRAL: {
		stavGameOver(frame);
		break;
									  }
	case StavyHry::OBRAZOVKA_SKORE: {
		stavSkore(frame);
		break;
									}
	case StavyHry::UVODNA_OBRAZOVKA: {
		stavUvodnaObrazovka(frame);
		break;
									 }
	case StavyHry::TOURIST_INFO: {
		stateTouristInfo(frame);
		break;
								 }

	default: {
		throw std::exception("Neocakavany stav");
			 }
	}
}

void Scene::ziskajAktualnyVstup(FrameData* frame) {
	try {
		frame->setVstup( zasobnikVstupov.poll() );
		if(log != NULL) {
			log->debugStream() << "Hra, prijmam  vstupy " << frame;
		}
	} catch (const std::out_of_range& oor) {
		// Zasobnik je prazdny, ponechaj tam staru texturu do kedy nepride obrazok
	}
}

/** 
* Funkcia na nastavenie textury
* @param nil 
* @see load()
* @return void
*/
void Scene::setBackgroud(CTexture texture) {
	CTexture actualTex = resManager->square.getTexture();
	if(actualTex.flagDelete) {
		actualTex.releaseTexture();
	}
	resManager->square.setTexture(texture);
}

uint Scene::getWindowWidth() {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	return viewport[2];
}

uint Scene::getWindowHeight() {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	return viewport[3];
}