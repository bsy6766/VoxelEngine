#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <unordered_map>
#include <GLFW\glfw3.h>
#include <unordered_map>

namespace Voxel
{
	/**
	*	@class InputHandler
	*	@brief Manages input received from GLFW. Used as singleton
	*/
	class InputHandler
	{
	protected:
		InputHandler();
		~InputHandler() = default;

		// Mouse
		double curX;
		double curY;
		std::unordered_map<int/*GLFW Mouse button*/, bool> mouseButtonMap;

		// Keyboard
		std::unordered_map<int/*GLFW Key*/, bool/*press or released*/> keyMap;
		//std::unordered_map<int/*GLFW modifier keys*/, bool> keyModsMap;

		void updateMousePosition(double x, double y);
		void updateMouse(int button, int action, int mods);
		void updateKeyboard(int key, int action, int mods);
	private:
		// Delete copy, move, assign operators
		InputHandler(InputHandler const&) = delete;             // Copy construct
		InputHandler(InputHandler&&) = delete;                  // Move construct
		InputHandler& operator=(InputHandler const&) = delete;  // Copy assign
		InputHandler& operator=(InputHandler &&) = delete;      // Move assign
	public:
		static InputHandler& getInstance()
		{
			static InputHandler instance;
			return instance;
		}

		// GLFW callback
		static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void glfwCursorPosCallback(GLFWwindow* window, double x, double y);
		static void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

		// InputHandler functions
		void getMousePosition(double& x, double& y);
		bool getKeyDown(int key);
		bool getKeyUp(int key);
		bool getMouseDown(int button);
		bool getMouseUp(int button);

		void setCursorToCenter();
	};
}

#endif