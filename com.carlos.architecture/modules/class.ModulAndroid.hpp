#pragma once
#include <carlos_global.h>
#include "class.BaseModulAndroid.hpp"
#include "../entities/class.GPSLogFile.hpp"
#include <boost/bimap.hpp>

namespace Architecture
{
	class ModulAndroid : public BaseModulAndroid  {
	private:
		// http://www.boost.org/doc/libs/1_54_0/libs/bimap/doc/html/boost_bimap/one_minute_tutorial.html
		typedef boost::bimap< std::string, ControllerCommands> bm_type;
		bm_type bm;
		GPSLogFile* logFile;
		double lastImagePos;

	protected:
		virtual void prepareBimap() {
			// Mapovanie kalvesnice na prikazi - tu ma byt len 1 pismenko !
			bm.insert( bm_type::value_type("w", ControllerCommands::UP) );
			bm.insert( bm_type::value_type("s", ControllerCommands::DOWN) );
			bm.insert( bm_type::value_type("d", ControllerCommands::RIGHT) );
			bm.insert( bm_type::value_type("a", ControllerCommands::LEFT) );
			bm.insert( bm_type::value_type("r", ControllerCommands::TAP) );
			bm.insert( bm_type::value_type("t", ControllerCommands::WHAT_IS_OBJECT));
			bm.insert( bm_type::value_type("y", ControllerCommands::MORE_ABOUT_OBJECT));
			bm.insert( bm_type::value_type("u", ControllerCommands::GAME));
			bm.insert( bm_type::value_type("i", ControllerCommands::TOURIST_INFO));
		}

		virtual void checkInput() {
			int key = cv::waitKey(30); // caka najmenej 30ms, zalezi od win procesov
			string str;  
			str =(char) key; // konvertuj
			updateCommand(str);
		}

		virtual bool updateCommand(std::string command) {
			bm_type::left_const_iterator left_iter = bm.left.find(command);
			if(left_iter == bm.left.end() ) {
				setActualCommand(ControllerCommands::NO_ACTION);
				//throw new exception("Unkown key !\n");
				return false;
			}
			setActualCommand(left_iter->second);
			return true;		
		}
		virtual string toString(ControllerCommands type)  {
			bm_type::right_const_iterator right_iter = bm.right.find(type);
			if(right_iter == bm.right.end() ) {
				return "-";
			}
			string text;
			text += right_iter->first;
			return text;
		}

	public:
		ModulAndroid(string filename) {
			BaseModulAndroid();
			prepareBimap();
			logFile = new GPSLogFile(filename);
			lastImagePos = -10000;
		}

		~ModulAndroid() {
			SAFE_DELETE(logFile)
		}

		virtual ControllerCommands getActualCommand() {
			checkInput();
			return BaseModulAndroid::getActualCommand();
		}

		// Sem pride pozicia obrazka vo videu v ms
		virtual void prepareFakeGPS(double actualPos) {
			// Citaj dalsi zaznam az ked sekunda presla
			double delta = actualPos - lastImagePos;
			if(delta > 100) {
				lastImagePos = actualPos;
				GPSRow row = logFile->readNext();
				setGPS(row);
			}
		}

		virtual void doReset() {
			lastImagePos = -10000;
			logFile->reset();
		}
	};
}



