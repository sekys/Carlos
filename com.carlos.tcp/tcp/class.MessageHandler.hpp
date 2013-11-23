#pragma once

class SocketListener;

class MessageHandler {
private:
	SocketListener* listener;

public:
	virtual void HandleMessage(char* xml) = 0;

	void setListener(SocketListener* listener) {
		this->listener = listener;
	}
	SocketListener* getListener() {
		return listener;
	}
};