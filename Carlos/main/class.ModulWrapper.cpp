#include "class.ModulWrapper.hpp"

using namespace Architecture;

ModulWrapper::ModulWrapper(IModul* modul, Dll* dll) {
	initCalled = false;	
	this->modul = modul;
	this->dll = dll;
}
ModulWrapper::~ModulWrapper() {
	SAFE_DELETE(modul)
		SAFE_DELETE(dll)
}

void ModulWrapper::callInit() {
	std::string name = typeid(*(this->modul)).name();
	if(initCalled) {
		cout << name << " uz so mraz spustil\n";
	} else {
		initCalled = true;
		if(modul->isThreaded()) {
			cout << name << " spustam v samostatnom vlakne.\n";
			init_thread = boost::thread(boost::bind(&IModul::init, modul));
		} else {
			cout << name << " spustam v hlavnom vlakne.\n";
			modul->init();
		}
	}
}