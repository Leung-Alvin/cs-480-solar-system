#ifndef ENGINE_H
#define ENGINE_H


#include <assert.h>
#include "window.h"
#include "graphics.h"

static void cursorPositionCallBack(GLFWwindow*, double xpos, double ypos);
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
static void rKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

class Engine
{
  public:
    Engine(const char*  name, int width, int height);

    Graphics* m_graphics;
    ~Engine();
    bool Initialize();
    void Run();
    void ProcessInput();
    unsigned int getDT();
    long long GetCurrentTimeMillis();
    void Display(GLFWwindow*, double);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

	bool getFirstMouse() { return firstMouse; }
	float getXOffset() { return xOffset; }
	float getYOffset() { return yOffset; }
	float getLastX() { return lastX; }
	float getLastY() { return lastY; }

	void setFirstMouse(bool value) { firstMouse = value; }
	void setXOffset(float value) { xOffset = value; }
	void setYOffset(float value) { yOffset = value; }
	void setLastX(float value) { lastX = value; }
	void setLastY(float value) { lastY = value; }

    bool isSnapping;
    bool isThird;

    PlanetID lockedTarget = NONE;


    static double scrollOffsetY;
    float tpZoom = 250.0f; // Initial Third Person distance
    float fpZoom = 150.0f; // Initial First Person (Nose) distance
  
  private:
    // Window related variables
    Window *m_window;    
    const char* m_WINDOW_NAME;
    int m_WINDOW_WIDTH;
    int m_WINDOW_HEIGHT;
    bool m_FULLSCREEN;



    bool m_running;

	bool firstClick;
	double prevMouseX;
	double prevMouseY;

	double scrollX;

    bool firstMouse;
    float xOffset;
    float yOffset;
    float lastX;
    float lastY;


	float acceleration = 0.0f;
	float moveSpeed = 0.0f;
};

#endif // ENGINE_H
