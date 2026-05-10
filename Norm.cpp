#include "Norm.h"

Norm::Norm(const char* fileName) {
	// Load the texture data from the file and initialize the texture

	loadTexture(fileName);
	initializeTexture();
}

Norm::Norm() {
	m_TextureID = -1;
	printf("No NORM Texture Data Provided.");
}

bool Norm::loadTexture(const char* texFile) {
	// Load the texture data from the file using SOIL
	m_TextureID = SOIL_load_OGL_texture(texFile, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (!m_TextureID) {
		printf("Failed: Could not open NORM texture file!\n");
		return false;
	}
	return true;
}

bool Norm::initializeTexture() {
	// Bind the texture and set texture parameters

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);

	
	return true;
}



