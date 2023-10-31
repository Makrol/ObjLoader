
#define GLEW_STATIC
#include <stdio.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

 // Animacja:
GLfloat RotX, RotY;

// Wsp�rz�dne wierzcho�k�w sze�cianu:
const float CubeVert[] = {
	-1.0f, -1.0f,  1.0f,	 1.0f, -1.0f,  1.0f,	-1.0f,  1.0f,  1.0f,	 1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,	-1.0f,  1.0f, -1.0f,	 1.0f, -1.0f, -1.0f,	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,	-1.0f,  1.0f,  1.0f,	 1.0f,  1.0f, -1.0f,	 1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,	 1.0f, -1.0f, -1.0f,	-1.0f, -1.0f,  1.0f,	 1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,	 1.0f,  1.0f, -1.0f,	 1.0f, -1.0f,  1.0f,	 1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,	-1.0f, -1.0f,  1.0f,	-1.0f,  1.0f, -1.0f,	-1.0f,  1.0f,  1.0f,
};

// Wsp�rz�dne tekstury dla jednostki #0:
const float CubeTex0[] = {
	0.0f, 0.0f,		2.0f, 0.0f,		0.0f, 2.0f,		2.0f, 2.0f,
	2.0f, 0.0f,		2.0f, 2.0f,		0.0f, 0.0f,		0.0f, 2.0f,
	1.0f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 1.0f,
	0.0f, 0.0f,		1.0f, 0.0f,		0.0f, 1.0f,		1.0f, 1.0f,
	1.0f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 1.0f,
	0.0f, 0.0f,		1.0f, 0.0f,		0.0f, 1.0f,		1.0f, 1.0f
};
// Wsp�rz�dne tekstury dla jednostki #1:
const float CubeTex1[] = {
	0.0f, 0.0f,		1.0f, 0.0f,		0.0f, 1.0f,		1.0f, 1.0f,
	1.0f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 1.0f,
	1.0f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 1.0f,
	0.0f, 0.0f,		1.0f, 0.0f,		0.0f, 1.0f,		1.0f, 1.0f,
	1.0f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 1.0f,
	0.0f, 0.0f,		1.0f, 0.0f,		0.0f, 1.0f,		1.0f, 1.0f
};

// Indeksy �cian sze�cianu:
const unsigned char CubeIndices[] = {
	0, 1, 2,    // strona 1
	2, 1, 3,
	4, 5, 6,    // strona 2
	6, 5, 7,
	8, 9, 10,    // strona 3
	10, 9, 11,
	12, 13, 14,    // strona 4
	14, 13, 15,
	16, 17, 18,    // strona 5
	18, 17, 19,
	20, 21, 22,    // strona 6
	22, 21, 23,
};

bool LoadTextureFromFile(const char* FileName) {
	// Parametry tekstury i wska�nik:
	int width, height, nchan;
	unsigned char* dt;
	dt = stbi_load(FileName, &width, &height, &nchan, 0);
	if (!dt) 	return false;
	// RGB = 3 kana�y:
	if (nchan != 3) {
		stbi_image_free(dt);
		return false;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, dt);
	// Filtrowanie bez mipmap:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(dt);
	// Ok:
	return true;
}

// Dwie tekstury:
GLuint ColorTex, ShadeTex;

void SetupTextures() {
	// Tekstura "zwyk�a":
	glGenTextures(1, &ColorTex);
	glBindTexture(GL_TEXTURE_2D, ColorTex);
	LoadTextureFromFile("tex0.png");
	// Tekstura przyciemnie�:
	glGenTextures(1, &ShadeTex);
	glBindTexture(GL_TEXTURE_2D, ShadeTex);
	LoadTextureFromFile("shadetex.png");
}

void SetupScene() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	// W��czenie teksturowania dla dw�ch jednostek:
	glActiveTexture(GL_TEXTURE0);	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);	glEnable(GL_TEXTURE_2D);
	// Wybranie sposobu miksowania tekstur:
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Tabele ze wsp�rz�dnymi sze�cianu:
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, CubeVert);
	// Wsp�rz�dne dla jednostki #0:
	glClientActiveTexture(GL_TEXTURE0);		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, CubeTex0);
	// Wsp�rz�dne dla jednostki #1:
	glClientActiveTexture(GL_TEXTURE1);		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, CubeTex1);
}

void OnRender() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	// Odsuni�cie kamery:
	glm::mat4 MatM = glm::translate(glm::vec3(0, 0, -4));
	// Animacja:
	MatM = glm::rotate(MatM, glm::radians(RotX), glm::vec3(1, 0, 0));
	MatM = glm::rotate(MatM, glm::radians(RotY), glm::vec3(0, 1, 0));

	glLoadMatrixf(glm::value_ptr(MatM));

	// Ustawienie tekstur:
	glActiveTexture(GL_TEXTURE0);	glBindTexture(GL_TEXTURE_2D, ColorTex);
	glActiveTexture(GL_TEXTURE1);	glBindTexture(GL_TEXTURE_2D, ShadeTex);

	glDrawElements(GL_TRIANGLES, sizeof(CubeIndices), GL_UNSIGNED_BYTE, CubeIndices);
	glutSwapBuffers();
}

void OnResize(int w, int h) {
	if (h == 0)	h = 1;
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glm::mat4 MatP = glm::perspective<float>(glm::radians(50.0f), (float)w / h, 0.25f, 5.0f);
	glLoadMatrixf(glm::value_ptr(MatP));
}

void OnKeyboard(unsigned char key, int x, int y) {
	if (key == 0x1B)	exit(0);	// out
}

void OnIdle() {
	static int last_time;
	// Time in milliseconds:
	int now_time = glutGet(GLUT_ELAPSED_TIME);

	if (last_time > 0) {
		float times = (now_time - last_time) / 1000.0f;
		// Rotacja obiekt�w:
		RotX += 10 * times;
		RotY += 35 * times;
	}
	// Copy for measuring the next frame:
	last_time = now_time;
	// Refresh frame:
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(960, 600);
	glutCreateWindow("OpenGL Multitexture");

	puts((char*)glGetString(GL_RENDERER));

	// Inicjalizacja biblioteki GLEW i sprawdzenie dost�pno�ci OpenGL 1.3:
	if (glewInit() != GLEW_OK) {
		puts("GLEW init FAIL!");
		exit(0);
	}
	if (!GLEW_VERSION_1_3) {
		puts("No OpenGL 1.3+ support!");
		exit(0);
	}

	int TexUnits;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &TexUnits);
	printf("Texture units: %d\n", TexUnits);

	if (TexUnits < 2) {
		puts("*** Too few texture units!");
		exit(0);
	}

	glutDisplayFunc(OnRender);
	glutReshapeFunc(OnResize);
	glutKeyboardFunc(OnKeyboard);
	glutIdleFunc(OnIdle);

	SetupScene();
	SetupTextures();

	glutMainLoop();
	return(0);
}
