#include <carlos_global.h>
#include <iostream>
#include <fstream>
#include "class.Carlos.hpp"
#include "../../com.carlos.architecture/configuration/class.Configuration.hpp"
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

#ifdef _DEBUG
	#pragma comment(lib, "../Debug/com.carlos.architecture.lib")
#else
	#pragma comment(lib, "../Release/com.carlos.architecture.lib")
#endif

using namespace Architecture;

/**
* Metoda za pomoci ktorej sa spsuti program.
*/
int main()
{
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// Nastav logy
	// ShowWindow( GetConsoleWindow(), SW_HIDE );
	std::string initFileName = "../data/log4cpp.properties";
	log4cpp::PropertyConfigurator::configure(initFileName);
	log4cpp::Category::getRoot().debug("Starting log4cpp, configuration %s", initFileName.c_str());

	// Carlos postupne inicializujeme, spustime, deinicializujeme
	Carlos* carlos = new Carlos();
	carlos->start();
	delete carlos;

	//_CrtDumpMemoryLeaks();
	return 0;
}
