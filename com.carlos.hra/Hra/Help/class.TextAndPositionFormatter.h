#pragma once

#include <vector>
#include <string>

#include "opencv2/opencv.hpp"

using cv::Point2f;

using std::vector;
using std::string;

#pragma warning(disable: 4786) 

class TextAndPositionFormatter {
public:
	static const unsigned int MAX_CHARS_ON_LINE	= 26;
	static const unsigned int MAX_NUM_OF_LINES	= 6;
	static const unsigned int FONT_HEIGHT_IN_PX	= 17;
	static const unsigned int FONT_WIDTH_IN_PX	= 14;

	void formatPosition(Point2f &, Point2f &, unsigned int, unsigned int, 
		unsigned int, unsigned int, unsigned int);

	void formatTextToLines(const char *, vector<string> &);

	unsigned int countMaxCharsOnLine(vector<string> lines);
};