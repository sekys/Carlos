#include "class.ModulesController.hpp"

using namespace Architecture;

void ModulesController::callInits() {
	moduls_type::iterator it;
	for (it = moduls.begin(); it!= moduls.end(); ++it) {
		(*it)->callInit();
	}
}

void ModulesController::destroyThreads() {
	cout << "Interupting threads\n";
	moduls_type::iterator it;
	for (it = moduls.begin(); it!= moduls.end(); ++it) {
		ModulWrapper* wrapper = *it;
		if(wrapper->modul->isThreaded()) {
			wrapper->init_thread.interrupt();
		}
	}
	cout << "Joining threads\n";
	for (it = moduls.begin(); it!= moduls.end(); ++it) {
		ModulWrapper* wrapper = *it;
		if(wrapper->modul->isThreaded()) {
			wrapper->init_thread.join();
		}
	}
}

void ModulesController::destroyModules() {
	destroyThreads();
	cout << "Destroying modules\n";
	moduls_type::iterator it;
	for (it = moduls.begin(); it!= moduls.end(); ++it) {
		delete *it;
	}
	reset();
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
	// Zatial chceme pouzit fake moduly
	android = addDll<ModulAndroid>("com.carlos.android.tcpServer.dll");
	databaza = add(new ModulDatabaza() );
	kamera = add(new ModulKamera("data/video/2013-10-20-12-25-52.avi") );
	kinect = add(new ModulKinect() );
	vykreslovanie = add(new ModulVykreslovania() );
	vyppolohy = add(new ModulVypocitaniaPolohy());
	spracovanie = addDll<ModulSpracovania>("com.carlos.dll.dll");
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
	moduls_type::iterator it;
	for (it = moduls.begin(); it!= moduls.end(); ++it) {
		(*it)->modul->preFrame();
	}
}