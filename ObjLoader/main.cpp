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
float cameraAngleX = 30.0f; 
float cameraAngleY = 0.0f;  
float cameraDistance = 20.0f;
bool isMousePressed = false;
int lastMouseX, lastMouseY;
float zoomFactor = 1.0f;

void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			isMousePressed = true;
			lastMouseX = x;
			lastMouseY = y;
		}
		else if (state == GLUT_UP) {
			isMousePressed = false;
		}
	}
}

void mouseWheel(int button, int dir, int x, int y) {
	if (dir > 0) {
		zoomFactor += 0.1f; 
	}
	else if (dir < 0 && zoomFactor > 0.1f) {
		zoomFactor -= 0.1f; 
	}

	glutPostRedisplay();
}

void mouseMotion(int x, int y) {
	if (isMousePressed) {
		int deltaX = x - lastMouseX;
		int deltaY = y - lastMouseY;

		cameraRotationX += deltaY * 0.1f;
		cameraRotationY += deltaX * 0.1f;

		lastMouseX = x;
		lastMouseY = y;

		glutPostRedisplay();
	}
}


const char* vertexShaderSource = R"(
	#version 330 core
	layout(location = 0) in vec3 inPos;
	layout(location = 1) in vec3 inNormal;
	layout(location = 2) in vec2 inTexCoord;

	out vec2 TexCoord;
	out vec3 FragNormal;
	out vec3 FragPos;
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main() {
		gl_Position = projection * view * model * vec4(inPos, 1.0);
		TexCoord = inTexCoord;
		FragNormal = mat3(transpose(inverse(model))) * inNormal;
		FragPos = vec3(model * vec4(inPos, 1.0));
	}
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoord;
    in vec3 FragNormal;
    in vec3 FragPos;
    out vec4 FragColor;
    uniform vec3 lightDir;
    uniform sampler2D textureSampler;

    void main() {
        vec3 lightDir = normalize(lightDir);
        float diff = max(dot(normalize(FragNormal), -lightDir), 0.0);

        vec4 textureColor = texture(textureSampler, TexCoord);

        vec4 color = textureColor;
        color.rgb *= diff;

        FragColor = color;
    }
)";

GLuint shaderProgram;


void init() {

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
GLuint vbo = 0;
GLuint tbo = 0;
GLuint nbo = 0;
GLuint ObjectVAO;
GLuint MatMVPLoc;
void initShader()
{
	glEnable(GL_DEPTH_TEST);

	// Wygenerowanie bufor�w z wierzcho�kami i kolorami i skopiowanie danych:
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model.verticesOut.size(), model.verticesOut.data(), GL_STATIC_DRAW);
	// Analogiczny bufor z normal:

	glGenBuffers(1, &nbo);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model.normalsOut.size(), model.normalsOut.data(), GL_STATIC_DRAW);


	glGenBuffers(1, &tbo);
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model.texCoordsOut.size(), model.texCoordsOut.data(), GL_STATIC_DRAW);

	// Przygotowanie VAO:
	glGenVertexArrays(1, &ObjectVAO);
	glBindVertexArray(ObjectVAO);
	// Index 0 -> wierzcho�ki:
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	// Index 1 -> normal:
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	// Odblokowanie atrybut�w nr 0 i 1:
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);


	//////////////////////////////////////////////////////////////////////////////////////////
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
	////////////////////////////////////////////////////////////////////////////////////////

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
	glUseProgram(shaderProgram);
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, cameraDistance * zoomFactor),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	viewMatrix = glm::rotate(viewMatrix, glm::radians(cameraRotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(cameraRotationY), glm::vec3(0.0f, 1.0f, 0.0f));

	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
		glm::value_ptr(glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f)));

	GLint textureLoc = glGetUniformLocation(shaderProgram, "textureSampler");
	glUniform1i(textureLoc, 0);

	glBindTexture(GL_TEXTURE_2D, model.textureID);

	GLuint lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
	glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, 1.0f, -1.0f));  
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	glBindVertexArray(ObjectVAO);
	glDrawArrays(GL_TRIANGLES, 0, model.verticesOut.size() / 3);
	glBindVertexArray(0);

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
	model.loadFile("mug.obj", shaderProgram, cameraDistance);
	initShader();
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);
	glutMouseWheelFunc(mouseWheel);
	glutMainLoop();
	return 0;
}