#ifndef MATERIAL_H
#define MATERIAL_H

#include "graphics_headers.h"

class Material
{
  public:
	Material(float matAmb[4], float matDiff[4], float matSpec[4], float matShine);
    ~Material();


	float matAmbient[4];
	float matDiffuse[4];
	float matSpecular[4];
	float matShininess;


  
  private:


};

#endif /* MATERIAL_H */
