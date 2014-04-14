#pragma once
#include <carlos_global.h>
#include <iostream>

/// Enum v ktorom su zoskupene vsetky mozne stavy hry 
enum GameStates {
	UVODNA_OBRAZOVKA,  /**< Stav v ktorom je hra po zapnuti*/
	HRAJE_HRU, /**< Stav do ktoreho sa hra prepne ked sa dotkne displaya - ked hra hru */
	OBRAZOVKA_PREHRAL, /**< Stav do ktoreho sa hra dostane ked vyleti s lietadlom mimo dovoleneho uzemia */
	OBRAZOVKA_SKORE, /**< Stav v ktorom je hra ked sa zobrazuje skore */
	TOURIST_INFO /**< Stav v ktorom je hra ked sa zobrazuje hra turisticke info` */
};

using namespace std;

inline ostream& operator<< (ostream& out, GameStates& command) {
	switch(command) {
	case GameStates::UVODNA_OBRAZOVKA: { out << "UVODNA_OBRAZOVKA"; break;}
	case GameStates::HRAJE_HRU: { out << "HRAJE_HRU";  break;}
	case GameStates::OBRAZOVKA_PREHRAL: { out << "OBRAZOVKA_PREHRAL";  break;}
	case GameStates::OBRAZOVKA_SKORE: { out << "OBRAZOVKA_SKORE";  break;}
	case GameStates::TOURIST_INFO: { out << "TOURIST_INFO";  break;}
	default: {
		throw new std::exception();
				}
	};
	return out;
}