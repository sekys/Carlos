#pragma once
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"

namespace Architecture
{
	/**
	* Modu lspracovania reality, ktory ma za ulohu
	* spracovat snimku a detekovat na nej objekty.
	*/
	class ModulSpracovania : public IModul  {
	public:

		/**
		* Tu su definovane vstupy do modulu
		*/
		class In {
		public:
			Image image; /**< snimka z ktorej sa detekuje */
			vector<WorldObject> recepts; /**< zoznam svetkovych objektov */

			friend ostream& operator<< (ostream& out, In& object) {
				out << "ModulSpracovaniaIn(Image: " << object.image << ", recepts :(";
				for(uint i=0; i < object.recepts.size(); i++) {
					out << object.recepts.at(i) << ",";
				}
				out << "));";
				return out;
			}
		};

		/**
		* Tu su definovane vystupy tohto modulu
		*/
		class Out {
		public:
			vector<DetekovanyObjekt> objects; /**< zoznam detekovanych objektov */

			friend ostream& operator<< (ostream& out, Out& object) {
				out << "ModulSpracovaniaOut(objects :(";
				for(uint i=0; i < object.objects.size(); i++) {
					out << object.objects.at(i) << ",";
				}
				out << "));";
				return out;
			}
		};


		// Spusti detekciu objektov na zakalde vstupu
		virtual Out detekujObjekty(In in) {
			// Hladam na obrazovky a ked najdem objekt ulozim ho + jeho poziciu
			DetekovanyObjekt najdenyObjekt;
			if(in.recepts.size() > 1 ) {
				najdenyObjekt.objekt = in.recepts.at(0);
			}

			// Poslem vysledok dalej
			Out out;
			out.objects.push_back(najdenyObjekt);
			return out;
		}

	};
}



