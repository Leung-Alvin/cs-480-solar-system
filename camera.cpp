#include "camera.h"

Camera::Camera()
{

}

Camera::~Camera()
{

}

bool Camera::Initialize(int w, int h)
{
  // Simple: look directly at the origin
  view = glm::lookAt(glm::vec3(0.0, 10.0, -16.0),  // Camera position
                     glm::vec3(0.0, 0.0, 0.0),      // Look at origin
                     glm::vec3(0.0, 1.0, 0.0));     // Up vector

  projection = glm::perspective( glm::radians(40.f), 
                                 float(w)/float(h), 
                                 0.01f, 
                                 750.0f);
  return true;
}

glm::mat4 Camera::GetProjection()
{
  return projection;
}

glm::mat4 Camera::GetView()
{
  return view;
}

void Camera::Update(glm::mat4 transform) {
    view = transform * view;
}
