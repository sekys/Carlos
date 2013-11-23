// http://www.drdobbs.com/jvm/java-and-c-socket-communication/222900697?pgno=3

#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include "class.MessageHandler.hpp"
#define DEFAULT_BUFLEN 512

class SocketListener {
protected:
    WSADATA wsaData;
    SOCKET ClientSocket;
    struct addrinfo *result;
	struct addrinfo *hints;
    char recvbuf[DEFAULT_BUFLEN];
    int iResult, iSendResult;
    int recvbuflen;
	bool listening;
	MessageHandler* handler;

public:
	SocketListener(SOCKET socket);
	DWORD start();
	void sendTEXT2Client(const char* txt);
	void setHandler(MessageHandler* mess);
	bool isListening();
};

