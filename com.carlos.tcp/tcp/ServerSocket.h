// http://www.drdobbs.com/jvm/java-and-c-socket-communication/222900697?pgno=3

#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include "SocketListener.h"

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
	ServerSocket(const char* ip, const char* port);
	~ServerSocket();

	SocketListener* getListener();
	DWORD start();
	bool isListening();
};


