#pragma once
#include "baseTypes.h"
#include "opencv2/opencv.hpp"
#include <opencv2\core\core.hpp>
#include "cv_entities.h"

namespace Architecture
{
	using namespace std;
	using namespace cv;

	class Image {
	public:
		unsigned long frame;
		Mat data;

		friend ostream& operator<< (ostream& out, Image& object) {
			out << "Image(frame:" << object.frame << ")";
			return out;
		}
	};

	class GPS {
	public:
		float latitude, longitude;

		friend ostream& operator<< (ostream& out, GPS& object) {
			out << "GPS(latitude: " << object.latitude << ", longitude:" << object.longitude << ")";
			return out;
		}
	};

	class WorldObject {
	public:
		uint id;
		GPS position;
		vector<string> cestyKSuborom;

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

	class DetekovanyObjekt {
	public:
		WorldObject objekt;
		RotatedRect boundary; // na obrazku

		friend ostream& operator<< (ostream& out, DetekovanyObjekt& object) {
			out << "DetekovanyObjekt(";
			out << "objekt: " << object.objekt << ",";
			out << ")";
			return out;
		}
	};

	enum ControllerCommands { 
		NO_ACTION,
		PRESS_1, 
		PRESS_2, 
		PRESS_3, 
		START, 
		STOP
	};

}