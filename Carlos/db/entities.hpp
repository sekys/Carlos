#pragma once
#include "sweetql.hpp"
#include "../architecture/entities/entities.h"
// http://stackoverflow.com/questions/120295/what-is-a-good-oo-c-wrapper-for-sqlite

class Question {
public:
	Question() {}
	static SqlTable<Question>& table() {
		static SqlTable<Question> tab = SqlTable<Question>::sqlTable("Question",
			SqlColumn<Question>("id", makeAttr(&Question::zadanie)),
			SqlColumn<Question>("zadanie",   makeAttr(&Question::zadanie)),
			SqlColumn<Question>("object_id",        makeAttr(&Question::object_id))
			);
		return tab;
	}

	std::string zadanie;
	int object_id;
	int id;
};

class Answer {
public:
	Answer() {}
	static SqlTable<Answer>& table() {
		static SqlTable<Answer> tab = SqlTable<Answer>::sqlTable("Answer",
			SqlColumn<Answer>("id",  makeAttr(&Answer::id)),
			SqlColumn<Answer>("odpoved",   makeAttr(&Answer::odpoved)),
			SqlColumn<Answer>("correct",        makeAttr(&Answer::correct)),
			SqlColumn<Answer>("question_id",        makeAttr(&Answer::object_id))
			);
		return tab;
	}

	int id;
	std::string odpoved;
	int correct;
	int object_id;
};

class Object {
public:
	Object() {}
	static SqlTable<Object>& table() {
		static SqlTable<Object> tab = SqlTable<Object>::sqlTable("Object",
			SqlColumn<Object>("id",  makeAttr(&Object::id)),
			SqlColumn<Object>("name",  makeAttr(&Object::name)),
			SqlColumn<Object>("latitude",   makeAttr(&Object::latitude)),
			SqlColumn<Object>("longitude",        makeAttr(&Object::longitude)),
			SqlColumn<Object>("source_url",        makeAttr(&Object::source_url))
			);
		return tab;
	}

	int id;
	float latitude;
	float longitude;
	std::string name;
	std::string source_url;
};

class recept_file_info {
public:
	recept_file_info() {}
	static SqlTable<recept_file_info>& table() {
		static SqlTable<recept_file_info> tab = SqlTable<recept_file_info>::sqlTable("recept_file_info",
			SqlColumn<recept_file_info>("id;",  makeAttr(&recept_file_info::id)),
			SqlColumn<recept_file_info>("path",   makeAttr(&recept_file_info::path)),
			SqlColumn<recept_file_info>("object",        makeAttr(&recept_file_info::object))
			);
		return tab;
	}
	int id;
	int object;
	std::string path;

};
