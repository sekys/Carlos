#pragma once
#include <carlos_global.h>
#include "../../entities/entities.h"
#include <iostream>
#include <fstream>

// Riadok zaznamu: 48.89561101, 18.07015561, 2013-10-20-12-25-52
// Prvy riadok: 48.89561101, 18.07015561, 2013-10-20-12-25-52
// Posledny riadok: 48.91793117, 18.09643851, 2013-10-20-12-29-51
// 240 riadkov, teda 240 sec
// DLzka videa 4minuty ~ 240sec ...
// Teda gps sa updatuje kazdu 1 sec
// Subor data/video/2013-10-20-12-25-52.avi

using namespace std;
using namespace Architecture;

class GPSRow : public GPS {
public:
	string date;
};

class GPSLogFile {
private:
	ifstream file;

public:
	GPSLogFile(string filename)  {
		file.open (filename);
	}

	~GPSLogFile() {
		if (file.is_open()) {
			file.close();
		}
	}

	virtual GPSRow readNext() {
		GPSRow row;
		file >> row.latitude;
		file.get(); // Preskoc 2 znaky
		file.get();
		file >> row.longitude;
		file.get(); // Preskoc 2 znaky
		file.get();
		file >> row.date;
		return row;
	}
};