#pragma once
#include "class.ModulesController.hpp"

namespace Architecture {

	class Carlos
	{
	private:
		ModulesController* controller;

	public:
		Carlos() {
			controller = new ModulesController();
		}

		~Carlos() {
			controller->stop();
			SAFE_DELETE(controller);
		}

		void spustiProgram() {
			controller->start();
			spracujSnimky();
		}

		void spracujJedenSnimok(Image& image);
		void spracujSnimky();
	};

}
