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
#define WIDTH 600
#define HEIGHT 600

class Model{
public:
    std::vector< GLint> vertexIndices;
    std::vector< GLint> uvIndices;
    std::vector< GLint> normalIndices;

    std::vector<GLfloat*> vertices;
    std::vector<GLfloat*> normals;
    std::vector<GLfloat*> texCoords;
    
    std::vector<GLfloat> verticesVec;

    std::string mltName="";
    GLuint textureID;
    
    std::vector<Face> faces;
    GLuint list;

public:

	void loadFile(std::string fileName, GLuint shaderProgram,float cameraDistance) {

        std::string myText;

        std::ifstream MyReadFile("model/"+ fileName);

        while (getline(MyReadFile, myText)) {
            
            newReadFromLine(myText);
        }
        MyReadFile.close();
        std::cout << "Vertices:" << vertices.size()<<std::endl;
        std::cout << "Texcords:" << texCoords.size() << std::endl;
        std::cout << "Normals:" << normals.size() << std::endl;
        std::cout << "Faces:" << faces.size() << std::endl;
        list = glGenLists(1);
        /*glNewList(list, GL_COMPILE);
       

        for (Face& face : faces)
        {
            if (face.normal != -1)
            {
                glNormal3fv(normals[face.normal]);
            }
            else
            {
                glDisable(GL_LIGHTING);
            }
            if (textureID != 0)
            {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, textureID);
                glBegin(GL_POLYGON);
                for (int i = 0; i < face.edge; i++)
                {
                    if (face.texCords[i] != -1)
                    {
                        glTexCoord2fv(texCoords[face.texCords[i]]);
                    }
                    verticesVec.push_back(*vertices[face.vertices[i]]);
                    glVertex3fv(vertices[face.vertices[i]]);
                }
                glEnd();
                           
            }
            else{
                glBegin(GL_POLYGON);
                for (int i = 0; i < face.edge; i++)
                {

                    glVertex3fv(vertices[face.vertices[i]]);
                }
                glEnd();
            }
            if (face.normal == -1)
                glEnable(GL_LIGHTING);
        }
        //glUseProgram(0);
        glEndList();*/

       

       /* for (GLfloat* f : vertices)
            delete f;
        vertices.clear();

        for (GLfloat* f : texCoords)
            delete f;
        texCoords.clear();

        for (GLfloat* f : normals)
            delete f;
        normals.clear();*/

        

	}

    void draw() {
        //glCallList(list);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        for (Face& face : faces)
        {
            if (face.normal != -1)
            {
                glNormal3fv(normals[face.normal]);
            }
            else
            {
                glDisable(GL_LIGHTING);
            }
            if (textureID != 0)
            {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, textureID);
                glBegin(GL_POLYGON);
                for (int i = 0; i < face.edge; i++)
                {
                    if (face.texCords[i] != -1)
                    {
                        glTexCoord2fv(texCoords[face.texCords[i]]);
                    }
                    verticesVec.push_back(*vertices[face.vertices[i]]);
                    glVertex3fv(vertices[face.vertices[i]]);
                }
                glEnd();

            }
            else {
                glBegin(GL_POLYGON);
                for (int i = 0; i < face.edge; i++)
                {

                    glVertex3fv(vertices[face.vertices[i]]);
                }
                glEnd();
            }
            if (face.normal == -1)
                glEnable(GL_LIGHTING);
        }
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
            vertices.push_back(new GLfloat[3]{ vertex.x,vertex.y,vertex.z });
        }
        else if (token == "vn")
        {
            glm::vec3 normal;
            stream >> normal.x >> normal.y >> normal.z;
            normals.push_back(new GLfloat[3]{ normal.x,normal.y,normal.z });
        }
        else if (token == "vt")
        {
            glm::vec2 uv;
            stream >> uv.x >> uv.y;
            texCoords.push_back(new GLfloat[2]{ uv.x,uv.y });
        }
        else if (token == "f")
        {
            std::string vertex1, vertex2, vertex3;
            glm::uvec4 vertexIndex, uvIndex, normalIndex;
            glm::uvec3 tmp;
            char slash;
            if (!(stream >> vertexIndex.x >> slash >> uvIndex.x >> slash >> normalIndex.x >>
                vertexIndex.y >> slash >> uvIndex.y >> slash >> normalIndex.y >>
                vertexIndex.z >> slash >> uvIndex.z >> slash >> normalIndex.z >>
                vertexIndex.w >> slash >> uvIndex.w >> slash >> normalIndex.w))
            {
                int* v = new int[3] { (int)vertexIndex.x - 1, (int)vertexIndex.y - 1, (int)vertexIndex.z - 1};
                int* t = new int[3] { (int)uvIndex.x - 1, (int)uvIndex.y - 1, (int)uvIndex.z - 1};
                faces.push_back(Face(3, v, t, normalIndex.x - 1));
            }
            else {
                int* v = new int[4] { (int)vertexIndex.x - 1, (int)vertexIndex.y - 1, (int)vertexIndex.z - 1, (int)vertexIndex.w - 1};
                int* t = new int[4] { (int)uvIndex.x - 1, (int)uvIndex.y - 1, (int)uvIndex.z - 1, (int)uvIndex.w - 1};
                faces.push_back(Face(4, v, t, normalIndex.x - 1));
            } 
        }
        else if (token == "mtllib")
        {
            stream >> mltName;
            loadTexture();
        }
       
    } 
  
};
