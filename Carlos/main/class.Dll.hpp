#pragma once
#include <windows.h>
#include <string>
#include <iostream>
#include <conio.h>
#include "utilities.hpp"
// http://edn.embarcadero.com/article/20165

namespace Architecture {

	/**
	* Trieda reprezentuje DLL subor, v ktorom je ulozeny nas modul.
	* Trieda sa stara o nacitanie modulu, jeho vypustenie a aj na konvertovanie
	*/
	class Dll
	{
		HINSTANCE hdll; /**< Dll subor v pameti. */
		std::string fileName; /**< Cesta k DLl suboru */

		// Metoda na nacitanie dll kniznice
		void load() {
			std::wstring wfile = s2ws(this->fileName);
			hdll = LoadLibrary(wfile.c_str());
			if(hdll == NULL) {
				std::string popis = "Kniznica nanejadena ";
				popis += this->fileName; 
				throw new std::runtime_error(popis);
			}
		}

	public:
		// Metoda ktorou sa otvori dll kniznice, priamo sa este nenacita jej obsah
		Dll(std::string fileName) {
			hdll = NULL;
			this->fileName = fileName;
			load();
		}

		/**
		* Metoda ziska instanciu triedy IModul, z daneho suboru na zaklade definovanej funkcie.
		* Funkcia je v ramci factory pattern. A teda jednotlivy modul mozme nacitat viac krat.
		* IModul typ je nasledne konvertovany podla potrieb.
		*/
		template <typename T>
		T* callFactory() {
			T* instance = NULL;
			//typedef void* (*pvFunctv)();
			typedef int (WINAPI *pvFunctv)();
			pvFunctv factory;
			FARPROC metoda = GetProcAddress( hdll, "callFactory" ); 
			if(metoda == NULL) {
				throw new std::exception("V dll nenajdena funkcia.");
			}
			factory = (pvFunctv) ( metoda );
			instance = (T*) ( factory() );
			return instance;
		}

		// Metoda sa stara o vypustenie kniznice z pameti
		Dll::~Dll() {
			if(hdll != NULL) {
				FreeLibrary(hdll); 
			}
		}
	};

}