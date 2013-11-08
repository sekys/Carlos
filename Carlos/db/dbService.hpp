#pragma once
#include "sweetql.hpp"
#include "entities.hpp"
// http://stackoverflow.com/questions/120295/what-is-a-good-oo-c-wrapper-for-sqlite

using namespace std;

class DBService {
private:
	SqliteDB *db;

public:
	DBService() {
		db = new SqliteDB("carlos.db3");
	}
	~DBService() {
		delete db;
		db = NULL;
	}

	vector<Object> selectObjects() {
		auto sel(db->select<Object>());
		std::vector<Object> list;
		
		std::for_each(sel.first, sel.second, [](const Object& p)) {
			printf("a");
		}
		return list;
	}

};