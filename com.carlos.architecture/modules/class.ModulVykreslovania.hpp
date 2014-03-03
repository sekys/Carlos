#pragma once
#include <carlos_global.h>
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"
#include "class.ModulVypocitaniaPolohy.hpp"

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
			vector<ModulVypocitaniaPolohy::Out> najdeneObjekty; /**< Najdene objekty */
			ControllerCommands command;
			Mat horizont;

			friend ostream& operator<< (ostream& out, In& object) {
				out << "ModulVykreslovaniaIn(";
				out << "image: " << object.image << ", ";
				out << "command: " << object.command << ", ";
				/*out << "najdeneObjekty: (";
					out << object.najdeneObjekty.at(i) << ",";
				out << object.najdeneObjekty.at(i) << ",";
				}*/
				out << "));";
				return out;
			}

			In() {

			}
			~In() {
				image.data.release();
				horizont.release();
			}
		};

		// Posli modulu spravu, ze ma vykreslit obrazok z rozsirenou realitou
		virtual void vykresliObrazokSRozsirenouRealitou(In* in) {
			/*Image obrazokNaPozadie = in.image; // tento obrazok je potrebne vykreslit na pozadi

			for(uint i=0; i < in.najdeneObjekty.size(); i++) {
				ModulVypocitaniaPolohy::Out najdenyObjekt = in.najdeneObjekty.at(i);
			}*/
		}
	};
}



