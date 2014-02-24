#pragma once
#include <carlos_global.h>
#include "tcp\ServerSocket.h"

/**
* Pretazena trieda ServerSocket, v ktorej povieme
* ze nas server moze prijat len jedneho klienta a ten moze mat otvoreny len jeden soket.
*/
class MyServerSocket : public ServerSocket {
private:
	MessageHandler* handler;

public:
	MyServerSocket(MessageHandler* handler, const char* ip, const char* port) : ServerSocket(ip, port) {
		this->handler = handler;
	}

	// ServerSocket ma factory pattern, je potrebne mu povedat akym sposobom sa budu vytvara sockety
	virtual SocketListener* buildListener(SOCKET socket) {
		SocketListener* listener = new SocketListener(socket);
		handler->setListener(listener);
		listener->setHandler(handler);
		return listener;
	}
};