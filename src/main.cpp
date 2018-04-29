#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS

#include <iostream>
#include <vector>
#include <string>
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "gtx/transform.hpp"
#include "render.hpp"
#include "model.hpp"
#include "globals.hpp"
#include "game.hpp"

#include <gtc/type_ptr.hpp>


// TODO: fix texture loading (se en gul pacman!!!!!1!)



// camera class:
// camera should have a shared pointer for view and projection 
// models should observe these two variables using weak pointers

void error_callback(int error, const char* description)
{
	std::cout << "Error " << error << ": " << description;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {

	if (!glfwInit())
	{
		// Initialization failed
		return -4;
	}


	GLFWwindow* window = glfwCreateWindow(glob::WINDOW_X, glob::WINDOW_Y, glob::WINDOW_NAME.c_str(), nullptr, nullptr); // glfwGetPrimaryMonitor()
	if (!window)
	{
		// Window or OpenGL context creation failed
		return -3;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	// bind glfw events to custom function
	glfwSetErrorCallback(error_callback);
	glfwSetKeyCallback(window, key_callback);

	glewExperimental = GL_FALSE;
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "OpenGL Error: " << error << std::endl;
	}

	if (glewInit() != GLEW_OK) {
		return -2;
		glfwTerminate();
	}
	


	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	glfwSwapInterval(0);
	
	Game game;
	game.initPacman("assets/levels/level0");
	game.gameState = glob::PLAYING;

	glFrontFace(GL_CW);
	glDepthRange(0, 1);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);

	double lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		if (game.gameState == glob::DEAD) {
			game.initPacman("assets/levels/level0");
			game.gameState = glob::PLAYING;
		}


		glob::ButtonPressed pressedButtons;
		glfwSwapBuffers(window);

		// Measure speed
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		if (game.gameState != glob::PLAYING) {
			deltaTime = 0;
		}

		glfwPollEvents();

		// todo: move to game?
		int enterPressed = glfwGetKey(window, GLFW_KEY_ENTER); // fixes bug with glfw key buffer
	
		pressedButtons.wKey		= GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W);
		pressedButtons.dKey		= GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D);
		pressedButtons.sKey		= GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S);
		pressedButtons.aKey		= GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A);
		pressedButtons.spaceKey = GLFW_PRESS == glfwGetKey(window, GLFW_KEY_SPACE);
		pressedButtons.lCtrlKey = GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
		pressedButtons.up		= GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP);
		pressedButtons.down		= GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN);
		pressedButtons.left		= GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT);
		pressedButtons.right	= GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT);
		
		//renderer2.draw();
		game.update(deltaTime, pressedButtons);
		game.draw();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;

}

