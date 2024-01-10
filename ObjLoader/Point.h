#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <glm/glm.hpp>
#include <GL/freeglut.h>
#include <cmath>
#include <soil.h>
#include "Face.h"
#include <glm/gtc/type_ptr.hpp>
class Point
{
public:
	GLfloat x, y, z;
	Point(GLfloat x, GLfloat y, GLfloat z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

