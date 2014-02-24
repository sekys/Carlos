#pragma once
#include <carlos_global.h>
#include "../class.ModulAndroid.hpp"
#include "class.GPSLogFile.hpp"
#include "../../entities/class.Time.hpp"
#include <boost/bimap.hpp>

namespace Architecture
{
	class FakeModulAndroid : public ModulAndroid  {
	private:
		// http://www.boost.org/doc/libs/1_54_0/libs/bimap/doc/html/boost_bimap/one_minute_tutorial.html
		typedef boost::bimap< std::string, ControllerCommands> bm_type;
		bm_type bm;
		GPSLogFile* logFile;
		double lastGetRow;

	protected:
		virtual void prepareBimap() {
			bm.insert( bm_type::value_type("w", ControllerCommands::UP) );
			bm.insert( bm_type::value_type("s", ControllerCommands::DOWN) );
			bm.insert( bm_type::value_type("d", ControllerCommands::RIGHT) );
			bm.insert( bm_type::value_type("a", ControllerCommands::LEFT) );
			bm.insert( bm_type::value_type("tap", ControllerCommands::TAP) );
		}

		virtual void checkInput() {
			int key = cv::waitKey(30); // caka najmenej 30ms, zalezi od win procesov
			string str;  
			str =(char) key; // konvertuj
			updateCommand(str);
		}

		virtual bool updateCommand(std::string command) {
			// TODO: synchronizacia
			bm_type::left_const_iterator left_iter = bm.left.find(command);
			if(left_iter == bm.left.end() ) {
				setActualCommand(ControllerCommands::NO_ACTION);
				//cout << "Unkown key !\n";
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
		FakeModulAndroid(string filename) {
			ModulAndroid();
			prepareBimap();
			logFile = new GPSLogFile(filename);
			lastGetRow = 0;
		}

		~FakeModulAndroid() {
			SAFE_DELETE(logFile)
		}

		virtual ControllerCommands getActualCommand() {
			checkInput();
			return ModulAndroid::getActualCommand();
		}

		virtual void preFrame() {
			// Citaj dalsi zaznam az ked sekunda presla
			double actual = Time::getInstance().GetAbsolute();
			double delta = actual - lastGetRow;
			if(delta > 1.0) {
				lastGetRow = actual;
				GPSRow row = logFile->readNext();
				setGPS(row);
			}
		}

	};
}



