#include "Material.h"

Material::Material(glm::vec4 matAmb, glm::vec4 matDiff, glm::vec4 matSpec, glm::vec1 matShine)
{
	matAmbient = matAmb;
	matDiffuse = matDiff;
	matSpecular = matSpec;
	matShininess = matShine;
}

Material::~Material()
{

}
