#include "class.Carlos.hpp"

using namespace Architecture;

int main()
{
	Carlos* carlos = new Carlos();
	carlos->start();
	delete carlos;
	return 0;
}
