#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\nonfree\features2d.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\nonfree\nonfree.hpp>
#include <opencv2\nonfree\gpu.hpp>
#include <opencv2\legacy\legacy.hpp>
#include <opencv2\gpu\gpu.hpp>
#pragma once
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"

namespace Architecture
{
	class ModulKalibracie : public IModul  {
	public:
		class In {
		public:
			Image image1;
			Image image2;

			friend ostream& operator<< (ostream& out, In& object) {
				out << "ModulKalibracieIn(Image: " << object.image1 << ";";
				return out;
			}
		};

		class Out {
		public:
			cv::Mat H;

			friend ostream& operator<< (ostream& out, Out& object) {
				out << "ModulKalibracieOut(H :(";
				out << "));";
				return out;
			}
		};

		virtual Out kalibruj(In in) {
			// Hladam na obrazovky a ked najdem objekt ulozim ho + jeho poziciu
			cv::Mat mat;

			// Poslem vysledok dalej
			Out out;
			out.H = mat;
			return out;
		}
	};
}



