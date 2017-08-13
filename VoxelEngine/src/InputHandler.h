#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <unordered_map>
#include <GLFW\glfw3.h>

namespace Voxel
{
	class InputHandler
	{
	private:
	public:
		static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void glfwCursorPosCallback(GLFWwindow* window, double x, double y);
		static void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		
	};
}

#endif