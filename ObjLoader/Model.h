#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <glm/glm.hpp>
#include <GL/freeglut.h>
#include <cmath>
class Model{
private:

   
    std::vector< glm::vec3 > v;
    std::vector< glm::vec2 > vt;
    std::vector< glm::vec3 > vn;
    std::vector< glm::vec3> f;

public:
    std::vector< glm::uvec3> vertexIndices;
    std::vector< glm::uvec3> uvIndices;
    std::vector< glm::uvec3> normalIndices;

    std::vector<glm::vec3> vertex;
    std::vector<float> uv;
    std::vector<unsigned int> normal;
	Model() {

	}

	void loadFile(std::string path) {

        std::string myText;

        std::ifstream MyReadFile("ex2.obj");

        while (getline(MyReadFile, myText)) {
            
            newReadFromLine(myText);
        }
        MyReadFile.close();

        for (int i = 0; i < f.size(); i++)
        {
            vertex.push_back(glm::vec3(v[f[i].x-1].x, v[f[i].x - 1].y, v[f[i].x - 1].z));
           /*

            uv.push_back(vt[f[i].y - 1].x);
            uv.push_back(vt[f[i].y - 1].y);

            normal.push_back(v[f[i].z - 1].x);
            normal.push_back(v[f[i].z - 1].y);
            normal.push_back(v[f[i].z - 1].z);*/
        }

	}
    void render() {
        glPushMatrix();
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < vertex.size(); i++)
        {
            glNormal3f(vn[f[i].z - 1].x, vn[f[i].z - 1].y, vn[f[i].z - 1].z);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3f(vertex[i].x, vertex[i].y, vertex[i].z);
        }
        

        glEnd();
        glPopMatrix();
    }


private:
   /* void readFromLine(std::string line)
    {
        std::istringstream stream(line);
        std::string token;
        float x, y, z;
        stream >> token;
        if (stream.fail()) {
            std::cerr << "B³¹d odczytu ." << std::endl;
        }
        else if (token == "v") {
            stream >> x >> y >> z;
            vert.push_back(x);
            vert.push_back(y);
            vert.push_back(z);
        }
        else if (token == "vn")
        {
            stream >> x >> y >> z;
            norm.push_back(x);
            norm.push_back(y);
            norm.push_back(z);
        }
        else if (token == "vt")
        {
            stream >> x >> y;
            text.push_back(x);
            text.push_back(y);
        }
        else if (token == "f")
        {
            unsigned int vIndex, tIndex, nIndex;
            char slash;

            for (int i = 0; i < 4; i++) {
                stream >> vIndex >> slash >> tIndex >> slash >> nIndex;
                if (!stream.fail()) {
                    indices.push_back(vIndex);
                    indices.push_back(tIndex);
                    indices.push_back(nIndex);
                }
            }
        }
        else {
            std::cerr << "Uszkodzony plik." << std::endl;
        }

    }*/
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
            
            vertexIndices.push_back(vertexIndex);
            uvIndices.push_back(uvIndex);
            normalIndices.push_back(normalIndex);
            f.push_back(glm::uvec3(vertexIndex.x, uvIndex.x, normalIndex.x));
            f.push_back(glm::uvec3(vertexIndex.y, uvIndex.y, normalIndex.y));
            f.push_back(glm::uvec3(vertexIndex.z, uvIndex.z, normalIndex.z));

        }
        else {
            std::cerr << "Uszkodzony plik." << std::endl;
        }

    }
    
    
};
