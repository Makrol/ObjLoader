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
class Model{
private:

    std::vector< glm::vec3 > v;
    std::vector< glm::vec2 > vt;
    std::vector< glm::vec3 > vn;
    
    std::vector< glm::uvec3> vertexIndices;
    std::vector< glm::uvec3> uvIndices;
    std::vector< glm::uvec3> normalIndices;
    
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texCoords;

    std::string mltName="";
    GLuint textureID;

public:

	void loadFile(std::string fileName) {

        std::string myText;

        std::ifstream MyReadFile("model/"+ fileName);

        while (getline(MyReadFile, myText)) {
            
            newReadFromLine(myText);
        }
        MyReadFile.close();

        procesData();
        loadTexture();

	}
    void render() {
        glPushMatrix();

        glBindTexture(GL_TEXTURE_2D, textureID);

        glBegin(GL_TRIANGLES);
        for (int i = 0; i < vertices.size(); i+=3)
        {   
            glTexCoord2f(texCoords[i], texCoords[i+1]);
            glNormal3f(normals[i], normals[i+1], normals[i+2]);
            glVertex3f(vertices[i], vertices[i+1], vertices[i+2]);
            
        }
      
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glPopMatrix();

    }


private:
    void procesData() {
        for (int i = 0; i < vertexIndices.size(); i++)
        {
            /// vertices
            vertices.push_back(v[vertexIndices[i].x - 1].x);
            vertices.push_back(v[vertexIndices[i].x - 1].y);
            vertices.push_back(v[vertexIndices[i].x - 1].z);


            vertices.push_back(v[vertexIndices[i].y - 1].x);
            vertices.push_back(v[vertexIndices[i].y - 1].y);
            vertices.push_back(v[vertexIndices[i].y - 1].z);

            vertices.push_back(v[vertexIndices[i].z - 1].x);
            vertices.push_back(v[vertexIndices[i].z - 1].y);
            vertices.push_back(v[vertexIndices[i].z - 1].z);

            /// texCords
            texCoords.push_back(vt[uvIndices[i].x - 1].x);
            texCoords.push_back(vt[uvIndices[i].x - 1].y);
            texCoords.push_back(0);

            texCoords.push_back(vt[uvIndices[i].y - 1].x);
            texCoords.push_back(vt[uvIndices[i].y - 1].y);
            texCoords.push_back(0);

            texCoords.push_back(vt[uvIndices[i].z - 1].x);
            texCoords.push_back(vt[uvIndices[i].z - 1].y);
            texCoords.push_back(0);
            

            /// normals
            normals.push_back(vn[normalIndices[i].x - 1].x);
            normals.push_back(vn[normalIndices[i].x - 1].y);
            normals.push_back(vn[normalIndices[i].x - 1].z);

            normals.push_back(vn[normalIndices[i].y - 1].x);
            normals.push_back(vn[normalIndices[i].y - 1].y);
            normals.push_back(vn[normalIndices[i].y - 1].z);

            normals.push_back(vn[normalIndices[i].z - 1].x);
            normals.push_back(vn[normalIndices[i].z - 1].y);
            normals.push_back(vn[normalIndices[i].z - 1].z);
           
        }
    }
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
            v.push_back(vertex);
        }
        else if (token == "vn")
        {
            glm::vec3 normal;
            stream >> normal.x >> normal.y >> normal.z;
            vn.push_back(normal);
        }
        else if (token == "vt")
        {
            glm::vec2 uv;
            stream >> uv.x >> uv.y;
            vt.push_back(uv);
        }
        else if (token == "f")
        {
            std::string vertex1, vertex2, vertex3;
            glm::uvec3 vertexIndex, uvIndex, normalIndex;
            glm::uvec3 tmp;
            char slash;
            stream >>   vertexIndex.x >> slash >> uvIndex.x >> slash >> normalIndex.x >> 
                        vertexIndex.y >> slash >> uvIndex.y >> slash >> normalIndex.y >> 
                        vertexIndex.z >> slash >> uvIndex.z >> slash >> normalIndex.z;
            
            vertexIndices.push_back(glm::uvec3(vertexIndex.x, vertexIndex.y, vertexIndex.z));
            uvIndices.push_back(glm::uvec3(uvIndex.x, uvIndex.y, uvIndex.z));
            normalIndices.push_back(glm::uvec3(normalIndex.x, normalIndex.y, normalIndex.z));

        }
        else if (token == "mtllib")
        {
            stream >> mltName;
        }
    } 
};
