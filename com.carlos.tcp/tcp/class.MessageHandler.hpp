#pragma once
#include <carlos_global.h>
class SocketListener;

/**
* Trieda reprezentuju interface, ktory sa spusti
* ked server prijme urcitu spravu. Cez tento interface sa mozme
* priamo opytat, ktory listener = klient poslal spravu.
* Rovnako takto mu mozme aj odpvoedat.
*/
class MessageHandler {
private:
	SocketListener* listener; /**< socket na klienta */

public:
	// Interface zachytava spravy
	virtual void HandleMessage(char* xml) = 0;


	void setListener(SocketListener* listener) {
		this->listener = listener;
	}
	SocketListener* getListener() {
		return listener;
	}
};