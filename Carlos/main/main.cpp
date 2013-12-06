#include "class.Carlos.hpp"

using namespace Architecture;

/**
* Metoda za pomoci ktorej sa spsuti program.
*/
int main()
{
	// Carlos postupne inicializujeme, spustime, deinicializujeme
	Carlos* carlos = new Carlos();
	carlos->start();
	delete carlos;
	return 0;
}
