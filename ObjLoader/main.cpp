#include <fstream>
#include <math.h>
#include <time.h>
#include <vector>

#include <GL/freeglut.h>
#include <GL/GL.h>
#include "Model.h"
Model model;
#define WIDTH 600
#define HEIGHT 600

float cameraAngleX = 30.0f; // Początkowy kąt obrotu kamery wokół osi X
float cameraAngleY = 0.0f;  // Początkowy kąt obrotu kamery wokół osi Y
float cameraDistance = 20.0f;
bool isMousePressed = false;
int lastMouseX, lastMouseY;

void handleMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			isMousePressed = true;
			lastMouseX = x;
			lastMouseY = y;
		}
		else if (state == GLUT_UP)
		{
			isMousePressed = false;
		}
	}
}
void handleMouseWheel(int wheel, int direction, int x, int y)
{
	if (direction > 0) // Przybliżanie
	{
		cameraDistance -= 1.0f;
		if (cameraDistance < 1.0f)
			cameraDistance = 1.0f;
	}
	else if (direction < 0) // Oddalanie
	{
		cameraDistance += 1.0f;
	}

	glutPostRedisplay(); // Oznacz ekran do odświeżenia
}

void handleMouseMove(int x, int y)
{
	if (isMousePressed)
	{
		int deltaX = x - lastMouseX;
		int deltaY = y - lastMouseY;

		cameraAngleY += deltaX * 0.5f;
		cameraAngleX += deltaY * 0.5f;

		lastMouseX = x;
		lastMouseY = y;

		glutPostRedisplay(); // Oznacz ekran do odświeżenia
	}
}
void init() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat light_pos[] = { -1.0f,10.0f,100.0f,1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(20.0, 1.0, 1.0, 2000.0);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -cameraDistance);
	glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f);
	glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);
	model.draw();
	glutSwapBuffers();
}
void reshape(int width, int height)
{
	if (height == 0)
		height = 1;

	float aspectRatio = (float)width / height;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspectRatio, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -20.0f);
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glEnable(GLUT_MULTISAMPLE);
	//glHint(GLUT_)
	glutSetOption(GLUT_MULTISAMPLE, 8);
	int POS_X = (glutGet(GLUT_SCREEN_WIDTH) - WIDTH) >> 1;
	int POS_Y = (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) >> 1;
	glutInitWindowPosition(POS_X, POS_Y);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Load Model");
	init();
	model.loadFile("Mug.obj");
	glutDisplayFunc(display);
	glutMouseFunc(handleMouse);
	glutMouseWheelFunc(handleMouseWheel);
	glutMotionFunc(handleMouseMove);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}
