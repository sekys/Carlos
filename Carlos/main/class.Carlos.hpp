#pragma once
#include "class.ModulesController.hpp"
#include "class.App.hpp"
#include "../db/class.DBService.hpp"

namespace Architecture {
	using namespace DB;

	class Carlos : public App
	{
	private:
		ModulesController* controller;
		DBService* db;
		Image frame;

		void spracujJedenSnimok(Image& image);
		void nacitajDalsiuSnimku();

	protected:
		virtual void Init();
		virtual bool Run();

	public:
		Carlos();
		~Carlos();
	};

}
