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
			najdenyObjekt.position1.x = 5.f;
			najdenyObjekt.position1.y = 5.f;
			najdenyObjekt.position2.x = 100.f;
			najdenyObjekt.position2.y = 100.f;
			if(in.recepts.size() > 1 ) {
				najdenyObjekt.objekt = in.recepts.at(0);
			}

			// Poslem vysledok dalej
			Out out;
			out.objects.push_back(najdenyObjekt);
			return out;
		}

		virtual cv::Mat kalibruj(cv::Mat image1, cv::Mat image2) {
			cout << "Hladam homografiu ... \n\n";

			// Hladam homografiu medzi dvoma obrazmi a vypocitam maticu transformacie
			cv::Mat H = cv::Mat::zeros(3,3,CV_32F);
			H.at<float>(0,0)= 1.;
			H.at<float>(1,1)= 1.;
			H.at<float>(2,2)= 1.;

			return H;
		}

		virtual cv::Mat najdiHorizont(cv::Mat image){
			cv::Mat horizont = cv::Mat::zeros(image.rows, image.cols, CV_8UC1); 
			cv::rectangle(horizont, cv::Point(0,0), cv::Point(image.cols, image.rows/2), cv::Scalar::all(255), -1);

			return horizont;
		}
	};
}



