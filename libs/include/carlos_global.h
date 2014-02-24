// Subor sa inkluduje do kazdeho suboru
#ifndef CARLOS_GLOBAL_H
#define CARLOS_GLOBAL_H

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;


#endif