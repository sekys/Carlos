#pragma once
#include "class.Dll.hpp"
#include "..\architecture\modules\class.ModulAndroid.hpp"
#include "..\architecture\modules\class.ModulDatabaza.hpp"
#include "..\architecture\modules\class.ModulKamera.hpp"
#include "..\architecture\modules\class.ModulKinect.hpp"
#include "..\architecture\modules\class.ModulSpracovania.hpp"
#include "..\architecture\modules\class.ModulVykreslovania.hpp"
#include "..\architecture\modules\class.ModulVypocitaniaPolohy.hpp"

namespace Architecture {
	using namespace std;

	class ModulesController
	{
	private:
		vector<Dll*> importedDlls;

		void callInits() {
			android->init();
			databaza->init();
			kamera->init();
			kinect->init();
			spracovania->init();
			vykreslovania->init();
			vyppolohy->init();
		}

		void dropDlls() {
			vector<Dll*>::iterator it;
			for (it = importedDlls.begin(); it!= importedDlls.end(); ++it) {
				delete *it;
			}
			importedDlls.clear();
		}

		void callDeconstructors() {
			SAFE_DELETE( android);
			SAFE_DELETE( databaza);
			SAFE_DELETE( kamera);
			SAFE_DELETE( kinect);
			SAFE_DELETE( spracovania);
			SAFE_DELETE( vykreslovania);
			SAFE_DELETE( vyppolohy);
		}

		void reset() {
			android = NULL;
			databaza = NULL;
			kamera = NULL;
			kinect = NULL;
			spracovania = NULL;
			vykreslovania = NULL;
			vyppolohy = NULL;
		}

		template <typename T> T* addDll(string file) {
			Dll* lib = new Dll(file);
			importedDlls.push_back(lib);			
			return lib->callFactory<T>();
		}

		virtual void installModules() {
			// Zatial chceme pouzit fake moduly
			android = new ModulAndroid();
			databaza = new ModulDatabaza();
			kamera = new ModulKamera("7.avi");
			kinect = new ModulKinect();
			//spracovania = new ModulSpracovania();
			vykreslovania = new ModulVykreslovania();
			vyppolohy = new ModulVypocitaniaPolohy();

			// Z dll sa to bude nacitavat takto
			cout << "Otvaram dll subor.\n";
			spracovania = addDll<ModulSpracovania>("com.carlos.dll.dll");
		}

	public:
		ModulAndroid* android;
		ModulDatabaza* databaza;
		ModulKamera* kamera;
		ModulKinect* kinect;
		ModulSpracovania* spracovania;
		ModulVykreslovania* vykreslovania;
		ModulVypocitaniaPolohy* vyppolohy;

		ModulesController() {
			reset();
		}

		virtual void start() {
			installModules();
			callInits();
		}

		virtual void stop() {
			callDeconstructors();
			dropDlls();
		}
	};

}