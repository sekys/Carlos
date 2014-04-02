#pragma once
#include <carlos_global.h>
#include "class.ModulesController.hpp"
#include "class.App.hpp"
#include <log4cpp.h>

namespace Architecture {

	/**
	* Trieda reprezentuje hlavny projekt Carlos a jeho halvny algoritmus, ktory spaja ostatne moduly
	*/
	class Carlos : public App
	{
	private:
		ModulesController* controller; /**< Carlos spaja rozne moduly */
		log4cpp::Category* log;
		

		// Metoda na spracovanie snimku
		void spracujJedenSnimok(Image image);

		// Metoda pre ziskanie snimku
		void nacitajDalsiuSnimku();

	protected:
		// Inicializacia Carlosu
		virtual void Init();

		// Run() metoda je vola v cykle dokedy bezi apliakcia
		virtual bool Run();

	public:
		float poz;
		Carlos();
		~Carlos();
	};

}
