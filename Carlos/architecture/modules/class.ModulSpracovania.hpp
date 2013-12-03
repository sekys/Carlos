#pragma once
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"

namespace Architecture
{
	class ModulSpracovania : public IModul  {
	public:
		class In {
		public:
			Image image;
			vector<WorldObject> recepts;

			friend ostream& operator<< (ostream& out, In& object) {
				out << "ModulSpracovaniaIn(Image: " << object.image << ", recepts :(";
				for(uint i=0; i < object.recepts.size(); i++) {
					out << object.recepts.at(i) << ",";
				}
				out << "));";
				return out;
			}
		};

		class Out {
		public:
			vector<DetekovanyObjekt> objects;

			friend ostream& operator<< (ostream& out, Out& object) {
				out << "ModulSpracovaniaOut(objects :(";
				for(uint i=0; i < object.objects.size(); i++) {
					out << object.objects.at(i) << ",";
				}
				out << "));";
				return out;
			}
		};


		virtual Out detekujObjekty(In in) {
			cout << "Dostal som\n\n" << in<< "\n\n";

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



