#include <InputHandler.h>

using namespace Voxel;

void InputHandler::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}
}

void InputHandler::glfwCursorPosCallback(GLFWwindow* window, double x, double y)
{
}

void InputHandler::glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
}