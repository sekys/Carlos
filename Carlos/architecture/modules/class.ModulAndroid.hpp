#pragma once
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"
#include <boost/bimap.hpp>

namespace Architecture
{
	class ModulAndroid : public IModul  {
	protected:
		// http://www.boost.org/doc/libs/1_54_0/libs/bimap/doc/html/boost_bimap/one_minute_tutorial.html
		typedef boost::bimap< int, ControllerCommands> bm_type;
		bm_type bm;
		ControllerCommands actual;

		virtual void prepare() {
			bm.insert( bm_type::value_type('1', ControllerCommands::PRESS_1) );
			bm.insert( bm_type::value_type('2', ControllerCommands::PRESS_2) );
			bm.insert( bm_type::value_type('3', ControllerCommands::PRESS_3) );
			bm.insert( bm_type::value_type('S', ControllerCommands::START) );
			bm.insert( bm_type::value_type('D', ControllerCommands::STOP) );
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

		virtual void checkInput() {
			int key = cv::waitKey(30); // caka najmenej 30ms, zalezi od win procesov
			//cout << key << "\n";
			bm_type::left_const_iterator left_iter = bm.left.find(key);
			if(left_iter == bm.left.end() ) {
				actual = ControllerCommands::NO_ACTION;
			} else {
				actual = left_iter->second;
			}
		}
	public:
		virtual GPS getAktualnaPozicia() {
			GPS gps;
			gps.latitude = 10.f;
			gps.longitude = 14.f;
			return gps;
		}

		virtual ControllerCommands getActualCommand() {
			checkInput();
			return actual;
		}

		virtual void init() {
			prepare();
		}
	};
}



