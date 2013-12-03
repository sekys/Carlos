#include "class.App.hpp"

void App::MainCycle() {
	MSG msg;
	ZeroMemory( &msg, sizeof(msg) );
	while(canRun()) {
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ) { // prekladame spravy
			TranslateMessage( &msg );  // prekladame spravy
			DispatchMessage( &msg ); // uvolnujeme
		} else {
			if(Run()) {
				Refresh();	// nemozme priamo volat UpdateWindow ale toto mozme ..je take iste
			}
			Sleep(0); // pomahame windowsu sa vyrovnat s hrou
		}
	}
}

App::App() {

}

void App::start() {
	Init();
	m_runnig = true;
	MainCycle();
}

bool App::canRun() {
	return m_runnig;
}
void App::stop() {
	m_runnig = false;
}