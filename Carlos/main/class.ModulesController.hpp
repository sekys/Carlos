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

	class ModulesController
	{
	private:
		typedef vector<ModulWrapper*> moduls_type;
		moduls_type moduls;

		void callInits();
		void destroyThreads();
		void destroyModules();
		void reset();

	protected:
		template <typename T> T* add(T* instance) {
			moduls.push_back(  new ModulWrapper(instance));
			return instance;
		}

		template <typename T> T* addDll(string file) {
			Dll* dll = new Dll(file);		
			T* instance = dll->callFactory<T>();
			moduls.push_back(  new ModulWrapper(instance, dll));
			return instance;
		}

	public:
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