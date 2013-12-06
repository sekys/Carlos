// http://www.drdobbs.com/jvm/java-and-c-socket-communication/222900697?pgno=3

#include "SocketListener.h"
#include <process.h> 
#include <stdlib.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib") // linker potrebuje winsock2 ktory je v ws2_32

using namespace std;

SocketListener::SocketListener(SOCKET socket) {
	recvbuflen = DEFAULT_BUFLEN;
	this->ClientSocket = socket;
}

void SocketListener::setHandler(MessageHandler* mess) {
	handler = mess;
}

bool SocketListener::isListening() {
	return listening;
}

DWORD SocketListener::start() {
	listening = true;
	while ( listening ) {
		int iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			cout << "Bytes received: " << iResult << "\n";
			// Ukonci retazec na zakalde prijatej dlzky

			recvbuf[iResult] = 0;

			if(handler == NULL) {
				cout << "handler is null\n";
				return 1;
			}

			// Posial spravu dalej na handler
			handler->HandleMessage(recvbuf);
		}
		else {
			// Client sa odpojil
			cout << "Client connection closing\n";
			closesocket(ClientSocket);
			return 1;
		}
	}

	return 0;
}

void SocketListener::sendTEXT2Client(const char* txt) {
	// Metoda pre poslanie spravy cleintovy
	int len = strlen(txt);
	cout << "Sending\n" << txt;
	int sent = send( this->ClientSocket, txt, len, 0 );

	if ( sent == SOCKET_ERROR ) {
		cout << "Send failed.\n";
		closesocket( this->ClientSocket );
		return;
	}
}