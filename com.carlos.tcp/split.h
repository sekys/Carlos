// http://stackoverflow.com/questions/5888022/split-string-by-single-spaces
#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>

void split(const std::string& str, std::vector<std::string>& v) {
	std::stringstream ss(str);
	ss >> std::noskipws;
	std::string field;
	char ws_delim;
	while(1) {
		if( ss >> field )
			v.push_back(field);
		else if (ss.eof())
			break;
		else
			v.push_back(std::string());
		ss.clear();
		ss >> ws_delim;
	}
}

/*
int main() {
	std::vector<std::string> v;
	split("hello world  how are   you", v);
	std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>(std::cout, "-"));
	std::cout << "\n";
}
*/