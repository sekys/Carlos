// http://www.drdobbs.com/jvm/java-and-c-socket-communication/222900697?pgno=3

#pragma once
#include <carlos_global.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "SocketListener.h"

/**
* Trieda ktorra reprezentuje server.
* V ramci toho trieda sa stara o spustenie servera, jeho zastavenie, jeho konfiguraciu.
* Server bezi nad socketom. Trieda sa stara aj o riadenie tohto socketu.
*/
class ServerSocket {
protected:
	WSADATA wsaData;
	SOCKET ListenSocket, ClientSocket;
	struct addrinfo *result;
	struct addrinfo hints;
	int iResult, iSendResult;
	bool listening;
	SocketListener* listener;
	const char* ip;
	const char* port;
	virtual SocketListener* buildListener(SOCKET socket) = 0;

public:
	// Nakonfiguruj server na urcitej ip adrese a porte
	ServerSocket(const char* ip, const char* port);

	// Vypni server
	~ServerSocket();

	// Ziskaj socket, nad ktorom bezi server
	SocketListener* getListener();

	// Spusti server
	DWORD start();

	// Je server v mode, ze caka na prihalsenie klineta ?
	bool isListening();
};


