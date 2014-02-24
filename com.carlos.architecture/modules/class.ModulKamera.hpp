#pragma once
#include <carlos_global.h>
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
		Image frame; /**< Aktualna snimak z maery, tato snimka sa moze preskocit, nemusi sa spracovat */

	public:

		class EndOfStream : public std::exception {
		public:
			EndOfStream() : std::exception("EndOfStream") {}
		};

		ModulKamera(const string& filename) {
			cap = new VideoCapture(filename);
		}

		ModulKamera(int device) {
			cap = new VideoCapture(device);
		}

		// Zapni kameru
		virtual void init() {
			if(!cap->isOpened()) {
				throw runtime_error("[ModulKamera] I cannot open device / file.");
			}
			frame.frame = -1;
		}

		~ModulKamera() {
			if(cap->isOpened()) {
				cap->release();
			}
			SAFE_DELETE(cap);
		}

		// Ziskaj aktualnu snimku z kamery
		virtual void readNext() {
			if(!cap->read(frame.data)) {
				throw EndOfStream();
			}
			frame.frame = cap->get(CV_CAP_PROP_POS_FRAMES);
		}

		Image getImage() {
			return frame;
		}
	};
}



