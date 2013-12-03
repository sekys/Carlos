#pragma once
#include <gl/glew.h>
#include <gl/glut.h>
#include "../Hra/class.Scene.hpp"


extern Scene scene;
int oldTimeSinceStart = 0;

/** 
 * Funkcia ma na vstupe stlacenu klavesu a poziciu lietadla na osiach x a y, 
 *nacita potrebne premenne, pripravi scenu a updatne display
 * @param key - stlacena klavesa
 * @param x - pozicia lietadla na osi x
 * @param y - pozicia lietadla na osi y
 * @return nil
 */
void keyboard(unsigned char key, int x, int y)
{
	scene.keyboard(key, x, y);
	glutPostRedisplay();
}

/** 
 * Funkcia nema na vstupe ziadne parametre, sluzi na pocitanie casu a vytvorenie framu
 * @return nil
 */
void renderScene() {
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime;
	float fDelta;
	if(oldTimeSinceStart == 0) {
		fDelta = 0.0;
	} else {
		deltaTime = timeSinceStart - oldTimeSinceStart;
		fDelta = (float) deltaTime / 1000.f;
	}
	oldTimeSinceStart = timeSinceStart;
	scene.frame(fDelta);
}

/** 
 * Funkcia ktora sa stara o inicializovanie kniznice openGl, nastavuje sa tu
 * vsetko potrebne od nazvu okna az po rozmery okna
 * @see MojaTrieda::init()
 * @return void
 */
void openGLInit() {
	char fakeParam[] = "fake";
	char *fakeargv[] = { fakeParam, NULL };
	int fakeargc = 1;

	glutInit( &fakeargc, fakeargv );
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Carlos");
	glewInit() ;

	glMatrixMode(GL_PROJECTION);
	gluPerspective(45, 1, 0.1, 100);
	gluLookAt(0.f, 10.f, 0.01f, 0.0f, 0.0f, 0.0f, 0, 1, 0);

	scene.init();
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glutDisplayFunc(renderScene);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
}