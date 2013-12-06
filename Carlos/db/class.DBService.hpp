#pragma once
#include "sweetql.hpp"
#include "entities.hpp"
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

	public:
		DBService() {
			db = new SqliteDB("data/carlos.db3");
		}
		~DBService() {
			delete db;
			db = NULL;
		}

		// Priklad metody, ako vytiahnut objekty z databazy, cez OOP mapovac
		// Pozn.: Callback sa nevola ked je insert alebo nozina je prazdna
		vector<Object> selectObjects() {
			auto sel(db->select<Object>());
			vector<Object> list;
			SqliteDB::Iterator<Object> it; // toto je iterator z kniznice
			for (it = sel.first ; it != sel.second; ++it) {
				int number = it->id;
				std::cout <<  number << '\n';
			}
			return list;
		}

	};

}
