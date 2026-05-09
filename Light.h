#ifndef LIGHT_H
#define LIGHT_H

#include "graphics_headers.h"

class Light
{
  public:
      Light(glm::vec4 gAmb, glm::vec4 lightAmb, glm::vec4 lightDiff,
          glm::vec4 lightSpec, glm::vec3 lightPos, glm::mat4 viewMatrix);
      void updateViewSpacePosition(glm::mat4 viewMatrix);
    ~Light();


    glm::vec4 m_globalAmbient;
    glm::vec4 m_lightAmbient;
    glm::vec4 m_lightDiffuse;
    glm::vec3 m_lightPosition;          
    glm::vec3 m_lightPositionViewSpace; 
    glm::vec4 m_lightSpecular;




  
  private:


};

#endif /* LIGHT_H */
