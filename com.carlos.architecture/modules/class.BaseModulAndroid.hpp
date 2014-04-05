#pragma once
#include <carlos_global.h>
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"
#include "opencv2/opencv.hpp"
#include <boost/thread/mutex.hpp>

namespace Architecture
{
	using namespace std;

	class BaseModulAndroid : public IModul  {
	private:
		ControllerCommands actualCommand;
		boost::mutex actualCommand_mtx;
		GPS gps;
		boost::mutex gps_mtx;

	public:
		BaseModulAndroid() {
			actualCommand = ControllerCommands::NO_ACTION;
		}

		// synchronizovane na urovni setter a getter
		virtual void setGPS(GPS gps) {
			gps_mtx.lock();
			this->gps = gps;
			gps_mtx.unlock();
		}

		// synchronizovane na urovni setter a getter
		virtual GPS getGPS() {
			gps_mtx.lock();
			GPS tempGps = gps;
			gps_mtx.unlock();
			return tempGps;
		}

		// synchronizovane na urovni setter a getter
		virtual void setActualCommand(ControllerCommands command) {
			actualCommand_mtx.lock();
			actualCommand = command;
			actualCommand_mtx.unlock();
		}

		// synchronizovane na urovni setter a getter
		virtual ControllerCommands getActualCommand() {
			actualCommand_mtx.lock();
			ControllerCommands tempActualCommand = actualCommand;
			actualCommand_mtx.unlock();
			return tempActualCommand;
		}
	};
}



