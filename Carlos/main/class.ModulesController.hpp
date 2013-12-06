#pragma once
#include "class.Dll.hpp"
#include "..\architecture\modules\class.ModulAndroid.hpp"
#include "..\architecture\modules\class.ModulDatabaza.hpp"
#include "..\architecture\modules\class.ModulKamera.hpp"
#include "..\architecture\modules\class.ModulKinect.hpp"
#include "..\architecture\modules\class.ModulSpracovania.hpp"
#include "..\architecture\modules\class.ModulVykreslovania.hpp"
#include "..\architecture\modules\class.ModulVypocitaniaPolohy.hpp"
#include "..\architecture\modules\fake\class.FakeModulAndroid.hpp"
#include "class.ModulWrapper.hpp"
#include <map>

namespace Architecture {
	using namespace std;

	/**
	* Trieda sa stara o riadenie modulov.
	* Teda o ich nacitanie, uvolnenie,  volanie ich inicializacnych metod.
	*/
	class ModulesController
	{
	private:
		typedef vector<ModulWrapper*> moduls_type; /**< Zoznam modulov je ulozeny vo vektore */
		moduls_type moduls; /**< Zoznam modulov */

		void callInits();	// zavolaj vsetky init metody modulov, teda inicializuj vsetky moduly
		void destroyThreads();	// Uvolni vsetky vlakna modulov
		void destroyModules();			// Uvolni vsetky moduly
		void reset();			// Restartuj vsetky premenne na zakaldne hodnoty

	protected:
		// Metoda na zaregistrovanie modula z instancie
		template <typename T> T* add(T* instance) {
			moduls.push_back(  new ModulWrapper(instance));
			return instance;
		}

		// Metoda na zaregistrovanie modula z dll subora
		template <typename T> T* addDll(string file) {
			Dll* dll = new Dll(file);		
			T* instance = dll->callFactory<T>();
			moduls.push_back(  new ModulWrapper(instance, dll));
			return instance;
		}

	public:
		// Zoznam modulov
		ModulAndroid* android;
		ModulDatabaza* databaza;
		ModulKamera* kamera;
		ModulKinect* kinect;
		ModulVykreslovania* vykreslovanie;
		ModulVypocitaniaPolohy* vyppolohy;
		ModulSpracovania* spracovanie;

		ModulesController();
		virtual void installModules();
		virtual void start();
		virtual void stop();
		void callPreFrames();
	};

}