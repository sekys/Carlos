#pragma once
// http://www.grinninglizard.com/tinyxml2docs/index.html
#include "tinyxml2/tinyxml2.h"
#include "../entities/baseTypes.h"
#include <sstream>

using namespace tinyxml2;

class Configuration {
private: 
	tinyxml2::XMLDocument doc;

	Configuration(const Configuration& a);  
	Configuration& operator=(const Configuration& a); 
	Configuration();

	float str_to_float(const std::string &in);

public: 
	static Configuration& getInstance() {
		static Configuration singleton; 
		return singleton; 
	}; 


	const XMLElement* getRoot();
	const XMLElement* getConfig(const char *name);
	const char* getTitle();
	const float getConfigf(const char *name);
};