#pragma once
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"

namespace Architecture
{
	/**
	* Modul vykreslvoania, ktory vykresli rozsirenu realitu
	*/
	class ModulVykreslovania : public IModul  {
	public:
		class In {
		public:
			Image image; /**< obrazok povodny z kamery */
			vector<Point2f> najdenePozicie; /**< Pozicie najdene pre objekty */
			ControllerCommands command;

			friend ostream& operator<< (ostream& out, In& object) {
				out << "ModulVykreslovaniaIn(";
				out << "image: " << object.image << ", ";
				out << "najdenePozicie: (";
				for(uint i=0; i < object.najdenePozicie.size(); i++) {
					out << object.najdenePozicie.at(i) << ",";
				}
				out << "));";
				return out;
			}
		};

		// Posli modulu spravu, ze ma vykreslit obrazok z rozsirenou realitou
		virtual void vykresliObrazokSRozsirenouRealitou(In in) {
			Image obrazokNaPozadie = in.image; // tento obrazok je potrebne vykreslit na pozadi

			for(uint i=0; i < in.najdenePozicie.size(); i++) {
				Point2f poziciaTextu = in.najdenePozicie.at(i);
			}
		}
	};
}



