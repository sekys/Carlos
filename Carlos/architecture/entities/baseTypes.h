/** @file baseTypes.h
*
* V tomto subore sa definuju zakladne typy a definuju makra.
*/
#pragma once
#include <fstream>
#include <ostream>
#include <istream>
#include <string>
#include <vector>

#define setter( x, y )   inline void set##y( x t )   { m_##y = t; };
#define getter( x, y )   inline x get##y()           { return m_##y; };
#define setget( x, y )   setter( x, y ) getter( x, y )					

#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;
#define SINGLETON(name) \
	public: \
			static name& getInstance() {\
				static name singleton; \
				return singleton; \
		  }; \
	private: \
		name(const name&);  \
		name& operator=(const name&); \

// Definuj zakaldne typy
typedef unsigned char byte;
typedef unsigned int uint;