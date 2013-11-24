#pragma once
#include "baseTypes.h"
#include "opencv2/opencv.hpp"

namespace Architecture
{
	using namespace std;

	class Rotation { // : public vec3f
	public:
		float rotation[3];

		friend ostream& operator<< (ostream& out, Rotation& object) {
			out << "Rotation(" << object.rotation[0] << ", " << object.rotation[1] << ", " << object.rotation[2] << ")";
			return out;
		}
	};

	class Position { // : public vec2f
	public:
		float x, y;

		friend ostream& operator<< (ostream& out, Position& object) {
			out << "Position(x:" << object.x << ", y:" << object.y << ")";
			return out;
		}
	};

	class Image {
	public:
		unsigned long frame;
		cv::Mat data;

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
		Position position1; // lavy horny roh na obrazku
		Position position2; // pravy dolny roh na obrazku

		friend ostream& operator<< (ostream& out, DetekovanyObjekt& object) {
			out << "DetekovanyObjekt(";
			out << "objekt: " << object.objekt << ",";
			out << "Position upper-left:" << object.position1;
			out << "Position bottom-right:" << object.position2; 
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