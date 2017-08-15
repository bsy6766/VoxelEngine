#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <unordered_map>
#include <GLFW\glfw3.h>
#include <unordered_map>
#include <ControllerManager.h>

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
		~InputHandler();

		// Mouse
		double curX;
		double curY;
		std::unordered_map<int/*GLFW Mouse button*/, bool> mouseButtonMap;

		// Keyboard
		std::unordered_map<int/*GLFW Key*/, bool/*press or released*/> keyMap;
		//std::unordered_map<int/*GLFW modifier keys*/, bool> keyModsMap;

		// Controller
		ControllerManager* controllerManager;

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

		// init
		void initControllerManager();

		// GLFW callback
		static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void glfwCursorPosCallback(GLFWwindow* window, double x, double y);
		static void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

		// Controller Manager Callback
		static void onButtonPressed(ControllerID id, IO::XBOX_360::BUTTON button);
		static void onButtonReleased(ControllerID id, IO::XBOX_360::BUTTON button);
		static void onAxisMoved(ControllerID id, IO::XBOX_360::AXIS axis, const float value);
		static void onControllerConnected(ControllerID id);
		static void onControllerDisconnected(ControllerID id);
		
		// InputHandler functions
		void getMousePosition(double& x, double& y);
		bool getKeyDown(int key);
		bool getKeyUp(int key);
		bool getMouseDown(int button);
		bool getMouseUp(int button);

		// TODO: Controller id can be change at connection and disctionection. save id to player and update
		// For now, assume it's 0
		// L_AXIS_X. Left = -1, Right = 1
		float getAxisValue(IO::XBOX_360::AXIS axis);
		bool hasController();

		void update();

		void setCursorToCenter();
	};
}

#endif