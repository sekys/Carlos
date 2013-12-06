#pragma once
#include "class.ModulesController.hpp"
#include "class.App.hpp"
#include "../db/class.DBService.hpp"

namespace Architecture {
	using namespace DB;

	/**
	* Trieda reprezentuje hlavny projekt Carlos a jeho halvny algoritmus, ktory spaja ostatne moduly
	*/
	class Carlos : public App
	{
	private:
		ModulesController* controller; /**< Carlos spaja rozne moduly */
		DBService* db; /**< Carlos pouziva databazu */
		Image frame; /**< Aktualna snimak z maery, tato snimka sa moze preskocit, nemusi sa spracovat */

		// Metoda na spracovanie snimku
		void spracujJedenSnimok(Image& image);

		// Metoda pre ziskanie snimku
		void nacitajDalsiuSnimku();

	protected:
		// Inicializacia Carlosu
		virtual void Init();

		// Run() metoda je vola v cykle dokedy bezi apliakcia
		virtual bool Run();

	public:
		Carlos();
		~Carlos();
	};

}
