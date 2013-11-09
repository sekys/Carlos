#ifndef DLLEXPORTS_H
#define DLLEXPORTS_H

#ifdef __dll__
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

extern "C"
IMPEXP void* callFactory();

#endif