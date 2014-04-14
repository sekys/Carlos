/** @file class.FrameData.hpp
* Trieda obsahuje funkcie na pracu s jednym framom
*/
#pragma once
#include "../../com.carlos.architecture/modules/class.ModulVykreslovania.hpp"

using namespace Architecture;

class FrameData {
private:
	ModulVykreslovania::In* vstup; /**< Jeden prijaty snimok */
	float deltaTime; /**< Zmena casu*/

public:
	FrameData();
	~FrameData();

	/** 
	* Funkcia ma na vstupe obrazok z hlavneho modulu
	* @param ModulVykreslovania::In* vstup
	* @see void Scene::ziskajAktualnyVstup(FrameData* frame)
	* @return void
	*/
	void setVstup(ModulVykreslovania::In* vstup);

	/** 
	* Funkcia nema na vstupe ziadny parameter, sluzi na prijatie ovladania z mobilu
	* @return ControllerCommands
	*/
	ControllerCommands getCommand();
	
	/** 
	* Funkcia nema na vstupe ziadny paramater, iba upravuje prijatu poziciu
	* @param  ::In* vstup
	* @see void Scene::ziskajAktualnyVstup(FrameData* frame)
	* @return void
	*/
	int getpozicia();

	float getDeltaTime();
	void setDeltaTime(float f);
	//cv::Mat getImage();
	cv::Mat getHorizont();
	bool hasVstup();
	ModulVykreslovania::In* getVstup();

	friend ostringstream& operator<< (ostringstream& out, FrameData* object);
};