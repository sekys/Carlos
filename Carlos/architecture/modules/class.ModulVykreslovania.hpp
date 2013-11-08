#pragma once
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"

namespace Architecture
{
	class ModulVykreslovania : public IModul  {
	public:
		class In {
		public:
			Image image;
			vector<Position> najdenePozicie; // pozicie pre text

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

		virtual void vykresliObrazokSRozsirenouRealitou(In in) {
			Image obrazokNaPozadie = in.image; // tento obrazok je potrebne vykreslit na pozadi

			for(uint i=0; i < in.najdenePozicie.size(); i++) {
				Position poziciaTextu = in.najdenePozicie.at(i);
			}
		}
	};
}



