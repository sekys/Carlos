
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

void Scene::init() {
	testGL();
	if(log != NULL) {
		log->debugStream() << "Spustam Scene::init()";
	}

	// Nacitaj controllery
	srand(time(NULL));
	mVisualController  = new VisualController();
	mResManager = new ResourceManager();
	mStates = new GameStateController(this);
	aktualnaPozicia = 0;
	blackBackground = cv::Mat(480, 640, CV_8UC3, Scalar(0, 0, 0));

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

	// Prepni na turisticke info (defaultny stav)
	mStates->switchTo(GameStates::TOURIST_INFO);
	//mStates->switchTo(GameStates::UVODNA_OBRAZOVKA);

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
	mVisualController->renderObject(mResManager->square, mWorld->getMatrix(aktualnaPozicia));

	testGL();
	glFlush();
	testGL();
}

void Scene::setBackgroud(CTexture texture) {
	CTexture actualTex = mResManager->square.getTexture();
	if(actualTex.flagDelete) {
		actualTex.releaseTexture();
	}
	mResManager->square.setTexture(texture);
}

void Scene::setBackgroud(cv::Mat& img) {
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

void Scene::setBlackBackground() {
	setBackgroud(blackBackground);
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