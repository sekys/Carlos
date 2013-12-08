/** @file class.ModulesCOntroller.cpp
*
* Implementacia ModulesController
*/
#include "class.ModulesController.hpp"

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
	cout << "Interupting threads\n";

	// V prvej faze sa snazime vsetky moduly stopnut ... to sa nemusi udiat hned
	moduls_type::iterator it;
	for (it = moduls.begin(); it!= moduls.end(); ++it) {
		ModulWrapper* wrapper = *it;
		if(wrapper->modul->isThreaded()) {
			wrapper->init_thread.interrupt();
		}
	}

	// V druhej faze cakame kym sa moduly stopnu
	cout << "Joining threads\n";
	for (it = moduls.begin(); it!= moduls.end(); ++it) {
		ModulWrapper* wrapper = *it;
		if(wrapper->modul->isThreaded()) {
			wrapper->init_thread.join(); // Cakame
		}
	}
}

void ModulesController::destroyModules() {
	// V tejto metode musime vsetky moduly uvolnit
	destroyThreads(); // teda uvolnime vlakna, ak existuju
	cout << "Destroying modules\n";
	moduls_type::iterator it;
	for (it = moduls.begin(); it!= moduls.end(); ++it) {
		delete *it; // uvolnime jednotlive instancie
	}
	reset(); // restartujeme premmenne na zakladne hodnoty
}

void ModulesController::reset() {
	android = NULL;
	databaza = NULL;
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
	databaza = add(new ModulDatabaza() );
	kamera = add(new ModulKamera("data/video/2013-10-20-12-25-52.avi") );
	kinect = add(new ModulKinect() );
	vykreslovanie = addDll<ModulVykreslovania>("com.carlos.hra.dll");
	vyppolohy = add(new ModulVypocitaniaPolohy());
	spracovanie = new ModulSpracovania();
}

ModulesController::ModulesController() {
	reset();
}

void ModulesController::start() {
	installModules();
	callInits();
}

void ModulesController::stop() {
	destroyModules();
}

void ModulesController::callPreFrames() {
	// V ramci tejto metody musime zavolat preFrame() metody vsetkych modulov
	moduls_type::iterator it;
	for (it = moduls.begin(); it!= moduls.end(); ++it) {
		(*it)->modul->preFrame();
	}
}