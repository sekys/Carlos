/** @file utilities.cpp
*
* Implementacia pomocnych funkcii.
*/
// http://stackoverflow.com/questions/5888022/split-string-by-single-spaces
#pragma once
#include <carlos_global.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include "utilities.hpp"
#include <windows.h>
#include <iostream>

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

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