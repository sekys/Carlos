// http://www.drdobbs.com/jvm/java-and-c-socket-communication/222900697?pgno=3

#include "SocketListener.h"
#include <process.h> 
#include <stdlib.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib") // link winsock2

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
			recvbuf[iResult-1] = 0; // null terminate the string according to the length

			// The message spec indicates the XML will end in a "new line"
			// character which can be either a newline or caraiage feed
			// Search for either and replace with a NULL to terminate
			if ( recvbuf[iResult-2] == '\n' || recvbuf[iResult-2] == '\r' )
				recvbuf[iResult-2] = 0;

			if(handler == NULL) {
				cout << "handler is null\n";
				return 1;
			}
			handler->HandleMessage(recvbuf);
		}
		else {
			cout << "Client connection closing\n";
			closesocket(ClientSocket);
			return 1;
		}
	}

	return 0;
}

void SocketListener::sendTEXT2Client(const char* txt) {
	// Send some XML to the client
	int len = strlen(txt);
	cout << "Sending\n" << txt;
	int sent = send( this->ClientSocket, txt, len, 0 );

	if ( sent == SOCKET_ERROR ) {
		cout << "Send failed.\n";
		closesocket( this->ClientSocket );
		return;
	}
}