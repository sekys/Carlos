/** @file ServerSocket.cpp
*
* Implementacia servera a prijmanie soketov.
*/
// http://www.drdobbs.com/jvm/java-and-c-socket-communication/222900697?pgno=3
#include <carlos_global.h>
#include "ServerSocket.h"
#include <process.h> 
#include <iostream>

using namespace std;

ServerSocket::ServerSocket(const char* ip, const char* port) {
	log = CREATE_LOG4CPP();
	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;
	listening = true;
	result = NULL;
	listener = NULL;
	this->ip = ip;
	this->port = port;
}

ServerSocket::~ServerSocket() {

}

bool ServerSocket::isListening() {
	return listening;
}

SocketListener* ServerSocket::getListener() {
	return listener;
};

DWORD ServerSocket::start() {
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		if(log != NULL) {
			log->debugStream() << "WSAStartup failed";
		}
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(ip, port, &hints, &result);
	if ( iResult != 0 ) {
		if(log != NULL) {
			log->debugStream() << "TCP getaddrinfo failed" << ip << ":" << port;
		}
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		if(log != NULL) {
			log->debugStream() << "socket failed " << ip << ":" << port;
		}
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		if(log != NULL) {
			log->debugStream() << "bind failed - zla ip adresa? " << ip << ":" << port;
		}
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		if(log != NULL) {
			log->debugStream() << "listen failed  " << ip << ":" << port;
		}
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	if(log != NULL) {
		log->debugStream() << "Listing for clients on port " << port;
	}
	while ( listening ) {
		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket != INVALID_SOCKET) {
			if(log != NULL) {
				log->debugStream() << "Client connected";
			}

			// Create socket listener and sender to handle this client
			if ( listener != NULL ) {
				try { delete listener; } catch (...) { }
				listener = NULL;
			}

			// Setup the socket sender and listener
			listener = buildListener(ClientSocket);
			if(listener == NULL) {
				if(log != NULL) {
					log->debugStream() << "buildListener return null";
				}
				return 1;
			}
			// listener->startThread();
			listener->start();
			if(log != NULL) {
				log->debugStream() << "listener start has end";
			}
			if(listener != NULL) {
				delete listener;
				listener = NULL;
			}
		}
		else {
			int error = WSAGetLastError();
			if(log != NULL) {
				log->debugStream() << "accept failed";
			}
			switch ( error ) {
			case 10093:
				listening = false;
				try {
					closesocket(ListenSocket);
				}
				catch (...) { }
				return 1;
			}
		}
	}

	return 0;
}

