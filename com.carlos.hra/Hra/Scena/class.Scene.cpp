/** @file class.Scene.cpp
* Trieda ktora sa stara o zobrazenie samotnej sceny, po jednotlivych framoch
*/
#include <carlos_global.h>
#include "class.Scene.hpp"
#include "..\Help\font.h"
#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Metoda pre potreby debugovania OpengGl
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
}

Scene::~Scene() {

}
/** 
* Funkcia nema na vstupe ziadne parametre, iba nastavi scenu na inicializacne hodnoty
* @param 
* @return void
*/
void Scene::init() {
	// Komentar
	testGL();
	if(log != NULL) {
		log->debugStream() << "Spustam Scene::init()";
	}

	// Nacitaj controllery
	srand(time(NULL));
	mVisualController  = new VisualController();
	mResManager = new ResourceManager();
	mStates = new GameStateController(this);

	// Nacitaj objekty
	int typ_lietadla = (rand() % 3) + 1 ;
	mPlane = new Plane(glm::vec2(10.0, 10.0), typ_lietadla);
	mWorld = new World(glm::vec2(320.0, 240.0)); //640x480
	mResManager->load(typ_lietadla);
	testGL();
	mVisualController->load(mResManager->shaders);
	mStates->loadAll();
	testGL();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	buildFont();

	// Prepni na uvodny stav
	mStates->switchTo(UVODNA_OBRAZOVKA);

	if(log != NULL) {
		log->debugStream() << "Koncim Scene::init()";
	}
	testGL();
}


void Scene::release() {
	if(log != NULL) {
		log->debugStream() << "Spustil som Scene::release.";
	}
	zasobnikVstupov.clear();
	SAFE_DELETE( mStates );
	SAFE_DELETE( mVisualController );
	SAFE_DELETE( mResManager );
	SAFE_DELETE( mPlane );
	SAFE_DELETE( mWorld );
	killFont();
	if(log != NULL) {
		log->debugStream() << "Ukoncil som Scene::release.";
	}
}

/** 
* Funkcia ma na vstupe 1 parameter a to zmenu casu, stara sa vykreslenie 1 framu
* @param fDelta - zmena casu
* @see renderScene()
* @return void
*/
void Scene::frame(float fDelta) {
	testGL();
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	testGL();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	testGL();

	FrameData frame;
	frame.setDeltaTime(fDelta);
	ziskajAktualnyVstup(&frame);
	mStates->frame(&frame);
	testGL();
	mVisualController->setPerspektive();
	testGL();

	//Tu potrebujem poziciu vzdy
	int aktualnaPozicia = 0; // Neviem, naco to je dobre ... ked Svet ma POZICIU dva krat ... 
	mVisualController->renderObject(mResManager->square, mWorld->getMatrix(aktualnaPozicia));

	testGL();
	glFlush();
	testGL();
}

/** 
* Funkcia na nastavenie textury
* @param nil 
* @see load()
* @return void
*/
void Scene::setBackgroud(CTexture texture) {
	CTexture actualTex = mResManager->square.getTexture();
	if(actualTex.flagDelete) {
		actualTex.releaseTexture();
	}
	mResManager->square.setTexture(texture);
}

void Scene::nastavPozadieZoVstupu(cv::Mat& img) {
	if(log != NULL) {
		log->debugStream() << "Nastavujem texturu z videa.";
	}
	GLenum inputColourFormat = GL_BGR;
	if (img.channels() == 1) {
		inputColourFormat = GL_LUMINANCE;
	}

	// 2. Prerob unsigned char na texturu
	CTexture texture;
	texture.createFromData(img.ptr(), img.cols, img.rows, GL_RGB, inputColourFormat, true);
	texture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_NEAREST_MIPMAP);
	texture.flagDelete = true;

	// 3. nastav texturu na pozadie kocky
	setBackgroud(texture);
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