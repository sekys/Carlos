#include <carlos_global.h>
#include "../com.carlos.architecture/DllExports.h"
#include "class.MyServerSocket.hpp"
#include "../com.carlos.architecture/utilities.hpp"
#include "../com.carlos.architecture/modules/class.ModulAndroid.hpp"
#include <windows.h>
#include <iostream>
#include <process.h> 
#include <iostream>
#include "../com.carlos.architecture/configuration/class.Configuration.hpp"
#include <log4cpp.h>
#include <log4cpp/PropertyConfigurator.hh>

#ifdef _DEBUG
#pragma comment(lib, "../Debug/com.carlos.architecture.lib")
#else
#pragma comment(lib, "../Release/com.carlos.architecture.lib")
#endif

using namespace Architecture;
using namespace std;

/**
* Modul ktory riadi TCP server a napoji tento server.
* Tento modul je potom napojeny na halvnu apliakciu cez DLL subor.
*/
class TCPServer : public ModulAndroid, MessageHandler {
private:
	ServerSocket* server;
	log4cpp::Category* log;

public:
	// Nas modul ma zatial nacitavat fake GPS suranice zo suboru 
	TCPServer() : ModulAndroid("../data/video/2013-10-20-12-25-52.txt") {
		server = NULL;
		log = CREATE_LOG4CPP();
	}

	// Nas modul sa spusta
	virtual void init() {
		ModulAndroid::init();
		if(log != NULL) {
			log->debugStream() << "Native TCP server starting";
		}
		Configuration& config = Configuration::getInstance();
		server = new MyServerSocket(this, config.getConfigTxt("MY_IP"), config.getConfigTxt("MY_PORT"));
		server->start();
	}

	// Zachytavam spravy od clienta
	void HandleMessage(char* input) {
		string inputText = input;
		if(log != NULL) {
			log->debugStream() << "Reading\n" + inputText + "\n";
		}
		size_t pos = inputText.find(":");
		if(pos == -1) {
			if(log != NULL) {
				log->debugStream() << "Sprava nieje spravne formatovana.";
			}
			return;
		}
		string key = inputText.substr(0, pos);
		string value = inputText.substr(pos+1, inputText.length()); 

		// Citaj prikaz
		if (key.compare("Command") == 0) {
			//sendRandomNumberResponse();
			updateCommand(value); 
			return;
		} 

		// Citaj GPS
		if (key.compare("gps") == 0) {
			vector<string> v;
			split(value, v);
			GPS gps;		
			gps.latitude = stod(v.at(0));
			gps.longitude = stod(v.at(1));
			setGPS(gps);
			return;
		} 

		if(log != NULL) {
			log->debugStream() << "Unkown command from tcp!";
		}
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
		SAFE_DELETE(server)
	}

	// Ma byt moodu lspusteny v samostatnom vlakne ?
	virtual bool isThreaded() { return true; }
};

// Ked nas modul ma byt vytvoreny cez riadiaci modul
IMPEXP void* callFactory() {
	std::string initFileName = "../data/log4cpp.properties";
	log4cpp::PropertyConfigurator::configure(initFileName);
	return static_cast< void* > (new TCPServer());
}

// Ked chceme modul spustit ako normalnu aplikaciu
int main() { 
	TCPServer* a = static_cast<TCPServer*> (callFactory());;
	a->init();
	delete a;
}

// Ked nas modul je nacitany cez riadiaci modul
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*) {
	printf("Dll nacitane\n");
	return 1;
}