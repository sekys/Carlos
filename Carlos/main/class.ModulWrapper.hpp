#pragma once
#include <boost/thread.hpp>
#include "../architecture/entities/baseTypes.h"
#include "../architecture/entities/class.IModul.hpp"
#include <iostream> 
#include "class.Dll.hpp"

namespace Architecture
{
	using namespace std;

	/**
	* Modul z pohladu aplikacie.
	* Pozn.: Synchronizacia je na urovni setter a getter
	*/
	class ModulWrapper {
		friend class ModulesController;	/**< modul dovoluje odkryt svoje casti len ModulesController casti */
	private:
		boost::thread init_thread; /**< Vlakno v ktorom bezi modul. */
		bool initCalled; /**< Bol uz modul inicializovany ? Predchadzame dvojitemu spusteniu. */
		Dll* dll; /**< Dll subor, z ktoreho bol modul nacitany. */
		IModul* modul; /**< Priamo modul ulozeny */

		ModulWrapper(IModul* modul, Dll* dll = NULL);
		~ModulWrapper();

		// Metoda na spustenie inicializacie modulu 
		virtual void callInit();
	};
}