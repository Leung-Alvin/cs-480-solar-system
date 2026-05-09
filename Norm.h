#pragma once
#include "graphics_headers.h"
#include <SOIL2/SOIL2.h>

class Norm
{

public:
	Norm();
	Norm(const char* fileName);
	bool loadTexture(const char* texFile);
	GLuint getNormalID() { return m_TextureID; }

private:
	GLuint m_TextureID;

	bool initializeTexture();

};

