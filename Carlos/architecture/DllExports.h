/** @file DllExports.h
*
* Tento subor sa ma inkludovat do kazdeho modulu.
* Obsahuje definiciu metody ako sa prepoji modul s halvnou aplikaciou.
*/
#ifndef DLLEXPORTS_H
#define DLLEXPORTS_H

#define IMPEXP __declspec(dllexport)

extern "C"
IMPEXP void* callFactory();

#endif