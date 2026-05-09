

#include "engine.h"
#include "glm/ext.hpp"

double Engine::scrollOffsetY = 0.0;

Engine::Engine(const char* name, int width, int height)
{
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
  //glfwSetWindowUserPointer(m_window->getWindow(), this);
  //glfwSetCursorPosCallback(m_window->getWindow(), cursorPositionCallBack);

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
    float shipHullSize = 40.0f;
    //if (scrollOffsetY != 0.0) {
    //    float zoomSpeed = 10.0f;

    //    if (isThird) {
    //        tpZoom -= (float)scrollOffsetY * zoomSpeed;
    //        tpZoom = glm::clamp(tpZoom, 50.0f, 100.0f); // Keep TP zoom reasonable
    //    }
    //    else {
    //        fpZoom += (float)scrollOffsetY * zoomSpeed; // '+' moves it further out the nose
    //        fpZoom = glm::clamp(fpZoom, 0.0f, 500.0f);   // Keep FP zoom past the cockpit
    //    }
    //    scrollOffsetY = 0.0;
    //}

    //if (scrollOffsetY != 0.0) {
    //    float zoomSpeed = 20.0f; // Adjust for sensitivity

    //    if (isThird) {
    //        tpZoom -= (float)scrollOffsetY * zoomSpeed;
    //        tpZoom = glm::clamp(tpZoom, 10.0f, 500.0f);
    //    }
    //    else {
    //        fpZoom += (float)scrollOffsetY * zoomSpeed;
    //        fpZoom = glm::clamp(fpZoom, 10.0f, 500.0f);
    //    }
    //    scrollOffsetY = 0.0;
    //}


    //if (scrollOffsetY != 0.0) {
    //    float zoomSpeed = 20.0f;

    //    if (isThird) {
    //        tpZoom -= (float)scrollOffsetY * zoomSpeed;
    //        tpZoom = glm::clamp(tpZoom, 10.0f, 1000.0f);
    //    }
    //    else {
    //        // --- RELATIVE ZOOM LOGIC ---
    //        float minZoom = 2.0f; // Minimum distance from ship center
    //        float maxZoom = 500.0f; // Default for when not locked

    //        if (isSnapping && lockedTarget != NONE) {
    //            float targetRadius = m_graphics->getPlanetRadius(lockedTarget);
    //            maxZoom = targetRadius * 2.5f;
    //            zoomSpeed = targetRadius * 0.1f; 
    //        }

    //        fpZoom += (float)scrollOffsetY * zoomSpeed;
    //        fpZoom = glm::clamp(fpZoom, minZoom, maxZoom);
    //    }
    //    scrollOffsetY = 0.0;
    //}

    if (scrollOffsetY != 0.0) {
        // Instead of adding a flat 20.0f, change by 10% per scroll
        float multiplier = (scrollOffsetY > 0) ? 0.9f : 1.1f;

        if (isThird) {
            tpZoom *= multiplier;
        }
        else {
            // Invert for FP because + moves further away
            fpZoom += scrollOffsetY;
        }
        scrollOffsetY = 0.0;
    }


    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window->getWindow(), true);

    glm::mat4 shipModel = m_graphics->getMesh()->GetModel();

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

    shipModel = glm::translate(shipModel, glm::vec3(0.0f, 0.0f, moveSpeed));

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

    //m_graphics->getCamera()->Update(glm::rotate(glm::mat4(1.0f), (float)x * 0.001f, glm::vec3(0.0f, 1.0f, 0.0f)));
    //m_graphics->getCamera()->Update(glm::rotate(glm::mat4(1.0f), (float)y * 0.001f, glm::vec3(1.0f, 0.0f, 0.0f)));

    float targetRadius = 1.0f;


    if (isSnapping && lockedTarget != NONE) {
        glm::vec3 targetPos = m_graphics->getPlanetPosition(lockedTarget);
        glm::vec3 shipPos = glm::vec3(shipModel[3]);
        targetRadius = m_graphics->getPlanetRadius(lockedTarget);

        glm::vec3 diff = shipPos - targetPos;
        float currentDist = glm::length(diff);

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

    if (isThird) {
        shipModel = glm::rotate(shipModel, (float)-x * sensitivity, glm::vec3(0.0f, 1.0f, 0.0f));
        shipModel = glm::rotate(shipModel, (float)y * sensitivity, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    m_graphics->getMesh()->Update(shipModel);

    glm::vec3 finalCameraPos;
    glm::vec3 finalLookAt;
    glm::vec3 shipPos = glm::vec3(shipModel[3]);
    glm::vec3 shipForward = glm::vec3(shipModel[2]); 
    glm::vec3 shipUp = glm::vec3(shipModel[1]);

    //if (isThird) {
    //    float minTP = std::max(shipHullSize * 2.0f, targetRadius * 0.5f);
    //    float maxTP = std::max(minTP + 100.0f, targetRadius * 10.0f);
    //    tpZoom = glm::clamp(tpZoom, minTP, maxTP);

    //    float heightOffset = glm::clamp(targetRadius * 0.5f, shipHullSize * 1.5f, 60.0f);

    //    finalCameraPos = glm::vec3(shipModel * glm::vec4(0.0f, heightOffset, -tpZoom, 1.0f));
    //    finalLookAt = shipPos + (shipForward * (targetRadius * 5.0f));
    //}
    if (isThird) {
        const float SHIP_MIN_DISTANCE = 40.0f;

        float minTP = SHIP_MIN_DISTANCE;
        float maxTP = 1500.0f;
        tpZoom = glm::clamp(tpZoom, minTP, maxTP);


        float zoomPercent = (tpZoom - minTP) / (maxTP - minTP);
        float angleScale = glm::mix(0.5f, 0.15f, zoomPercent);
        float heightOffset = tpZoom * angleScale;


        finalCameraPos = glm::vec3(shipModel * glm::vec4(0.0f, heightOffset, -tpZoom, 1.0f));

        finalLookAt = shipPos + (shipForward * (tpZoom * 0.5f));
    }
    else {

        float cockpitHeight = shipHullSize * 0.5f;

        float minFP = shipHullSize;
        float maxFP = std::max(minFP + 10.0f, targetRadius * 2.0f);
        fpZoom = glm::clamp(fpZoom, minFP, maxFP);

        finalCameraPos = glm::vec3(shipModel * glm::vec4(0.0f, cockpitHeight, fpZoom, 1.0f));

        if (isSnapping && lockedTarget != NONE) {
            finalLookAt = m_graphics->getPlanetPosition(lockedTarget);
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
  //long long TimeNowMillis = GetCurrentTimeMillis();
  //assert(TimeNowMillis >= m_currentTimeMillis);
  //unsigned int DeltaTimeMillis = (unsigned int)(TimeNowMillis - m_currentTimeMillis);
  //m_currentTimeMillis = TimeNowMillis;
  //return DeltaTimeMillis;
    return glfwGetTime();
}

long long Engine::GetCurrentTimeMillis()
{
  //timeval t;
  //gettimeofday(&t, NULL);
  //long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
  //return ret;
    return (long long) glfwGetTime();
}

void Engine::Display(GLFWwindow* window, double time) {

    m_graphics->HierarchicalUpdate2(time);
    m_graphics->Render();
    m_window->Swap();
}

static void cursorPositionCallBack(GLFWwindow* window, double xpos, double ypos) {
    Engine* eng = (Engine*)glfwGetWindowUserPointer(window);

    if (eng->getFirstMouse()) {
		eng->setLastX(xpos);
        eng->setLastY(ypos);
        eng->setFirstMouse(false);
    }

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