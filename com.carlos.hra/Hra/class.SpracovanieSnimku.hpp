/** @file class.SpracovanieSnimku.hpp
* Trieda ktora sa stara o spracovanie snimkov
*/
#pragma once
#include "opencv2/opencv.hpp"

/**
* Horizont sa dokresluje postupne do obrazka
* V hernej logike sa zistuje horizont tym, ze sa prechadza snimok a porovnavaju sa farby na pozadi
* Teda lietadlo sa nesmie dotknut definovanej farby - skupiny farieb
*/
class SpracovanieSnimku {
private:
	static cv::Mat prehodVideo(cv::Mat bg) {
		
		return bg;
	}


public:
	static cv::Mat vypocetHorizontu(cv::Mat bg, cv::Mat horizont) {
		cv::Mat lol;
		return horizont;
	}

};

