#pragma once
#include "CThread.h"

namespace Architecture
{
	// Model z pohladu halvnej aplikacie
	// oddelim tak vlakna pre nich
	// ale nebudu mat moznost synchronizacie
	// moznost synchronizacie bude v gettri a settri ? jj
	class Modul : public CThread  {
	private:
		IModul* modul;
		bool initCalled;
		HANDLE getThread();
		DWORD m_ThreadFunc();

		HANDLE ServerSocket::getThread() {
			return this->m_hThread;
		}


	public:
		IModul() {
			initCalled = false;
		}

		template <typename T> T* addDll(string file) {
			Dll* lib = new Dll(file);
			importedDlls.push_back(lib);			
			return lib->callFactory<T>();
		}

		void start() {
			std::string name = typeid(*this).name();
			if(initCalled) {
				cout << name << " uz so mraz spustil\n";
			} else {
				if(isThreaded()) {
					cout << name << " spustam v samostatnom vlakne.\n";
				} else {
					cout << name << " spustam v hlavnom vlakne.\n";
					init();
				}
				initCalled = true;
			}
		}


		// Toto mozte nahradit
		virtual void init();
		virtual bool isThreaded() = 0;
		virtual ~IModul() {}
	};
}