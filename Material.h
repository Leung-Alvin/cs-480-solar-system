#ifndef MATERIAL_H
#define MATERIAL_H

#include "graphics_headers.h"

class Material
{
  public:
	Material(glm::vec4 matAmb, glm::vec4 matDiff, glm::vec4 matSpec, glm::vec1 matShine);
    ~Material();


	glm::vec4 matAmbient;
	glm::vec4 matDiffuse;
	glm::vec4 matSpecular;
	glm::vec1 matShininess;


  
  private:


};

#endif /* MATERIAL_H */
