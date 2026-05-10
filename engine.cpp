

#include "engine.h"
#include "glm/ext.hpp"

double Engine::scrollOffsetY = 0.0;

Engine::Engine(const char* name, int width, int height)
{
	// Initialize our window and graphics objects here
  m_WINDOW_NAME = name;
  m_WINDOW_WIDTH = width;
  m_WINDOW_HEIGHT = height;
  prevMouseX = 0; 
  prevMouseY = 0;
  xOffset = 0;
  yOffset = 0;
  firstMouse = true;
  isSnapping = false;
  isThird = true;
  fpZoom = 10.0f;
  tpZoom = 300.0f;

}


Engine::~Engine()
{
  delete m_window;
  delete m_graphics;
  m_window = NULL;
  m_graphics = NULL;
}

bool Engine::Initialize()
{
  // Start a window
  m_window = new Window(m_WINDOW_NAME, &m_WINDOW_WIDTH, &m_WINDOW_HEIGHT);
  if(!m_window->Initialize())
  {
    printf("The window failed to initialize.\n");
    return false;
  }

  // Start the graphics
  m_graphics = new Graphics();
  if(!m_graphics->Initialize(m_WINDOW_WIDTH, m_WINDOW_HEIGHT))
  {
    printf("The graphics failed to initialize.\n");
    return false;
  }


  // Set up input callbacks and modes

  glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwSetWindowUserPointer(m_window->getWindow(), this);

  glfwSetCursorPosCallback(m_window->getWindow(), cursorPositionCallBack);

  glfwSetScrollCallback(m_window->getWindow(), scrollCallback);
  glfwSetKeyCallback(m_window->getWindow(), rKeyCallback);


  // No errors
  return true;
}

void Engine::Run()
{
  m_running = true;

  while (!glfwWindowShouldClose(m_window->getWindow()))
  {
      ProcessInput();
      Display(m_window->getWindow(), glfwGetTime());
      glfwPollEvents();
  }
  m_running = false;

}

void Engine::ProcessInput()
{
    // Provide clearance to prevent camera clipping 
    float shipHullSize = 20.0f;


	// Calculate zoom changes based on scroll input
    if (scrollOffsetY != 0.0) {
        float multiplier = (scrollOffsetY > 0) ? 0.9f : 1.1f;

        if (isThird) {
            tpZoom *= multiplier;
        }
        else {
            fpZoom *= multiplier;
        }
        scrollOffsetY = 0.0;
    }


    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window->getWindow(), true);

    glm::mat4 shipModel = m_graphics->getMesh()->GetModel();

	// Define control parameters for movement and rotation
	float sensitivity = 0.002;
    float rollSpeed = 0.02f;
    float pitchSpeed = 0.03f;
    float yawSpeed = 0.03f;

    float accelRate = 10.0f;
    float brakeRate = 10.0f;  
    float maxSpeed = 300.0f;    
    float minSpeed = 0.0f;
    float drag = 0.98f;    
    float stopThreshold = 0.001f; 

	// Update accleration based on mouse input
    if (glfwGetMouseButton(m_window->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        moveSpeed += accelRate;
    }
    else if (glfwGetMouseButton(m_window->getWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        moveSpeed -= brakeRate;
    }
    else {
        moveSpeed *= drag;
        if (std::abs(moveSpeed) < stopThreshold) {
            moveSpeed = 0.0f;
        }
    }

    // Speed ratio is fed into shaders to determine engine glow intensity
	m_graphics->m_shipSpeedRatio = std::min(std::abs(moveSpeed) / maxSpeed, 1.0f);

	// Move ship forward/backward based on current speed
    shipModel = glm::translate(shipModel, glm::vec3(0.0f, 0.0f, moveSpeed));

	// Rotate ship based on keyboard input
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
        shipModel = glm::rotate(shipModel, pitchSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
        shipModel = glm::rotate(shipModel, -pitchSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
        shipModel = glm::rotate(shipModel, yawSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
        shipModel = glm::rotate(shipModel, -yawSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_E) == GLFW_PRESS) {
        shipModel = glm::rotate(shipModel, rollSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_Q) == GLFW_PRESS) {
        shipModel = glm::rotate(shipModel, -rollSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    

	// Calculate mouse movement offsets
    double mouseX, mouseY;
    glfwGetCursorPos(m_window->getWindow(), &mouseX, &mouseY);
    static bool firstFrame = true;
    if (firstFrame) {
        prevMouseX = mouseX;
        prevMouseY = mouseY;
        firstFrame = false;
    }
    double x = mouseX - prevMouseX;
    double y = mouseY - prevMouseY;

    prevMouseX = mouseX;
    prevMouseY = mouseY;

    float targetRadius = 1.0f;

	// If snapping is enabled and a target is locked, smoothly move the ship towards the target position
    if (isSnapping && lockedTarget != NONE) {
        glm::vec3 targetPos = m_graphics->getPlanetPosition(lockedTarget);
        glm::vec3 shipPos = glm::vec3(shipModel[3]);
        targetRadius = m_graphics->getPlanetRadius(lockedTarget);

        glm::vec3 diff = shipPos - targetPos;
        float currentDist = glm::length(diff);
        

		// Set approach distance based on target type and size to prevent overshooting or getting too close
        if (currentDist > 0.1f) {
            float approachDist;

            if (lockedTarget == SUN){
                approachDist = 10.0f;
			}
            else if (targetRadius < 0.5f) {
                approachDist = 0.2f;
			}
            else if (targetRadius < 1.0f) {
                approachDist = 1.0f;
            }
            glm::vec3 direction = glm::normalize(diff);
            glm::vec3 finalDestination = targetPos + (direction * approachDist);

            float alpha = 0.05f;
            glm::vec3 newPos = glm::mix(shipPos, finalDestination, alpha);
            shipModel[3] = glm::vec4(newPos, 1.0f);
        }
    }
	// If in third-person mode, rotate the ship based on mouse movement to allow for free look around the ship
    if (isThird) {
        shipModel = glm::rotate(shipModel, (float)-x * sensitivity, glm::vec3(0.0f, 1.0f, 0.0f));
        shipModel = glm::rotate(shipModel, (float)y * sensitivity, glm::vec3(1.0f, 0.0f, 0.0f));
    }

	// Update the ship's model matrix in the graphics system
    m_graphics->getMesh()->Update(shipModel);

	// Calculate camera position and orientation based on current mode (first-person or third-person) and ship's position and orientation
    glm::vec3 finalCameraPos;
    glm::vec3 finalLookAt;
    glm::vec3 shipPos = glm::vec3(shipModel[3]);
    glm::vec3 shipForward = glm::vec3(shipModel[2]); 
    glm::vec3 shipUp = glm::vec3(shipModel[1]);

	// In third-person mode, position the camera behind the ship at a distance determined by tpZoom, and look slightly ahead of the ship to provide better visibility during movement
    if (isThird) {
        const float SHIP_MIN_DISTANCE = 40.0f;

        float minTP = SHIP_MIN_DISTANCE;
        float maxTP = 1500.0f;
        tpZoom = glm::clamp(tpZoom, minTP, maxTP);

        //cout << "minTP: " << minTP << ", maxTP: " << maxTP << ", tpZoom: " << tpZoom << endl;


        float zoomPercent = (tpZoom - minTP) / (maxTP - minTP);
        float angleScale = glm::mix(0.5f, 0.15f, zoomPercent);
        float heightOffset = tpZoom * angleScale;


        finalCameraPos = glm::vec3(shipModel * glm::vec4(0.0f, heightOffset, -tpZoom, 1.0f));

        finalLookAt = shipPos + (shipForward * (tpZoom * 0.5f));
    }
	// In first-person mode, position the camera at the ship's cockpit and allow zooming in and out with limits to prevent clipping or excessive distance. If snapping to a target, smoothly interpolate the camera position towards a point in front of the target to provide a dynamic view of the target while maintaining the ship's orientation.
    else {

        float cockpitHeight = shipHullSize * 0.5f;

        float minFP = 0.00001f;
        //float maxFP = std::max(minFP + 10.0f, targetRadius * 2.0f);
		float maxFP = 100.0f;
        fpZoom = glm::clamp(fpZoom, minFP, maxFP);

        finalCameraPos = glm::vec3(shipModel * glm::vec4(0.0f, cockpitHeight, fpZoom, 1.0f));

		// If snapping to a target, smoothly interpolate the camera position towards a point in front of the target to provide a dynamic view of the target while maintaining the ship's orientation

        if (isSnapping && lockedTarget != NONE) {
            glm::vec3 targetPos = m_graphics->getPlanetPosition(lockedTarget);
			glm::vec3 cameraPos = shipPos + (shipUp * cockpitHeight) + shipForward;
			glm::vec3 viewDir = glm::normalize(cameraPos - targetPos);
			
			glm::vec3 zoomedPos = targetPos + (viewDir * fpZoom);

            finalLookAt = targetPos;
			finalCameraPos = glm::mix(finalCameraPos, zoomedPos, 0.1f);
        }
        else {
            finalLookAt = finalCameraPos + (shipForward * 1000.0f);
        }
    }

    glm::mat4 newView = glm::lookAt(finalCameraPos, finalLookAt, shipUp);
    m_graphics->getCamera()->setView(newView);

    xOffset = 0.0f;
    yOffset = 0.0f;
}

void Engine::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
 
    glfwGetCursorPos(window, &xpos, &ypos);
    std::cout << "Position: (" << xpos << ":" << ypos << ")";
}

unsigned int Engine::getDT()
{

    return glfwGetTime();
}

long long Engine::GetCurrentTimeMillis()
{
    return (long long) glfwGetTime();
}

void Engine::Display(GLFWwindow* window, double time) {

    m_graphics->HierarchicalUpdate2(time);
    m_graphics->Render();
    m_window->Swap();
}

static void cursorPositionCallBack(GLFWwindow* window, double xpos, double ypos) {

	// Retrieve the Engine instance from the window's user pointer

    Engine* eng = (Engine*)glfwGetWindowUserPointer(window);

	// If this is the first time the mouse callback is called, initialize the lastX and lastY values to the current mouse position to prevent large jumps in camera orientation
    if (eng->getFirstMouse()) {
		eng->setLastX(xpos);
        eng->setLastY(ypos);
        eng->setFirstMouse(false);
    }
	// Calculate the offset of the mouse movement since the last callback and update the Engine's xOffset and yOffset values, which will be used to rotate the camera in the ProcessInput function.
    eng->setXOffset(xpos - eng->getLastX());
    eng->setYOffset(eng->getLastY() - ypos);

    eng->setLastX(xpos);
    eng->setLastY(ypos);
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Engine::scrollOffsetY = yoffset;
}


void rKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
	// Toggle snapping mode when the R key is pressed. If snapping is enabled, lock onto the nearest planet and switch to third-person view. If snapping is disabled, unlock the target and switch back to first-person view.
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        engine->isSnapping = !engine->isSnapping;

        if (engine->isSnapping) {
            engine->lockedTarget = engine->m_graphics->findClosestPlanetID();
            engine->isThird = false;
            engine->m_graphics->m_showShip = false;
        }
        else {
            engine->lockedTarget = NONE;
            engine->isThird = true;
            engine->m_graphics->m_showShip = true; 
        }
    }
}