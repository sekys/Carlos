#include "class.Carlos.hpp"
#include "../db/dbService.hpp"

using namespace std;
using namespace Architecture;

int main()
{
	DBService* db = new DBService();
	db->selectObjects();

	Carlos* carlos = new Carlos();
	carlos->spustiProgram();
	delete carlos;
	return 0;
}
