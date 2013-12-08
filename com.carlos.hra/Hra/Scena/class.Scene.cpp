/** @file class.Scene.cpp
* Trieda ktora sa stara o zobrazenie samotnej sceny, po jednotlivych framoch
*/

#include "class.Scene.hpp"
#include "..\Help\font.h"
#include <gl/glew.h>
#include <gl/glut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdarg.h>
#include <stdexcept>

/** 
* Funkcia nema na vstupe ziadne parametre, iba nastavi scenu na inicializacne hodnoty
* @param 
* @return void
*/
void Scene::init() {
	printf("Spustam Scene::init()\n");
	resManager.load();
	visualController.load(resManager.shaders);
	prepniStavNaObrazovku();
	buildFont();
	glEnable(GL_TEXTURE_2D);
	printf("Koncim Scene::init()\n");
}

/** 
* Funkcia ma na vstupe 3 parametre, stacenu klavesu a aktualnu poziciu lietadla x a y koordinaty
* @param key - stlacena klavesa
* @param x - pozicia lietadla na osi x
* @param y - pozicia lietadla na osi y
* @see keyboard(unsigned char key, int x, int y)
* @return void
*/
/*
void Scene::keyboard(unsigned char key, int x, int y)
{
switch(key)
{
case 'q': {
exit(0);
break;
}
default: {
pressedKey = key;
break;
}
}
}
*/
/** 
* Funkcia ma na vstupe 1 parameter a to zmenu casu, stara sa vykreslenie 1 framu
* @param fDelta - zmena casu
* @see renderScene()
* @return void
*/
void Scene::frame(float fDelta) {
	glClearColor(1,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_LIGHTING);

	FrameData frame;
	frame.deltaTime = fDelta;
	ziskajAktualnyVstup(&frame);
	visualController.setPerspektive();
	visualController.renderObject(resManager.square, world->getMatrix());
	delenieStavov(&frame);

	// Flushujeme buffer
	glFlush();
	glutPostRedisplay();
	glutSwapBuffers();
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
	default: {
		printf("Neocakavany stav\n");
			 }
	}
}

const char* ControllerCommand2cchar(ControllerCommands command) {
	switch(command) {
	case ControllerCommands::NO_ACTION: return("NO");
	case ControllerCommands::DOWN: return("DOWN"); 
	case ControllerCommands::UP: return("UP"); 
	case ControllerCommands::LEFT: return("LEFT"); 
	case ControllerCommands::RIGHT: return("RIGHT");
	};
	return NULL;
}

void Scene::ziskajAktualnyVstup(FrameData* frame) {

	// 1. krok prerob snimku z Cv:mat na unsigned char
	// http://r3dux.org/2012/01/how-to-convert-an-opencv-cvmat-to-an-opengl-texture/
	try {
		frame->vstup = zasobnikVstupov.top();
		frame->command = frame->vstup.command;
		frame->hasVstup = true;
		printf("Prijmam  prikaz %s\n", ControllerCommand2cchar(frame->command));


	} catch (const std::out_of_range& oor) {
		// Zasobnik je prazdny, ponechaj tam staru texturu do kedy nepride obrazok
		// Ponechaj staru texturu, ....
		frame->hasVstup = false;
		frame->command = ControllerCommands::NO_ACTION;
	}
}

/** 
* Funkcia na nastavenie textury
* @param nil 
* @see load()
* @return void
*/
void Scene::setBackgroud(CTexture texture) {
	CTexture actualTex = resManager.square.getTexture();
	if(actualTex.flagDelete) {
		actualTex.releaseTexture();
	}
	resManager.square.setTexture(texture);

}

void Scene::sendVstup(ModulVykreslovania::In data) {
	zasobnikVstupov.push(data);
}