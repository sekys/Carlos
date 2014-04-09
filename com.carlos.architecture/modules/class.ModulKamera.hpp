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
		string filename;

		void load() {
			if(filename.empty()) {
				// TODO: VideoCapture nacitava vlastny DLL subor a moze sposobit loader dead lock
				cap = new VideoCapture(0);
			} else {
				cap = new VideoCapture(filename);
			}
			if(!cap->isOpened()) {
				throw runtime_error("[ModulKamera] I cannot open device / file.");
			}
			frame.pos_msec = 0;
		}

		void unload() {
			if(cap != NULL) {
				if(cap->isOpened()) {
					cap->release();
				}
				delete cap;
				cap = NULL;
			}
		}

	public:

		class EndOfStream : public std::exception {
		public:
			EndOfStream() : std::exception("EndOfStream") {}
		};

		ModulKamera(const string filename) {
			this->filename = filename;
			load();
		}

		// Zapni kameru
		virtual void init() {
			// load();
		}

		~ModulKamera() {
			unload();
		}



		// Ziskaj aktualnu snimku z kamery
		virtual Image readNext() {
			if(!cap->read(frame.data)) {
				throw EndOfStream();
			}
			frame.pos_msec = cap->get(CV_CAP_PROP_POS_MSEC);
			return frame;
		}

		virtual void doReset() {
			unload();
			load();
			// cap->set(CV_CAP_PROP_POS_MSEC, 0); toto sposobuje delay a je to tak neefektivne
		}

		/*Image getImage() {
		return frame;
		}*/
	};
}



