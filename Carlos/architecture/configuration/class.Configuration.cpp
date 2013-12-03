#include "class.Configuration.h"

Configuration::Configuration() {
	doc.LoadFile("data/configuration.xml");
}

float Configuration::str_to_float(const std::string &in)
{
	std::istringstream  o(in); 
	float f; 
	if (!(o >> f)) {
		throw new std::exception("str_to_float");
	}
	o >> f; 
	return f;
}

const XMLElement* Configuration::getRoot() {
	return doc.FirstChildElement( "configuration" );
}

const XMLElement* Configuration::getConfig(const char *name) {
	return getRoot()->FirstChildElement(name);
}

const float Configuration::getConfigf(const char *name) {
	return str_to_float( getConfig(name)->GetText() );
}

const char* Configuration::getTitle() {
	return getConfig("title")->GetText();
}
