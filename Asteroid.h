#pragma once
#include "object.h"
#include "Texture.h"
#include <vector>
#include "graphics_headers.h"
class Asteroid : public Object
{
public:
    Asteroid();


    void Render(GLint positionAttribLoc, GLint colorAttribLoc, int instanceCount);
    void Render(GLint positionAttribLoc, GLint colorAttribLoc, GLint tcAttribLoc, GLint hasTex, GLint normalAttribLoc, int instanceCount);

    void Render(GLint positionAttribLoc, GLint colorAttribLoc);
    void Render(GLint positionAttribLoc, GLint colorAttribLoc, GLint tcAttribLoc, GLint hasTex);

    void Update(const std::vector<glm::mat4>& modelMatrices);

    glm::mat4 GetModel() { return model; }
    void Update(glm::mat4 matModel);

    Asteroid(const char* fname);

    std::vector<int> getIndices();
    std::vector<glm::vec3> getVertices();
    std::vector<glm::vec2> getTexCoords();
    std::vector<glm::vec3> getNormals();

    GLuint getTextureID() { return m_texture->getTextureID(); }

    bool hasTex;

private:
    glm::vec3 pivotLocation;
    glm::mat4 model;
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    GLuint VB;
    GLuint IB;
    Texture* m_texture;

    GLuint vao;
    
    GLuint instanceVBO;
    int instanceCount;

    float angle;

    void setupVertices();
    void setupBuffers();
    void setupModelMatrix(glm::vec3 pivotLoc, float angle, float scale);

    int numVertices;
    int numIndices;
    std::vector<int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
};

