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
#include "Point.h"
class Model{
public:
    std::vector<Point> vertices;
    std::vector<Point> normals;
    std::vector<Point> texCoords;


    std::vector<GLfloat> verticesOut;
    std::vector<GLfloat> normalsOut;
    std::vector<GLfloat> texCoordsOut;

    std::string mltName="";
    GLuint textureID;
    
    std::vector<Face> faces;

public:

	void loadFile(std::string fileName, GLuint shaderProgram,float cameraDistance) {

        std::string myText;

        std::ifstream MyReadFile("model/"+ fileName);

        while (getline(MyReadFile, myText)) {
            
            newReadFromLine(myText);
        }
        MyReadFile.close();
	}

private:

    
    void loadTexture()
    {
        if (mltName != "")
        {
            std::string mltText;
            std::ifstream mltReader("model/" + mltName);
            while (getline(mltReader, mltText))
            {
                std::istringstream stream(mltText);
                std::string token;
                stream >> token;
                if (token == "map_Kd")
                {
                    glGenTextures(1, &textureID);
                    glBindTexture(GL_TEXTURE_2D, textureID);
                    stream >> token;
                    int width, height, channels;
                    
                    textureID = SOIL_load_OGL_texture(
                        token.c_str(),
                        SOIL_LOAD_AUTO,
                        SOIL_CREATE_NEW_ID,
                        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
                    );

                    if (textureID == 0) {
                        printf("Error loading texture: %s\n", SOIL_last_result());
                    }  
                }
            }
            mltReader.close();
        }
    }

    void newReadFromLine(std::string line)
    {
        std::istringstream stream(line);
        std::string token;
        float x, y, z;
        stream >> token;
        if (stream.fail()) {
            std::cerr << "B³¹d odczytu ." << std::endl;
        }
        else if (token == "v") {
            glm::vec3 vertex;
            stream >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(Point(vertex.x, vertex.y, vertex.z));
        }
        else if (token == "vn")
        {
            glm::vec3 normal;
            stream >> normal.x >> normal.y >> normal.z;
            normals.push_back(Point(normal.x, normal.y, normal.z));
        }
        else if (token == "vt")
        {
            glm::vec2 uv;
            stream >> uv.x >> uv.y;
            texCoords.push_back(Point(uv.x, uv.y, 0));
        }
        else if (token == "f")
        {
            std::string vertex1, vertex2, vertex3;
            glm::uvec4 vertexIndex, uvIndex, normalIndex;
            glm::uvec3 tmp;
            char slash;
            if ((stream >> vertexIndex.x >> slash >> uvIndex.x >> slash >> normalIndex.x >>
                vertexIndex.y >> slash >> uvIndex.y >> slash >> normalIndex.y >>
                vertexIndex.z >> slash >> uvIndex.z >> slash >> normalIndex.z))
            {
                verticesOut.push_back(vertices[vertexIndex.x - 1].x);
                verticesOut.push_back(vertices[vertexIndex.x - 1].y);
                verticesOut.push_back(vertices[vertexIndex.x - 1].z);

                verticesOut.push_back(vertices[vertexIndex.y - 1].x);
                verticesOut.push_back(vertices[vertexIndex.y - 1].y);
                verticesOut.push_back(vertices[vertexIndex.y - 1].z);

                verticesOut.push_back(vertices[vertexIndex.z - 1].x);
                verticesOut.push_back(vertices[vertexIndex.z - 1].y);
                verticesOut.push_back(vertices[vertexIndex.z - 1].z);

                texCoordsOut.push_back(texCoords[uvIndex.x - 1].x);
                texCoordsOut.push_back(texCoords[uvIndex.x - 1].y);


                texCoordsOut.push_back(texCoords[uvIndex.y - 1].x);
                texCoordsOut.push_back(texCoords[uvIndex.y - 1].y);


                texCoordsOut.push_back(texCoords[uvIndex.z - 1].x);
                texCoordsOut.push_back(texCoords[uvIndex.z - 1].y);



                normalsOut.push_back(normals[normalIndex.x - 1].x);
                normalsOut.push_back(normals[normalIndex.x - 1].y);
                normalsOut.push_back(normals[normalIndex.x - 1].z);

                normalsOut.push_back(normals[normalIndex.y - 1].x);
                normalsOut.push_back(normals[normalIndex.y - 1].y);
                normalsOut.push_back(normals[normalIndex.y - 1].z);

                normalsOut.push_back(normals[normalIndex.z - 1].x);
                normalsOut.push_back(normals[normalIndex.z - 1].y);
                normalsOut.push_back(normals[normalIndex.z - 1].z);

            }     
        }
        else if (token == "mtllib")
        {
            stream >> mltName;
            loadTexture();
        }
       
    } 
  
};
