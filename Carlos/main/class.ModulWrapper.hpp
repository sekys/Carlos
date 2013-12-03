#pragma once
#include <boost/thread.hpp>
#include "../architecture/entities/baseTypes.h"
#include "../architecture/entities/class.IModul.hpp"
#include <iostream> 
#include "class.Dll.hpp"

namespace Architecture
{
	using namespace std;

	// Modul z pohladu aplikacie
	// Synchronizacia je na urovni setter a getter
	class ModulWrapper {
		friend class ModulesController;
	private:
		boost::thread init_thread;
		bool initCalled;
		Dll* dll;
		IModul* modul;

		ModulWrapper(IModul* modul, Dll* dll = NULL);
		~ModulWrapper();

		virtual void callInit();
	};
}