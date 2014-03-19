/** @file font.h
* Trieda Scene ktora sa stara o zobrazenie textu v okne
*/
#pragma once
#include <stdio.h>
#include <windows.h>
#include <stdarg.h>
#include <gl/glew.h>
#include <gl/wglew.h>

extern GLuint	base; /**< Vytvoreny font */

/** 
* Funkcia nema na vstupe ziadny parameter, sluzi na vytvorenie fontu
* @see Scene::init()
* @return GLvoid
*/
GLvoid buildFont();
GLvoid killFont(GLvoid);

/** 
* Funkcia sa stara o vypisovanie textu
* @see Scene::stavHrania(float fDelta)
* @return GLvoid
*/
GLvoid glPrint(const char *fmt, ...);

inline void popProjectionMatrix();
inline void pushScreenCoordinateMatrix();
void printLineOfText(const char * str, int x, int y);