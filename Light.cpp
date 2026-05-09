#include "Light.h"

Light::Light(glm::vec4 gAmb, glm::vec4 lightAmb, glm::vec4 lightDiff,
    glm::vec4 lightSpec, glm::vec3 lightPos, glm::mat4 viewMatrix)
{
    // 1. Establish Light Position (View Space transformation)
    // Note: We multiply by the View Matrix to move the light from World -> Camera space
    glm::vec4 vPos = viewMatrix * glm::vec4(lightPos, 1.0f);
    m_lightPosition = lightPos;
    m_lightPositionViewSpace[0] = vPos.x;
    m_lightPositionViewSpace[1] = vPos.y;
    m_lightPositionViewSpace[2] = vPos.z;

    // 2. Establish Colors (Ambient)
    m_lightAmbient[0] = lightAmb.r;
    m_lightAmbient[1] = lightAmb.g;
    m_lightAmbient[2] = lightAmb.b;
    m_lightAmbient[3] = lightAmb.a;

    // 3. Establish Colors (Diffuse)
    m_lightDiffuse[0] = lightDiff.r;
    m_lightDiffuse[1] = lightDiff.g;
    m_lightDiffuse[2] = lightDiff.b;
    m_lightDiffuse[3] = lightDiff.a;

    // 4. Establish Colors (Specular)
    m_lightSpecular[0] = lightSpec.r;
    m_lightSpecular[1] = lightSpec.g;
    m_lightSpecular[2] = lightSpec.b;
    m_lightSpecular[3] = lightSpec.a;

    // 5. Establish Global Ambient
    m_globalAmbient[0] = gAmb.r;
    m_globalAmbient[1] = gAmb.g;
    m_globalAmbient[2] = gAmb.b;
    m_globalAmbient[3] = gAmb.a;
}

Light::~Light()
{

}

void Light::updateViewSpacePosition(glm::mat4 viewMatrix)
{
    glm::vec4 vPos = viewMatrix * glm::vec4(m_lightPosition, 1.0f);
    m_lightPositionViewSpace[0] = vPos.x;
    m_lightPositionViewSpace[1] = vPos.y;
    m_lightPositionViewSpace[2] = vPos.z;
}