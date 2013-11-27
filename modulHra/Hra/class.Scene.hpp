#pragma once
/** @file class.Scene.hpp
* Trieda Scene ktora sa stara o zobrazenie modelu lietadla, textu, framu a ovladanie lietadla
*/

#include "Help\class.MyShaderController.hpp"
#include "Entity\class.Plain.hpp"
#include "Entity\class.World.hpp"
#include "StavyHry.h"

class Scene {
private:
	void delenieStavov(float fDelta);
	void stavHrania(float fDelta);
	void stavUvodnaObrazovka();
	void prepniStavNaObrazovku();
	void prepniStavNaGameOver();
	void prepniStavNaScore();
	void prepniStavNaHrania();
	void havaroval();
	void stavGameOver(float fDelta);
	void stavSkore(float fDelta);

	float casPrejdenyNaGameOver;

public:
	MyShaderController shaders; 
	unsigned char pressedKey; /**< Stlacena klavesa */
	Plain* plain; /**< Nacitany model lietadla */
	World* world;
	StavyHry aktualnyStav;

	Scene()  {
		plain = new Plain(glm::vec2(10.0, 10.0));
		world = new World(glm::vec2(320.0, 240.0));
	}
	~Scene() {
		delete plain;
		delete world;
	}

	void init();
	void frame(float fDelta);
	void keyboard(unsigned char key, int x, int y);
};