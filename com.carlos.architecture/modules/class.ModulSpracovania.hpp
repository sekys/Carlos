#pragma once
#include <carlos_global.h>
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"

namespace Architecture
{
	/**
	* Modul spracovania reality, ktory ma za ulohu
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
			cv::Mat horizont;

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
			//cout << "Dostal som\n\n" << in<< "\n\n";

			// Hladam na obrazovky a ked najdem objekt ulozim ho + jeho poziciu
			DetekovanyObjekt najdenyObjekt;
			if(in.recepts.size() > 1 ) {
				najdenyObjekt.objekt = in.recepts.at(0);
			}
			najdenyObjekt.boundary = RotatedRect(Point2f( 100, 100 ), Size( 50 ,  50 ), 0); // roi

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



