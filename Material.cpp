#include "Material.h"

Material::Material(float matAmb[4], float matDiff[4], float matSpec[4], float matShine)
{
	for (int i = 0; i < 4; i++) {
		matAmbient[i] = matAmb[i];
		matDiffuse[i] = matDiff[i];
		matSpecular[i] = matSpec[i];
	}
	matShininess = matShine;
}

Material::~Material()
{

}
