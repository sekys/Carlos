#pragma once
#include <windows.h>

namespace Architecture
{

	class Time
	{
	private: 
		LONGLONG m_llQPFTicksPerSec;

		Time(const Time& a);  
		Time& operator=(const Time& a); 
		Time() {
			LARGE_INTEGER qwTicksPerSec = { 0 };
			QueryPerformanceFrequency( &qwTicksPerSec );
			m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
		}

	public:
		static Time& getInstance() {
			static Time singleton; 
			return singleton; 
		}; 

		inline double GetAbsolute()	{
			LARGE_INTEGER qwTime = { 0 };
			QueryPerformanceCounter( &qwTime );
			double fTime = qwTime.QuadPart / ( double )m_llQPFTicksPerSec;
			return fTime;
		}
	};
}