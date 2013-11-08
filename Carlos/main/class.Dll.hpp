#pragma once
#include <windows.h>
#include <string>
#include <iostream>
#include <conio.h>
#include "utilities.hpp"
// http://edn.embarcadero.com/article/20165

namespace Architecture {

	class Dll
	{
		HINSTANCE hdll;
		std::string fileName;

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
		Dll(std::string fileName) {
			hdll = NULL;
			this->fileName = fileName;
			load();
		}
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

		Dll::~Dll() {
			if(hdll != NULL) {
				FreeLibrary(hdll); 
			}
		}
	};

}