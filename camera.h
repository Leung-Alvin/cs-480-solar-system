#ifndef CAMERA_H
#define CAMERA_H

#include "graphics_headers.h"

class Camera
{
  public:
    Camera();
    ~Camera();
    bool Initialize(int w, int h);
    glm::mat4 GetProjection();
    glm::mat4 GetView();
	void Update(glm::mat4 transform);
    void Update(float xOff, float yOff, bool left, bool right, bool up, bool down);
	void setView(glm::mat4 newView) { view = newView; }

  
  private:

    double x = 0.0;
    double y = 10.0;
    double z = -16.0;
    glm::mat4 projection;
    glm::mat4 view;

	glm::vec3 cameraPos = glm::vec3(x, y, z);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float yaw = -90.0f;
    float pitch = 0.0f;
    float fov = 22.5f;

};

#endif /* CAMERA_H */
