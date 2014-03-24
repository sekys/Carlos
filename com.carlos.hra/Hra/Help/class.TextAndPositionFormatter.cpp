#include "class.TextAndPositionFormatter.h"

void TextAndPositionFormatter::formatPosition(
		Point2f &pos, Point2f &newPos, unsigned int line, 
		unsigned int numOfLines, unsigned int maxCharsOnLine,
		unsigned int windowWidth, unsigned int windowHeight
	) {
	
	if (windowWidth - (unsigned int) pos.x < maxCharsOnLine*FONT_WIDTH_IN_PX) {
		newPos.x = windowWidth - maxCharsOnLine*FONT_WIDTH_IN_PX;
	} else {
		newPos.x = pos.x < 0. ? 0. : pos.x;
 	}

	if (windowHeight - (unsigned int) pos.y < numOfLines*FONT_HEIGHT_IN_PX) {
		newPos.y = windowHeight - numOfLines*FONT_HEIGHT_IN_PX;
	} else {
		newPos.y = pos.y < 0. ? 0. : pos.y;
 	}

	newPos.y = newPos.y + line*FONT_HEIGHT_IN_PX;
}

void TextAndPositionFormatter::formatTextToLines(const char *s, vector<string> &lines) {
	if (s == NULL) {
		return;
	}

	const char *start = s, *c = s;
	unsigned int i = 1, numOfLines = 0;

	for (; *c && numOfLines < MAX_NUM_OF_LINES; c++) {
		if (i == MAX_CHARS_ON_LINE) {
			string line;
			for (const char *n = start; n <= c; n++) {
				line.append(1, *n);
			}
			lines.push_back(line);
			start = c + 1;
			i = 1;
			numOfLines++;
		} else {
			i++;
		}
	}

	if (start && numOfLines < MAX_NUM_OF_LINES) {
		string line;
		for (const char *n = start; n < c; n++) {
			line.append(1, *n);
		}
		lines.push_back(line);
	}
}

unsigned int TextAndPositionFormatter::countMaxCharsOnLine(vector<string> lines) {
	int maxCharsOnLine = 0;

	for (int i=0; i<lines.size(); i++) {
		if (lines[i].length() > maxCharsOnLine) {
			maxCharsOnLine = lines[i].length();
		}
	}

	return maxCharsOnLine;
}