#include <carlos_global.h>
#include <iostream>
#include <fstream>
#include "class.Carlos.hpp"
#include "../../com.carlos.architecture/configuration/class.Configuration.hpp"
#pragma comment(lib, "../Debug/com.carlos.architecture.lib")

using namespace Architecture;

/**
* Metoda za pomoci ktorej sa spsuti program.
*/
int main()
{
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// Nastav logy
	// Zapni / Vypni pomocnu konzolu 
	/*AllocConsole();
	freopen("CONIN$", "r",stdin);
	freopen("CONOUT$", "w",stdout);
	freopen("CONOUT$", "w",stderr);
	*/
	ShowWindow( GetConsoleWindow(), SW_HIDE );
	// Presmeruj vystup do log suboru
	ofstream out("../log.txt");
	cout.rdbuf( out.rdbuf() );
	cout.setf( std::ios_base::unitbuf ); // This will cause a flush at the end of every <<
	

	// Carlos postupne inicializujeme, spustime, deinicializujeme
	Carlos* carlos = new Carlos();
	carlos->start();
	delete carlos;

	//_CrtDumpMemoryLeaks();
	return 0;
}
