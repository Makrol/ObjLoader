
#include <GL/freeglut.h>
#include <cmath>
#include "Model.h"

float cameraX = 0.0f;
float cameraZ = 5.0f;
float cameraAngle = 0.0f;
float cameraRotationSpeed = 0.1f;
Model model;

void init() {
    glShadeModel(GL_FLAT); // Użyj płaskiego cieniowania dla prostoty
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    // Zdefiniuj właściwości materiału (przykładowe parametry)
    GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat shininess = 64.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 10.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraX, 0.0, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Ustaw pozycję światła
    GLfloat lightPosition[] = { 1.0, 1.0, 1.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    model.render();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'a') {
        cameraAngle += cameraRotationSpeed;
        cameraX = 5.0f * cos(cameraAngle);
        cameraZ = 5.0f * sin(cameraAngle);
    }
    else if (key == 'd') {
        cameraAngle -= cameraRotationSpeed;
        cameraX = 5.0f * cos(cameraAngle);
        cameraZ = 5.0f * sin(cameraAngle);
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    model.loadFile("dfd");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Camera Rotation with GLUT");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    init(); // Wywołaj funkcję inicjalizacyjną
    glutMainLoop();
    return 0;
}