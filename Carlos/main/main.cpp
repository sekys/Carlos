#include <carlos_global.h>
#include <iostream>
#include <fstream>
#include "class.Carlos.hpp"
#pragma comment(lib, "../Debug/com.carlos.architecture.lib")

using namespace Architecture;

/**
* Metoda za pomoci ktorej sa spsuti program.
*/
int main()
{
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	/*AllocConsole();
	freopen("CONIN$", "r",stdin);
	freopen("CONOUT$", "w",stdout);
	freopen("CONOUT$", "w",stderr);
	*/
	//freopen ("log.txt", "w", stdout);
	/*ofstream out("out.txt");
	cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
	*/

	// Carlos postupne inicializujeme, spustime, deinicializujeme
	Carlos* carlos = new Carlos();
	carlos->start();
	delete carlos;

	//_CrtDumpMemoryLeaks();
	return 0;
}
