/** @file class.Scene.cpp
* Trieda ktora sa stara o zobrazenie samotnej sceny, po jednotlivych framoch
*/


#include "class.Scene.hpp"
#include "Help\font.h"
#include <gl/glew.h>
#include <gl/glut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdarg.h>

/** 
* Funkcia nema na vstupe ziadne parametre, iba nastavi scenu na inicializacne hodnoty
* @param 
* @return nil
*/
void Scene::init() {
	shaders.load();
	prepniStavNaObrazovku();
	buildFont();
	glEnable(GL_TEXTURE_2D);
}

/** 
* Funkcia ma na vstupe 3 parametre, stacenu klavesu a aktualnu poziciu lietadla x a y koordinaty
* @param key - stlacena klavesa
* @param x - pozicia lietadla na osi x
* @param y - pozicia lietadla na osi y
* @see openGLInit()
* @see keyboard(unsigned char key, int x, int y)
* @return void
*/
void Scene::keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'q': {
		exit(1);
		break;
			  }
	default: {
		pressedKey = key;
		break;
			 }
	}
}

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

	shaders.setPerspektive();
	shaders.renderObject(shaders.square, world->getMatrix());
	delenieStavov(fDelta);

	/// Nulujeme hodnoty
	pressedKey = '\0';
	glFlush();
	glutPostRedisplay();
	glutSwapBuffers();
}

void Scene::delenieStavov(float fDelta) {
	switch(aktualnyStav) {
	case StavyHry::HRAJE_HRU: {
		stavHrania(fDelta);
		break;
							  }
	case StavyHry::OBRAZOVKA_PREHRAL: {
		stavGameOver(fDelta);
		break;
									  }
	case StavyHry::OBRAZOVKA_SKORE: {
		stavSkore(fDelta);
		break;
									}
	case StavyHry::PRIPRAVA_NA_HRU: {
		break;
									}
	case StavyHry::UVODNA_OBRAZOVKA: {
		stavUvodnaObrazovka();
		break;
									 }
	default: {
		printf("Neocakavany stav\n");
			 }
	}
}

void Scene::prepniStavNaObrazovku() {
	aktualnyStav = StavyHry::UVODNA_OBRAZOVKA;
	shaders.setBackground("Data/Carlos_uvod.png");
}

void Scene::prepniStavNaGameOver() {
	aktualnyStav = StavyHry::OBRAZOVKA_PREHRAL;
	shaders.setBackground("Data/Carlos_game_over.png");
	casPrejdenyNaGameOver = 0.0;
}

void Scene::prepniStavNaScore() {
	aktualnyStav = StavyHry::OBRAZOVKA_SKORE;
	shaders.setBackground("Data/Carlos_Score.png");
}

void Scene::prepniStavNaHrania() {
	aktualnyStav = StavyHry::HRAJE_HRU;
	shaders.setBackground("Data/bratislavsky.png");
	plain->setStartPosition();
}

void Scene::stavSkore(float fDelta) {
	if(pressedKey == 'w') {
		prepniStavNaHrania();
	}
}

void Scene::stavGameOver(float fDelta) {
	casPrejdenyNaGameOver += fDelta;
	if(casPrejdenyNaGameOver > 3.0) {
		// presiel cas a zmeni sa stav
		prepniStavNaScore() ;
	}
}

void Scene::stavHrania(float fDelta) {
	/// Spustame logiku casti
	collisionStatus colStatus;
	colStatus = plain->collisionTest(*world); // prava strana lietadla je 1 ked je na pravo colStatus.right
	// 001011
	printf("%d %d %d %d\n",  colStatus.bottom, colStatus.left, colStatus.right, colStatus.top);
	// lietadlo je na lavej strane sveta a 1 je		right
	// lietadlo je na pravej strane a 1 je		left		a right
	// lietadlo je hore 1 je:					bottom		a right
	// lietadlo je dole							top		
	// v strede je								front, right, top 

	bool contain = world->contains(*plain);
	if(contain) {
		printf("Y\n"); // toto vracia Y aj ked sa len dotykaju, neskor sa to prepne na N
	} else {
		printf("N\n");
		havaroval();
	}
	contain = plain->contains(*world);
	if(contain) {
		printf("A\n");
	} else {
		printf("B\n");  // toto vracia stael B
	}
	plain->logic(fDelta, pressedKey);
	shaders.renderObject(shaders.plain, plain->getMatrix());

	// Hud
	glUseProgram(0);
	glDisable(GL_LIGHTING);
	glLoadIdentity();
	glTranslatef(0.0f,0.8f,0.0f);
	glColor3f(0.0f, 0.f, 0.f);
	glRasterPos2f(0.0f, 0.f); 
	//glPrint("STLAC MEDZERNIK");
	glEnable(GL_LIGHTING);
}

void Scene::havaroval() {
	prepniStavNaGameOver();
}

void Scene::stavUvodnaObrazovka() {
	// Hud
	glUseProgram(0);
	glDisable(GL_LIGHTING);
	glLoadIdentity();
	glTranslatef(0.0f,0.8f,0.0f);
	glColor3f(0.0f, 0.f, 0.f);
	glRasterPos2f(0.0f, 0.f); 
	//glPrint("STLAC MEDZERNIK");
	glEnable(GL_LIGHTING);

	if(pressedKey == 'w') {
		prepniStavNaHrania();
	}
}