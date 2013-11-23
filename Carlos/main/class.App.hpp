#include <windows.h>

class App {
protected:
	bool m_runnig;

	virtual void Init() = 0;
	virtual bool Run()  = 0;
	virtual void Refresh()  {}

	virtual void MainCycle() {
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

public:
	App() {

	}

	virtual void start() {
		Init();
		m_runnig = true;
		MainCycle();
	}

	bool canRun() {
		return m_runnig;
	}
	void stop() {
		m_runnig = false;
	}
};