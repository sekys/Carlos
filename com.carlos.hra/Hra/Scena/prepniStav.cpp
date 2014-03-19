/** @file prepniStav.cpp
* Trieda ktora sa stara o to co sa ma vykonat pri prepnuti stavu hry 
*/
#include "class.Scene.hpp"
#include <iostream>
#include <cstdio>
#include <ctime>


using std::cerr;
using std::endl;

/** 
* Funkcia nema na vstupe ziadny parameter, stara sa o zobrazenie uvodnej obrazovky hry
* @see void Scene::init()
* @return void 
*/
void Scene::prepniStavNaObrazovku() {
	aktualnyStav = StavyHry::UVODNA_OBRAZOVKA;
	setBackgroud(resManager->bgUvod);


	zasobnikVstupov.clear();
	cout << "Prepinam stan na uvodnu obrazovku.\n";
}

/** 
* Funkcia nema na vstupe ziadny parameter, stara sa o zobrazenie obrazovky game over, ked lietadlo vyleti mimo obrazovky resp. klesne pod horizont
* @see void Scene::havaroval()
* @see void Scene::stavHrania(float fDelta)
* @return void 
*/
void Scene::prepniStavNaGameOver() {
	
	aktualnyStav = StavyHry::OBRAZOVKA_PREHRAL;
	duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
	std::cout<<"printf: "<< duration <<'\n';
	
	setBackgroud(resManager->bgGameOver);
	casPrejdenyNaGameOver = 0.0;
	zasobnikVstupov.clear();
	cout << "Prepinam stav na obrazovku prehral.\n";
}

/** 
* Funkcia nema na vstupe ziadny parameter, stara sa o zobrazenie obrazovky so skore
* @see void Scene::stavGameOver(float fDelta)
* @return void 
*/
void Scene::prepniStavNaScore() {

	aktualnyStav = StavyHry::OBRAZOVKA_SKORE;
	setBackgroud(resManager->bgScore);
	zasobnikVstupov.clear();
	cout << "Prepinam stav na obrazovku skore.\n";
}

/** 
* Funkcia nema na vstupe ziadny parameter, stara sa o zobrazenie obrazkov z videa - po zacati hry
* @see void Scene::stavUvodnaObrazovka()
* @return void 
*/
void Scene::prepniStavNaHrania() {
	start = std::clock();
	aktualnyStav = StavyHry::HRAJE_HRU;
	plain->setStartPosition(typ_lietadla);

	zasobnikVstupov.clear();
	cout << "Prepinam stav na hraje hru.\n";
}

void Scene::switchStateToTouristInfo() {
	aktualnyStav = StavyHry::TOURIST_INFO;
	zasobnikVstupov.clear();
	cout << "Switching state to tourist info.\n";
}