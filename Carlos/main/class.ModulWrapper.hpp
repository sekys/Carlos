#pragma once
#include <carlos_global.h>
#include <boost/thread.hpp>
#include "../../com.carlos.architecture/entities/class.IModul.hpp"
#include <iostream> 
#include "class.Dll.hpp"
#include <log4cpp.h>

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

		log4cpp::Category* log;

		ModulWrapper(IModul* modul, Dll* dll = NULL);
		~ModulWrapper();

		// Metoda na spustenie inicializacie modulu 
		virtual void callInit();
	};
}