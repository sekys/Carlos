/** @file utilities.hppp
*
* Definicia pomocnych funkcii.
*/
#pragma once
#include <carlos_global.h>
#include <string>
#include <vector>

/** Funkcia ktora prerobi string na wstring
*
* @param c1 string text
* @return wstring text
*/
std::wstring s2ws(const std::string& s);

/** Funkcia ktora rozdeli retazec do vektoru podla delimetera.
*
* @param c1 vstupny text
* @param c2 pole vyslednych retazcov
* @return void
*/
void split(const std::string& str, std::vector<std::string>& v);