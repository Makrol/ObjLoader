#define GLEW_STATIC
#include <math.h>
#include <time.h>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/GL.h>
#include "Model.h"
Model model;
#define WIDTH 600
#define HEIGHT 600
float cameraRotationX = 30.0f;
float cameraRotationY = 0.0f;
float cameraAngleX = 30.0f; // Początkowy kąt obrotu kamery wokół osi X
float cameraAngleY = 0.0f;  // Początkowy kąt obrotu kamery wokół osi Y
float cameraDistance = 20.0f;
bool isMousePressed = false;
int lastMouseX, lastMouseY;

const char* vertexShaderSource = R"(
    #version 330 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal; // Dodaj nowy atrybut dla normalnych
out vec2 TexCoord;
out vec3 FragNormal; // Przekaż normalne do shadera fragmentów
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(inPos, 1.0);
    TexCoord = inTexCoord;
    FragNormal = mat3(transpose(inverse(model))) * inNormal; // Przekaż normalne do shadera fragmentów
}
)";

const char* fragmentShaderSource = R"(
    #version 330 core
	in vec2 TexCoord;
	in vec3 FragNormal; // Przekaż normalne z shadera wierzchołków
	out vec4 FragColor;
	uniform vec3 lightPos; // Uwzględnij pozycję światła

	void main() {
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(normalize(FragNormal), lightDir), 0.0);

		// Modyfikuj kolor fragmentu na podstawie oświetlenia
		vec4 color = vec4(1.0, 0.0, 0.0, 1.0); // Zawsze czerwony kolor
		color.rgb *= diff; // Uwzględnij oświetlenie difuzji

		FragColor = color;
	}
)";

GLuint shaderProgram;

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

		cameraRotationY += deltaX * 0.5f;
		cameraRotationX += deltaY * 0.5f;

		lastMouseX = x;
		lastMouseY = y;

		glutPostRedisplay(); // Oznacz ekran do odświeżenia
	}
}
void init() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat light_pos[] = { -1.0f,-1.0f,-1.0f,.0f };
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
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}
void initShader()
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Sprawdzenie błędów kompilacji shadera wierzchołków
	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
	}

	// Kompilacja shadera fragmentów
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Sprawdzenie błędów kompilacji shadera fragmentów
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
	}

	// Stworzenie programu shaderowego i przypisanie shaderów
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Sprawdzenie błędów linkowania programu shaderowego
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
	}

	// Usunięcie nieużywanych shaderów
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -cameraDistance);
	glRotatef(cameraRotationX, 1.0f, 0.0f, 0.0f);
	glRotatef(cameraRotationY, 0.0f, 1.0f, 0.0f);
	glUseProgram(shaderProgram);

	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));  // Ustawienie jednostkowej macierzy jako model

	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(lookAt(glm::vec3(0.0f, 0.0f, cameraDistance), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))));  // Ustawienie macierzy view

	GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f)));  // Ustawienie macierzy projekcji
	model.draw();
	glUseProgram(0);
	
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
	glewInit();
	initShader();
	model.loadFile("Mug.obj",shaderProgram,cameraDistance);
	glutDisplayFunc(display);
	glutMouseFunc(handleMouse);
	glutMouseWheelFunc(handleMouseWheel);
	glutMotionFunc(handleMouseMove);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}