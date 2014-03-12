#include <carlos_global.h>
#include "class.DBService.hpp"
#include "entities.hpp"


using namespace DB;
using namespace std;
using namespace Architecture;

//-- Prevzate od
// http://www.thismuchiknow.co.uk/?p=71
#define DEG2RAD(dg) (dg * 0.01745327)
static void distanceFunc(sqlite3_context *context, int argc, sqlite3_value **argv)
{
	// check that we have four arguments (lat1, lon1, lat2, lon2)
	assert(argc == 4);
	// check that all four arguments are non-null
	if (sqlite3_value_type(argv[0]) == SQLITE_NULL || sqlite3_value_type(argv[1]) == SQLITE_NULL || sqlite3_value_type(argv[2]) == SQLITE_NULL || sqlite3_value_type(argv[3]) == SQLITE_NULL) {
		sqlite3_result_null(context);
		return;
	}
	// get the four argument values
	double lat1 = sqlite3_value_double(argv[0]);
	double lon1 = sqlite3_value_double(argv[1]);
	double lat2 = sqlite3_value_double(argv[2]);
	double lon2 = sqlite3_value_double(argv[3]);
	// convert lat1 and lat2 into radians now, to avoid doing it twice below
	double lat1rad = DEG2RAD(lat1);
	double lat2rad = DEG2RAD(lat2);
	// apply the spherical law of cosines to our latitudes and longitudes, and set the result appropriately
	// 6378.1 is the approximate radius of the earth in kilometres
	sqlite3_result_double(context, acos(sin(lat1rad) * sin(lat2rad) + cos(lat1rad) * cos(lat2rad) * cos(DEG2RAD(lon2) - DEG2RAD(lon1))) * 6378.1);
}
//---------------------------

DBService::DBService() {
	db = new SqliteDB("../data/carlos.db3");
	sqlite3_create_function(db->getConnection(), "distance", 4, SQLITE_UTF8, NULL, &distanceFunc, NULL, NULL);
}
DBService::~DBService() {
	delete db;
	db = NULL;
}



vector<WorldObject> DBService::najdiVsetkySvetoveObjektyBlizkoGPS(GPS gps) {
	vector<WorldObject> zoznam;

	// Hladaj objekty na zaklade vzdialenosti k autu, do 50km
	std::ostringstream where;
	where << "distance(latitude, longitude, " << gps.latitude << ", " << gps.longitude << ") < 50.0";
	auto sel(db->select<Object>(where.str()));
	SqliteDB::Iterator<Object> it; 
	for (it = sel.first ; it != sel.second; ++it) {
		// Vyber svtetovy objekt a uloz ho
		WorldObject world;
		world.id = it->id;
		world.position.latitude = it->latitude;
		world.position.longitude = it->longitude;

		// K svetovemu objektu najdi deskriptory
		std::ostringstream receptWhere;
		receptWhere << "id = " << world.id;
		auto paths(db->select<recept_file_info>(receptWhere.str()));
		SqliteDB::Iterator<recept_file_info> pit; 
		for (pit = paths.first ; pit != paths.second; ++pit) {
			world.cestyKSuborom.push_back(pit->path);
			zoznam.push_back(world);
		}

		//cout << world;
	}
	return zoznam;
}

/*
 * V databaze sa z nejakeho neznameho dovodu nesmu nachadzat
 * NULL hodnoty. Ak sa tam take najdu, vyhodi sa vynimka
 * a program spadne.
 */

Object *DBService::getObjectById(uint id) {
	Object *object = NULL;

	std::ostringstream where;
	where << "id = " << id;
	auto obj(db->select<Object>(where.str()));
	SqliteDB::Iterator<Object> it;

	for (it = obj.first ; it != obj.second; ++it) {
		object = new Object();

		object->id = it->id;
		object->name = it->name;
		object->longitude = it->longitude;
		object->latitude = it->latitude;
		object->short_description = it->short_description;
		object->long_description = it->long_description;
		object->source_url = it->source_url;
	}
	
	return object;
}

// Priklad metody, ako vytiahnut objekty z databazy, cez OOP mapovac
// Pozn.: Callback sa nevola ked je insert alebo nozina je prazdna
/*vector<Object> selectObjects() {
auto sel(db->select<Object>());
vector<Object> list;
SqliteDB::Iterator<Object> it; // toto je iterator z kniznice
for (it = sel.first ; it != sel.second; ++it) {
int number = it->id;
std::cout <<  number << '\n';
}
return list;
}*/
