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
  view = glm::lookAt(glm::vec3(0.0, 12.0, -18.0),  // Camera position
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

void Camera::Update(float xOff, float yOff, bool left, bool right, bool up, bool down) {
    yaw += xOff * 0.1f;
    pitch += yOff* 0.1f;


    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(dir);

    glm::vec3 rightVec = glm::normalize(glm::cross(cameraFront, glm::vec3(0, 1, 0)));

    float speed = 0.15f;

    if (left)  cameraPos -= rightVec * speed;
    if (right) cameraPos += rightVec * speed;

    if (up)    cameraPos += cameraFront * speed;
    if (down)  cameraPos -= cameraFront * speed;

    view = glm::lookAt(cameraPos, cameraPos + cameraFront, glm::vec3(0, 1, 0));
}