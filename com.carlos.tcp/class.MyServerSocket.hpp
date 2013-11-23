#pragma once
#include "tcp\ServerSocket.h"

class MyServerSocket : public ServerSocket {
private:
	MessageHandler* handler;

public:
	MyServerSocket(MessageHandler* handler, const char* ip, const char* port) : ServerSocket(ip, port) {
		this->handler = handler;
	}
	virtual SocketListener* buildListener(SOCKET socket) {
		SocketListener* listener = new SocketListener(socket);
		handler->setListener(listener);
		listener->setHandler(handler);
		return listener;
	}
};