// http://www.drdobbs.com/jvm/java-and-c-socket-communication/222900697?pgno=3

#pragma once
#include <carlos_global.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "class.MessageHandler.hpp"
#define DEFAULT_BUFLEN 512

/**
* Trieda ktora reprezentuej ssocket medzi serverom a clientom.
* Teda cez socket sa komunikuje.
*/
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
	// Metoda pre zabalenie scoketu
	SocketListener(SOCKET socket);

	// Metoda pre otvorenie socketu
	DWORD start();

	// Metoda na poslanie spravy clientovy
	void sendTEXT2Client(const char* txt);

	// Setter pre nastavenie handlera, pre tento socket, ak bude prazdny prijata sprava sa nikomu neposle
	void setHandler(MessageHandler* mess);

	// Je socket otvoreny ?
	bool isListening();
};

