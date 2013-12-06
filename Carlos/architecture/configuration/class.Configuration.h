#pragma once
// http://www.grinninglizard.com/tinyxml2docs/index.html
#include "tinyxml2/tinyxml2.h"
#include "../entities/baseTypes.h"
#include <sstream>

using namespace tinyxml2;
/**
* Tato trieda sluzi pre spravu konfiguracie pre Carlos.
* Dokaze nacitat konfiguraciu do XMl stromu
*/
class Configuration {
private: 
	tinyxml2::XMLDocument doc;

	Configuration(const Configuration& a);  
	Configuration& operator=(const Configuration& a); 
	Configuration();

	// Metoda na prerobenie retazca na float, ktora sa pouziva v ramci XML
	float str_to_float(const std::string &in);

public: 
	// Trieda je typu singleton
	static Configuration& getInstance() {
		static Configuration singleton; 
		return singleton; 
	}; 

	// Ziskaj koren a dalsie elementy
	const XMLElement* getRoot();
	const XMLElement* getConfig(const char *name);
	const char* getTitle();
	const float getConfigf(const char *name);
};