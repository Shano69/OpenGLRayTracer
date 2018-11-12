#pragma once

#include "Application.h"
#include "Raytracer/Raytracer.h"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>



class GLFWApplication : public Application {

	static int SCREEN_WIDTH, SCREEN_HEIGHT;

public:
	// constructors
	GLFWApplication() {}
	GLFWApplication(const char *windowTitle, int width, int height);
	~GLFWApplication() {}

	// render methods
	void init() override; //initialises render
	void clear() override; // clear buffer
	void display() override; // display buffer on window
	void terminate() override; // closes application

	// event management
	void pollEvents();

	// other methods
	bool shouldRun();
	void showFPS();
	GLFWwindow* getWindow() { return m_window; }

	//get keys
	bool* getKeys() { return keys; }


	inline GLfloat getXChange() 
	{
		GLfloat theChange = xChange;
		xChange = 0.0f;
		return theChange;
	}
	inline GLfloat getYChange() 
	{
		GLfloat theChange = yChange;
		yChange = 0.0f;
		return theChange;
	}

	bool getMoved() { return mouseMoved; }
	void setMoved(bool set) { mouseMoved = set; }

private:
	//camera controls variables

	static bool keys[1024];
	static GLfloat lastX;
	static GLfloat lastY;
	static GLfloat xChange;
	static GLfloat yChange;
	static bool mouseFirstMoved;
	static bool mouseMoved;

	void createCallbacks();
	static void  GLFWApplication::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
	static void GLFWApplication::mouseCallback(GLFWwindow *window, double xPos, double yPos);
	static void GLFWApplication::scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

	

	GLFWwindow* m_window = NULL;
};