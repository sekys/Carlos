#pragma once
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"

namespace Architecture
{
	class ModulVypocitaniaPolohy : public IModul  {
	public:
		class In {
		public:
			Point3f rotaciaHlavy;
			RotatedRect polohaObjektu;
			GPS gpsPolohaObjektu;
			GPS gps;

			friend ostream& operator<< (ostream& out, In& object) {
				out << "ModulVypocitaniaPolohyIn(";
				out << "rotaciaHlavy: " << object.rotaciaHlavy << ", ";
				//out << "polohaObjektu: " << object.polohaObjektu << ", ";
				out << "gpsPolohaObjektu: " << object.gpsPolohaObjektu << ", ";
				out << "gps: " << object.gps << ", ";
				out << "));";
				return out;
			}
		};


		class Out {
		public:
			Point2f polohaTextu;
			bool najdeny;

			friend ostream& operator<< (ostream& out, Out& object) {
				out << "ModulVypocitaniaPolohyOut(";
				out << "polohaTextu: " << object.polohaTextu << ", ";
				out << "najdeny: " << object.najdeny << ", ";
				out << "));";
				return out;
			}
		};


		virtual Out vypocitajPolohuTextu(In in) {
			Out out;
			out.polohaTextu.x = 0.5f; // cca v strede obrazovky
			out.polohaTextu.y = 0.5f;
			out.najdeny = false; // alebo nahodu sme to nevypocitali a poloha sa neda vypocitat
			return out;
		}

	};
}



