#pragma once
class Face
{
public:
    int edge;
    int* vertices;
    int* texCords;
    int normal;
    Face(int edge, int* vertices, int* texCords, unsigned int normal = -1)
    {
        this->edge = edge;
        this->vertices = vertices;
        this->texCords = texCords;
        this->normal = normal;
    }
};

