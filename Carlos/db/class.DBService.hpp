#pragma once
#include "sweetql.hpp"
#include "entities.hpp"
// http://stackoverflow.com/questions/120295/what-is-a-good-oo-c-wrapper-for-sqlite

namespace DB {
	using namespace std;

	class DBService {
	private:
		SqliteDB *db;

	public:
		DBService() {
			db = new SqliteDB("data/carlos.db3");
		}
		~DBService() {
			delete db;
			db = NULL;
		}

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


/*
// Callback sa nevola ked je insert alebo nozina je prazdna
int The_Callback(void *a_param, int argc, char **argv, char **column){
for (int i=0; i< argc; i++)
printf("%s,\t", argv[i]);
printf("aaa\n");
return 0;

int rc;
char *zErrMsg = 0;
char* sql = "SELECT * from object";
const char* data = "Callback function called";

// Execute SQL statement
rc = sqlite3_exec(db->getConnection(), sql, The_Callback, (void*)data, &zErrMsg);
if( rc != SQLITE_OK ){
fprintf(stderr, "SQL error: %s\n", zErrMsg);
sqlite3_free(zErrMsg);
}else{
fprintf(stdout, "Operation done successfully\n");
}

}*/
