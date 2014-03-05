#pragma once
#include <carlos_global.h>
#include "sweetql.hpp"
#include "entities.hpp"
#include "../entities/entities.h"
// http://stackoverflow.com/questions/120295/what-is-a-good-oo-c-wrapper-for-sqlite

namespace DB {
	using namespace std;

	/**
	* Trieda ktora ma na starosti celu databazu.
	* Pripaja sa na databazu, riadi ju, odpoji sa s nej.
	* Zaroven cerpa a posiela informacie do databazy.
	*/
	class DBService {
	private:
		SqliteDB *db; /**< nas typ databazy je sqlite, neskor sa to moze zmenit */

		DBService(const DBService& a);  
		DBService& operator=(const DBService& a);
		DBService();
		~DBService();


	public:
		static DBService& getInstance() {
			static DBService singleton; 
			return singleton; 
		}; 

		virtual vector<Architecture::WorldObject> najdiVsetkySvetoveObjektyBlizkoGPS(Architecture::GPS gps);
		// REPLACE PP 5.3.2014 >>
		// virtual Object *getObjectById(uint id);
		virtual Object *getObjectById(Architecture::uint id);
		// REPLACE PP 5.3.2014 <<
	};

}
