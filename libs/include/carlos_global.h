// Subor sa inkluduje do kazdeho suboru
#ifndef CARLOS_GLOBAL_H
#define CARLOS_GLOBAL_H

// Debugovanie memory leak
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

// Compiler konstanty
#define CARLOS_DEBUG_OPENGL false

// Pomocne makra
#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;


#endif