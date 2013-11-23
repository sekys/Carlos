#pragma once
// http://www.grinninglizard.com/tinyxml2docs/index.html
#include "tinyxml2/tinyxml2.h"
#include "../architecture/entities/baseTypes.h"

using namespace tinyxml2;

class Configuration {
private: 
	tinyxml2::XMLDocument doc;

	Configuration(const Configuration& a);  
	Configuration& operator=(const Configuration& a); 

	Configuration() {
		doc.LoadFile("data/configuration.xml");
	}

public: 
	static Configuration& getInstance() {
		static Configuration singleton; 
		return singleton; 
	}; 


	const XMLElement* getRoot() {
		return doc.FirstChildElement( "configuration" );
	}

	const XMLElement* getConfig(const char *name) {
		return getRoot()->FirstChildElement(name);
	}

	const char* getTitle() {
		return getConfig("title")->GetText();
	}

};