#include <windows.h>

class App {
protected:
	bool m_runnig;

	virtual void Init() = 0;
	virtual bool Run()  = 0;
	virtual void Refresh()  {}

	virtual void MainCycle();
public:
	App();

	virtual void start();
	bool canRun();
	void stop();
};