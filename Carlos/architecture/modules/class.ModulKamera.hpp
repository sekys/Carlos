#pragma once
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"
#include "opencv2/opencv.hpp"

namespace Architecture
{
	using namespace cv;

	/**
	* Trieda ktora manazuje kameru.
	* Pripoji kameru a prebera stream z nej
	*/
	class ModulKamera : public IModul  {
	private:
		// http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html#videocapture
		VideoCapture* cap;

	public:

		ModulKamera(const string& filename) {
			cap = new VideoCapture(filename);
		}

		ModulKamera(int device) {
			cap = new VideoCapture(device);
		}

		// Zapni kameru
		virtual void init() {
			if(!cap->isOpened()) {
				cout << "[ModulKamera] I cannot open device / file.\n";
			}
		}

		~ModulKamera() {
			SAFE_DELETE(cap);
		}

		// Ziskaj aktualnu snimku z kamery
		virtual void readNext(Image& image) {
			*cap >> image.data;
		}
	};
}



