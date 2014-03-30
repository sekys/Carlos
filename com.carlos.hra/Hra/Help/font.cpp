#include "font.h"

GLuint	base; /**< Vytvoreny font */

/** 
* Funkcia nema na vstupe ziadny parameter, sluzi na vytvorenie fontu
* @see Scene::init()
* @return GLvoid
*/
GLvoid buildFont()
{
	HFONT	font;
	HFONT	oldfont;

	base = glGenLists(96);

	font = CreateFont( -24,
		0,
		0,
		0,
		FW_BOLD,
		FALSE,
		FALSE,
		FALSE,
		ANSI_CHARSET,
		OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE|DEFAULT_PITCH,
		"Courier New");

	HDC__* hDC = wglGetCurrentDC();
	oldfont = (HFONT)SelectObject(hDC, font);
	wglUseFontBitmaps(hDC, 32, 96, base);
	SelectObject(hDC, oldfont);
	DeleteObject(font);
}

GLvoid killFont(GLvoid)
{
	glDeleteLists(base, 96);
}

/** 
* Funkcia sa stara o vypisovanie textu
* @see Scene::stavHrania(float fDelta)
* @return GLvoid
*/
GLvoid glPrint(const char *fmt, ...)
{
	char		text[256];
	va_list		ap;

	if (fmt == NULL)
		return;

	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);

	glPushAttrib(GL_LIST_BIT);
	glListBase(base - 32);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

void printLineOfText(const char *str, int x, int y) {
	glUseProgram(0);
	VisualController::pushScreenCoordinateMatrix();
	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);	
	glDisable(GL_LIGHTING);

	glPushMatrix();
	glLoadIdentity();

	glColor3ub(255,255,255);
	glRasterPos2f(x, y);
	glPrint(str);
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glPopAttrib();
	VisualController::popProjectionMatrix();
}
