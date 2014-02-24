#include <carlos_global.h>
#include "class.Carlos.hpp"
#pragma comment(lib, "../Debug/com.carlos.architecture.lib")

using namespace Architecture;

/**
* Metoda za pomoci ktorej sa spsuti program.
*/
int main()
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// Carlos postupne inicializujeme, spustime, deinicializujeme
	Carlos* carlos = new Carlos();
	carlos->start();
	delete carlos;

	_CrtDumpMemoryLeaks();
	return 0;
}
