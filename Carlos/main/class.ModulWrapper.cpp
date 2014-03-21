/** @file class.ModulWrapper.cpp
*
* Implementacia ModulWrapper.
*/
#include <carlos_global.h>
#include "class.ModulWrapper.hpp"

using namespace Architecture;

// Prijmeme modul, pripadne subor a zabalime ho
ModulWrapper::ModulWrapper(IModul* modul, Dll* dll) {
	log = CREATE_LOG4CPP();
	initCalled = false;	
	this->modul = modul;
	this->dll = dll;
}

// Ked sa nas obal ma znicit, znic aj samotny modul a tovoreny subor
ModulWrapper::~ModulWrapper() {
	SAFE_DELETE(modul)
	SAFE_DELETE(dll)
}

// Spusti inicializaciu modulu na zakalde jeho parametrov
void ModulWrapper::callInit() {
	std::string name = typeid(*(this->modul)).name();
	if(initCalled) {
		if(log != NULL) {
			log->debugStream() << name << " uz som raz spustil";
		}
	} else {
		initCalled = true;
		if(modul->isThreaded()) {
			if(log != NULL) {
				log->debugStream() << name << " spustam v samostatnom vlakne.";
			}
			init_thread = boost::thread(boost::bind(&IModul::init, modul));
		} else {
			if(log != NULL) {
				log->debugStream() << name << " spustam v hlavnom vlakne.";
			}
			modul->init();
		}
	}
}