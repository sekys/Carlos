/** @file class.ModulesCOntroller.cpp
*
* Implementacia ModulesController
*/
#include <carlos_global.h>
#include "class.ModulesController.hpp"
#include <log4cpp.h>

using namespace Architecture;

void ModulesController::callInits() {
	// V ramci tejto metody musime zavolat init() metody vsetkych modulov
	moduls_type::iterator it;
	for (it = moduls.begin(); it!= moduls.end(); ++it) {
		(*it)->callInit();
	}
}


void ModulesController::destroyThreads() {
	// V ramci tejto metody musime znicit vsetky thready jednotlivych modulov
	if(log != NULL) {
		log->debugStream()  << "Interupting threads";
	}

	// V prvej faze sa snazime vsetky moduly stopnut ... to sa nemusi udiat hned
	moduls_type::iterator it;
	for (it = moduls.begin(); it!= moduls.end(); ++it) {
		ModulWrapper* wrapper = *it;
		if(wrapper->modul->isThreaded()) {
			wrapper->init_thread.interrupt();
		}
	}

	// V druhej faze cakame kym sa moduly stopnu
	if(log != NULL) {
		log->debugStream()  << "Joining threads";
	}
	for (it = moduls.begin(); it!= moduls.end(); ++it) {
		ModulWrapper* wrapper = *it;
		if(wrapper->modul->isThreaded()) {
			wrapper->init_thread.timed_join( boost::posix_time::seconds(1)  );
		}
	}
}

void ModulesController::destroyModules() {
	// V tejto metode musime vsetky moduly uvolnit
	destroyThreads(); // teda uvolnime vlakna, ak existuju
	if(log != NULL) {
		log->debugStream()  << "Destroying modules";
	}
	moduls_type::iterator it;
	for (it = moduls.begin(); it!= moduls.end(); ++it) {
		delete *it; // uvolnime jednotlive instancie
	}
	reset(); // restartujeme premmenne na zakladne hodnoty
}

void ModulesController::reset() {
	android = NULL;
	//databaza = NULL;
	kamera = NULL;
	kinect = NULL;
	vykreslovanie = NULL;
	vyppolohy = NULL;
	spracovanie = NULL;
	moduls.clear();
}

void ModulesController::installModules() {
	// V tejto metode instalujeme moduly:
	// - fake moduly
	// - moduly normalne
	// - moduly z dll suborov
	android = addDll<ModulAndroid>("com.carlos.android.tcpServer.dll");
	kamera = add(new ModulKamera("../data/video/2013-10-20-12-25-52.avi") );
	//kamera = add(new ModulKamera(0) );
	kinect = add(new ModulKinect() );
	//kinect = addDll<ModulKinect>("com.carlos.kinect.dll");
	vykreslovanie = addDll<ModulVykreslovania>("com.carlos.hra.dll");
	//vyppolohy = add(new ModulVypocitaniaPolohy());
	vyppolohy = addDll<ModulVypocitaniaPolohy>("com.carlos.textPosModule.dll");
	//spracovanie = add(new ModulSpracovania());
	spracovanie = addDll<ModulSpracovania>("com.carlos.spracovanieReality.dll");
}

ModulesController::ModulesController() {
	log = CREATE_LOG4CPP();
	reset();
}

void ModulesController::start() {
	installModules();
	callInits();
}

void ModulesController::stop() {
	destroyModules();
}
/*
void ModulesController::callPreFrames() {
	// V ramci tejto metody musime zavolat preFrame() metody vsetkych modulov
	moduls_type::iterator it;
	for (it = moduls.begin(); it!= moduls.end(); ++it) {
		(*it)->modul->preFrame();
	}
}
*/
void ModulesController::callReset() {
	// V ramci tejto metody musime zavolat reset() metody vsetkych modulov
	moduls_type::iterator it;
	for (it = moduls.begin(); it!= moduls.end(); ++it) {
		(*it)->modul->doReset();
	}
}