#pragma once
#include "../../libs/include/carlos_global.h"
#include "opencv2/opencv.hpp"
#include <opencv2\core\core.hpp>
#include "cv_entities.h"

namespace Architecture
{
	typedef unsigned char byte;
	typedef unsigned int uint;

	using namespace std;
	using namespace cv;

	/**
	* Trieda pre obrazok ako univerzalna entita v ramci Carlos.
	*/
	class Image {
	public:
		double frame;
		Mat data;

		friend ostream& operator<< (ostream& out, Image& object) {
			out << "Image(frame:" << object.frame << ")";
			return out;
		}
	};

	/**
	* Trieda reprezentuje GPS informaciu
	*/
	class GPS {
	public:
		float latitude, longitude;

		friend ostream& operator<< (ostream& out, GPS& object) {
			out << "GPS(latitude: " << object.latitude << ", longitude:" << object.longitude << ")";
			return out;
		}
	};

	/**
	* Trieda reprezentuje objekt v 3D svete 
	*/
	class WorldObject {
	public:
		uint id;
		GPS position;
		vector<string> cestyKSuborom; int value; /**< cesty k deskriptorom */

		friend ostream& operator<< (ostream& out, WorldObject& object) {
			out << "WorldObject(";
			out << "Id: " << object.id << ", ";
			out << "position:" << object.position << ", ";
			out << "cesty: (";
			for(uint i=0; i < object.cestyKSuborom.size(); i++) {
				out << object.cestyKSuborom.at(i) << ",";
			}
			out << "))";
			return out;
		}
	};

	/**
	* Trieda reprezentuje objekt detekovany na obrazovke videa.
	* Dany objekt odkazuje na svetovy objekt
	*/
	class DetekovanyObjekt {
	public:
		WorldObject objekt;
		RotatedRect boundary; /**< ojekt ma velkost rectange, to je aj jeho pozicia na obrazku */

		friend ostream& operator<< (ostream& out, DetekovanyObjekt& object) {
			out << "DetekovanyObjekt(";
			out << "objekt: " << object.objekt << ",";
			out << ")";
			return out;
		}
	};

	/**
	* Enum reprezentuje mozne typy prikazov, ktore prichadzaju od ovladaca
	*/
	enum ControllerCommands { 
		NO_ACTION,
		UP, 
		DOWN, 
		LEFT, 
		RIGHT, 
		TAP,
		WHAT_IS_OBJECT,
		MORE_ABOUT_OBJECT
	};

	inline ostream& operator<< (ostream& out, ControllerCommands& command) {
		switch(command) {
		case ControllerCommands::NO_ACTION: { out << "NO_ACTION"; break;}
		case ControllerCommands::DOWN: { out << "DOWN";  break;}
		case ControllerCommands::UP: { out << "UP";  break;}
		case ControllerCommands::LEFT: { out << "LEFT";  break;}
		case ControllerCommands::RIGHT: { out << "RIGHT";  break;}
		case ControllerCommands::TAP: { out << "TAP";  break;}
		case ControllerCommands::WHAT_IS_OBJECT: { out << "WHAT IS OBJECT";  break; }
		case ControllerCommands::MORE_ABOUT_OBJECT: { out << "MORE ABOUT OBJECT";  break; }
		default: {
			throw new std::exception();
				 }
		};
		return out;
	}

}