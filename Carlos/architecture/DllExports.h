#ifndef DLLEXPORTS_H
#define DLLEXPORTS_H

#define IMPEXP __declspec(dllexport)

extern "C"
IMPEXP void* callFactory();

#endif