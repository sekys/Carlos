#pragma once
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"
#include <boost/bimap.hpp>
#include "opencv2/opencv.hpp"

namespace Architecture
{
	using namespace std;

	class ModulAndroid : public IModul  {
	private:
		ControllerCommands actualCommand;
		GPS gps;

	public:
		ModulAndroid() {
			actualCommand = ControllerCommands::NO_ACTION;
		}

		// TODO: synchornizovat
		virtual void setGPS(GPS gps) {
			this->gps = gps;
		}

		// TODO: synchornizovat
		virtual GPS getGPS() {
			return gps;
		}

		// TODO: synchornizovat
		virtual void setActualCommand(ControllerCommands command) {
			actualCommand = command;
		}

		// TODO: synchornizovat
		virtual ControllerCommands getActualCommand() {
			return actualCommand;
		}
	};
}



