#include "Asteroid.h"
Asteroid::Asteroid()
{
    setupVertices();
    setupBuffers();
    setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);
    m_texture = nullptr;
    hasTex = false;
}

Asteroid::Asteroid(const char* fname) {

    setupVertices();
    setupBuffers();
    setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);

    // load texture from file
    m_texture = new Texture(fname);
    if (m_texture && m_texture->getTextureID() != 0)
        hasTex = true;
    else
        hasTex = false;
}

Asteroid::Asteroid(const char* fname, const char* nname) {

    setupVertices();
    setupBuffers();
    setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);

    // load texture from file
    m_texture = new Texture(fname);
    if (m_texture && m_texture->getTextureID() != 0)
        hasTex = true;
    else
        hasTex = false;

	m_norm = new Norm(nname);
    if (m_norm && m_norm->getNormalID() != 0)
        hasNorm = true;
    else
		hasNorm = false;
}


void Asteroid::setupVertices() {
    numVertices = 14;
    numIndices = 24;
    for (int i = 0; i < numVertices; i++) { vertices.push_back(glm::vec3()); }
    for (int i = 0; i < numVertices; i++) { texCoords.push_back(glm::vec2()); }
    for (int i = 0; i < numVertices; i++) { normals.push_back(glm::vec3()); }
    for (int i = 0; i < numIndices; i++) { indices.push_back(0); }

	vertices[0] = glm::vec3(-.4, .9, .3);
    vertices[1] = glm::vec3(-0.5, 0.5, -0.5);
    vertices[2] = glm::vec3(-0.5, 0.5, 0.5);
    vertices[3] = glm::vec3(0.5, 0.5, 0.5);
    vertices[4] = glm::vec3(0.5, 0.5, -0.5);
    vertices[5] = glm::vec3(-.65, 0, .1);
    vertices[6] = glm::vec3(-0.5, -0.5, -0.5);
    vertices[7] = glm::vec3(-0.5, -0.5, 0.5);
    vertices[8] = glm::vec3(-.3, .45, .75);
    vertices[9] = glm::vec3(0.5, -0.5, 0.5);
    vertices[10] = glm::vec3(1, .25, -.1);
    vertices[11] = glm::vec3(0.5, -0.5, -0.5);
    vertices[12] = glm::vec3(-.6, -.2, .3);
    vertices[13] = glm::vec3(-.3, -.75, .3);

    // Create vertices from the unique vertex data
    // Each vertex gets position, normal, and texture coordinate
    for (int i = 0; i < 14; i++) {
        // Ensure texture coordinates are properly assigned
        glm::vec2 texCoord = texCoords[i];

        // Normalize the normal vector to ensure proper lighting
        glm::vec3 normalizedNormal = glm::normalize(normals[i]);

        Vertices.push_back(Vertex(vertices[i], normalizedNormal, texCoord));
    }
    Indices = {
      0, 1, 2,
      0, 2, 3,  
      0, 3, 4,  
      0, 4, 1, 
	  5, 1, 2,
	  5, 2, 7,
	  5, 6, 7,
	  5, 6, 1,
	  8, 2, 3,
	  8, 3, 9,
      8, 9, 7,
	  8, 7, 2,
	  10, 3, 4,
	  10, 4, 11,
      10, 11, 9,
	  10, 9, 3,
	  12, 4, 1,
      12, 4, 11,
      12, 11, 6,
      12, 6, 1,
      13, 6, 7,
      13, 7, 9,
      13, 9, 11,
      13, 11, 6
      };
}


void Asteroid::setupBuffers() {
    // For OpenGL 3
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
}

void Asteroid::setupModelMatrix(glm::vec3 pivot, float angle, float scale) {
    pivotLocation = pivot;
    model = glm::translate(glm::mat4(1.0f), pivotLocation);
    model *= glm::rotate(glm::mat4(1.f), angle, glm::vec3(0., 1., 0));
    model *= glm::scale(glm::vec3(scale, scale, scale));
}

void Asteroid::Update(glm::mat4 matModel) {

    model = matModel;

}

// accessors
std::vector<int> Asteroid::getIndices() { return indices; }
std::vector<glm::vec3> Asteroid::getVertices() { return vertices; }
std::vector<glm::vec2> Asteroid::getTexCoords() { return texCoords; }
std::vector<glm::vec3> Asteroid::getNormals() { return normals; }

void Asteroid::Update(const std::vector<glm::mat4>& modelMatrices) {
    instanceCount = static_cast<int>(modelMatrices.size());
    
    glBindVertexArray(vao);
    
    // Create instance buffer
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
    
    // Set up matrix attributes (mat4 takes 4 attribute slots)
    GLsizei vec4Size = sizeof(glm::vec4);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(12 + i);
        glVertexAttribPointer(12 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * vec4Size));
        glVertexAttribDivisor(12 + i, 1); // Tell OpenGL this is an instanced attribute
    }
    
    glBindVertexArray(0);
}

void Asteroid::Render(GLint positionAttribLoc, GLint colorAttribLoc, GLint, int instanceCount) {
    glBindVertexArray(vao);
    // Enable Vertext Attributes
    glEnableVertexAttribArray(positionAttribLoc);
    glEnableVertexAttribArray(colorAttribLoc);

    // Bind your VBO buffer(s) and then setup vertex attribute pointers
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glVertexAttribPointer(positionAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(colorAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));


    // Bind your index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

    // Render with instancing
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(Indices.size()), GL_UNSIGNED_INT, 0, instanceCount);

    // Disable Vertex Attributes
    glDisableVertexAttribArray(positionAttribLoc);
    glDisableVertexAttribArray(colorAttribLoc);
}

void Asteroid::Render(GLint posAttribLoc, GLint colAttribLoc, GLint tcAttribLoc, GLint hasTextureLoc, GLint normalAttribLoc, int instanceCount) {
    
    glBindVertexArray(vao);
    // Enable vertex attribute arrays for each vertex attrib
    glEnableVertexAttribArray(posAttribLoc);
    //glEnableVertexAttribArray(colAttribLoc);
    glEnableVertexAttribArray(tcAttribLoc);
	glEnableVertexAttribArray(normalAttribLoc);

    // Bind your VBO
    glBindBuffer(GL_ARRAY_BUFFER, VB);

    // Set vertex attribute pointers to load correct data
    glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    //glVertexAttribPointer(colAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(tcAttribLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
	glVertexAttribPointer(normalAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // Bind your Element Array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

    // Handle texture binding
 //   if (m_texture != NULL) {
 //       glUniform1i(hasTextureLoc, true);
 //       glActiveTexture(GL_TEXTURE0);
 //       glBindTexture(GL_TEXTURE_2D, m_texture->getTextureID());
 //   }
 //   else {
 //       glUniform1i(hasTextureLoc, false);
 //   }

 //   if (m_norm != NULL) {
 //       glUniform1i(normalAttribLoc, true);
 //       glActiveTexture(GL_TEXTURE1);
 //       glBindTexture(GL_TEXTURE_2D, m_norm->getNormalID());
 //   }
 //   else {
 //       glUniform1i(normalAttribLoc, false);
	//}

    // Render with instancing
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(Indices.size()), GL_UNSIGNED_INT, 0, instanceCount);

    // Disable vertex arrays
    glDisableVertexAttribArray(posAttribLoc);
    //glDisableVertexAttribArray(colAttribLoc);
    glDisableVertexAttribArray(tcAttribLoc);
    glDisableVertexAttribArray(normalAttribLoc);
}