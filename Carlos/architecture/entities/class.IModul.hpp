#pragma once

namespace Architecture
{
	/**
	* Interface pre kazdy modul, interface spaja spolocne spravanie modulov
	*/
	class IModul  {
	public: 
		// Modul bez v samostantom vlakne, vzdy
		// Tieto metody nie su osetrene pre viac vlakien
		// Osetrene su len gettre a settre jednotlivych modulov



		/**
		* Inicializacia modulu... do tejto metody si mozes dat
		* while(1) { .. } aby ti program nikdy nepadol, iba ak to potrebujes
		*/
		virtual void init() {} 

		/**
		* Tato metoda sa vola na zaciatku kazdeho snimku
		* Mala by obsahovat jednoducho logiku, ziadne zlozite vypocty
		* Napriklad to mozes pouzit na restartovanie premennych
		*/
		virtual void preFrame() {} 

		/**
		* Deconstructor sa moze pretazit
		* pouzijes ked chces vymazat alokovanu pamet
		*/
		virtual ~IModul() {}

		// Ma byt modul paralizovany ?
		virtual bool isThreaded() { return false; }
	};
}