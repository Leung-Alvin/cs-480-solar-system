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
	// Apply the transformation to the view matrix
    view = transform * view;
}

void Camera::Update(float xOff, float yOff, bool left, bool right, bool up, bool down) {
	// Update yaw and pitch based on mouse movement

    yaw += xOff * 0.1f;
    pitch += yOff* 0.1f;

	// Constrain pitch to prevent flipping

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

	// Calculate the new front vector based on updated yaw and pitch
    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(dir);

	// Calculate the right vector for strafing

    glm::vec3 rightVec = glm::normalize(glm::cross(cameraFront, glm::vec3(0, 1, 0)));


	// Move the camera based on keyboard input
    float speed = 0.15f;

    if (left)  cameraPos -= rightVec * speed;
    if (right) cameraPos += rightVec * speed;

    if (up)    cameraPos += cameraFront * speed;
    if (down)  cameraPos -= cameraFront * speed;

	// Update the view matrix based on the new camera position and orientation
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, glm::vec3(0, 1, 0));
}