#include "../Carlos/architecture/DllExports.h"
#include "class.MyServerSocket.hpp"
#include "split.h"
#include "../Carlos/architecture/modules/fake/class.FakeModulAndroid.hpp"
#include <windows.h>
#include <iostream>
#include <process.h> 
#include <iostream>

using namespace Architecture;
using namespace std;

class MojaTrieda : public FakeModulAndroid, MessageHandler {
private:
	ServerSocket* server;

public:
	MojaTrieda() : FakeModulAndroid("data/video/2013-10-20-12-25-52.txt") {
		server = NULL;
	}

	// nas modul sa spusta
	virtual void init() {
		FakeModulAndroid::init();
		cout << "Native TCP server starting\n";
		server = new MyServerSocket(this, "localhost", "8080");
		server->start();
	}

	// Zachytavam spravy od clienta
	void HandleMessage(char* input) {
		string inputText = input;
		cout << "Reading\n" + inputText + "\n";

		unsigned pos = inputText.find(":");
		string key = inputText.substr(pos); 
		string value = inputText.substr(pos, inputText.length()); 

		// Citaj prikaz
		if (key.compare("Command") != 0) {
			//sendRandomNumberResponse();
			updateCommand(value);
			return;
		} 

		// Citaj GPS
		if (key.compare("gps") != 0) {
			vector<string> v;
			split(value, v);
			GPS gps;		
			gps.latitude = stod(v.at(0));
			gps.longitude = stod(v.at(1));
			setGPS(gps);
			return;
		} 

		cout << "Unkown command from tcp!\n";
	}

	// Posielam clientovy odpoved
	/*void sendRandomNumberResponse() {
	char buffer[512];
	buffer[0] = 0;
	sprintf_s(buffer, "Command:5\n");
	getListener()->sendTEXT2Client(buffer);
	}*/

	// Nas modul ma byt vypusteny zo systemu
	~MojaTrieda() {
		if(server != NULL) {
			delete server;
		}
	}

	virtual bool isThreaded() { return true; }
};

// Ked nas modul ma byt vytvoreny cez riadiaci modul
IMPEXP void* callFactory() {
	return static_cast< void* > (new MojaTrieda());
}

// Ked chceme modul spustit ako normalnu aplikaciu
int main() { 
	MojaTrieda* a = new MojaTrieda();
	a->init();
	delete a;
}

// Ked nas modul je nacitany cez riadiaci modul
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*) {
	printf("Dll nacitane\n");
	return 1;
}