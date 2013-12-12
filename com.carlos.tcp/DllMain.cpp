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

/**
* Modul ktory riadi TCP server a napoji tento server.
* Tento modul je potom napojeny na halvnu apliakciu cez DLL subor.
*/
class TCPServer : public FakeModulAndroid, MessageHandler {
private:
	ServerSocket* server;

public:
	// Nas modul ma zatial nacitavat fake GPS suranice zo suboru 
	TCPServer() : FakeModulAndroid("data/video/2013-10-20-12-25-52.txt") {
		server = NULL;
	}

	// Nas modul sa spusta
	virtual void init() {
		FakeModulAndroid::init();
		cout << "Native TCP server starting\n";
		server = new MyServerSocket(this, "192.168.247.222", "1234");
		server->start();
	}

	// Zachytavam spravy od clienta
	void HandleMessage(char* input) {
		string inputText = input;
		cout << "Reading\n" + inputText + "\n";
		size_t pos = inputText.find(":");
		if(pos == -1) {
			cout << "Sprava nieje spravne formatovana.\n";
			return;
		}
		string key = inputText.substr(0, pos);
		string value = inputText.substr(pos+1, inputText.length()); 

		// Citaj prikaz
		if (key.compare("Command") != 0) {
			//sendRandomNumberResponse();
			updateCommand(value); // TODO: tu ostava :
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
	~TCPServer() {
		if(server != NULL) {
			delete server;
		}
	}

	// Ma byt moodu lspusteny v samostatnom vlakne ?
	virtual bool isThreaded() { return true; }
};

// Ked nas modul ma byt vytvoreny cez riadiaci modul
IMPEXP void* callFactory() {
	return static_cast< void* > (new TCPServer());
}

// Ked chceme modul spustit ako normalnu aplikaciu
int main() { 
	TCPServer* a = new TCPServer();
	a->init();
	delete a;
}

// Ked nas modul je nacitany cez riadiaci modul
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*) {
	printf("Dll nacitane\n");
	return 1;
}