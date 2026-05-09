#ifndef MESH_H
#define MESH_H

#include <vector>
#include "graphics_headers.h"
#include "Texture.h"
#include "Norm.h"

class Mesh
{
public:
    Mesh();
    Mesh(glm::vec3 pivot, const char* fname);
    Mesh(glm::vec3 pivot, const char* fname, const char* tname);
    Mesh(glm::vec3 pivot, const char* fname, const char* tname, const char* nname);

    ~Mesh();
    void Update(glm::mat4 model);
    void Render(GLint posAttrib, GLint colAttrib);
    void Render(GLint positionAttribLoc, GLint colorAttribLoc, GLint tcAttribLoc, GLint hasTex, GLint normalAttribLoc);

    glm::mat4 GetModel();

    bool InitBuffers();
    bool loadModelFromFile(const char* path);

    bool hasTex;
    bool hasNorm;
    GLuint getTextureID() { return m_texture->getTextureID(); }
	GLuint getNormalID() { return m_norm->getNormalID(); }



private:
    glm::vec3 pivotLocation;
    glm::mat4 model;
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    GLuint VB;
    GLuint IB;

    Texture* m_texture;
	Norm* m_norm;

    GLuint vao;

    float angle;
};

#endif