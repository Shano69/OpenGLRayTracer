#include "GLFWApplication.h"
#include <sstream>


int GLFWApplication::SCREEN_WIDTH = 0;
int GLFWApplication::SCREEN_HEIGHT = 0;

GLfloat GLFWApplication::lastX = 300 / 2.0;
GLfloat GLFWApplication::lastY = 300 / 2.0;
GLfloat GLFWApplication::xChange = 0.0f;
GLfloat GLFWApplication::yChange = 0.0f;

bool GLFWApplication::mouseFirstMoved = true;
bool GLFWApplication::mouseMoved = false;
bool GLFWApplication::keys[1024];


GLFWApplication::GLFWApplication(const char *windowTitle, int width, int height) {
	m_windowTitle = windowTitle; // set window title
	// set application window dimensions
	m_width = width;
	m_height = height;

	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	m_window = glfwCreateWindow(width, height, windowTitle, nullptr, nullptr);

	
}

bool GLFWApplication::shouldRun() {
	return !glfwWindowShouldClose(getWindow());
}


void GLFWApplication::createCallbacks()
{
	glfwSetKeyCallback(m_window, keyCallback);
}

// call back functions
// Is called whenever a key is pressed / released via GLFW
 void GLFWApplication::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	//std::cout << "keyboard event" << std::endl;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{

		if (action == GLFW_PRESS)
		{
			keys[key] = true;
			mouseMoved = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
			mouseMoved = true;
		}
	}
}

// called whenever a mouse event is triggered
 void  GLFWApplication::mouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	 if (GLFWApplication::mouseFirstMoved)
	 {
		 GLFWApplication::lastX = xPos;
		 GLFWApplication::lastY = yPos;
		 GLFWApplication::mouseFirstMoved = false;
	 }

	 double xOffset = xPos - GLFWApplication::lastX;
	 double yOffset = GLFWApplication::lastY - yPos;

	 xChange = xOffset;
	 yChange = yOffset;

	 GLFWApplication::lastX = xPos;
	 GLFWApplication::lastY = yPos;

	 if (xChange > 0 || yChange > 0 || xChange < 0 || yChange < 0)
	 {
		 mouseMoved = true;
	 }
	 
}


void  GLFWApplication::scrollCallback(GLFWwindow *window, double xOffset, double yOffset)
{
	std::cout << "scroll event" << std::endl;

}




// Renderer initialisation
void GLFWApplication::init() {

	if (nullptr == m_window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return;
	}

	// make the created window the curent window
	glfwMakeContextCurrent(m_window);
	glfwGetFramebufferSize(m_window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	createCallbacks();
	glfwSetCursorPosCallback(m_window, mouseCallback);
	//glfwSetScrollCallback(m_window, scrollCallback);

	// remove the mouse cursor
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// modern GLEW approach
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return;
}

void GLFWApplication::pollEvents() {
	
	glfwPollEvents();
}

void GLFWApplication::showFPS() {
	static double previousSeconds = 0.0;
	static int frameCount;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime();

	elapsedSeconds = currentSeconds - previousSeconds;

	// limit FPS refresh rate to 4 times per second
	if (elapsedSeconds > 0.25) {
		previousSeconds = currentSeconds;
		double FPS = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / FPS;

		std::ostringstream outs;
		outs.precision(3);
		outs << std::fixed
			<< m_windowTitle << "  "
			<< "FPS: " << FPS << "  "
			<< "Frame time: " << msPerFrame << "ms" << std::endl;
		glfwSetWindowTitle(m_window, outs.str().c_str());

		frameCount = 0;
	}
	frameCount++;
	
}



// clear buffer
void GLFWApplication::clear() {
	// Clear the colorbuffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLFWApplication::display() {
	glBindVertexArray(0);
	// Swap the buffers
	glfwSwapBuffers(m_window);
}

void GLFWApplication::terminate() 
{
     	glfwTerminate();
}
 