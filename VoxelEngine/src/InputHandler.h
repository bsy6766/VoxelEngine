#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <unordered_map>
#include <Application.h>
#include <GLFW\glfw3.h>
#include <unordered_map>
#include <ControllerManager.h>
#include <functional>

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
		std::unordered_map<int/*GLFW Mouse button*/, int> mouseButtonMap;
		std::unordered_map<int/*GLFW Mouse button*/, int> mouseButtonTickMap;

		// Mouse scroll
		int mouseScrollValue;

		// Keyboard map that saves input state
		std::unordered_map<int/*GLFW Key*/, int> keyMap;
		// Keyboard map that saves input state for only single frame
		std::unordered_map<int/*GLFW Key*/, int> keyTickMap;

		// glfw modifier bit. 
		int mods;

		// input buffer. For debug console
		bool bufferEnabled;
		std::string buffer;

		// Controller
		ControllerManager* controllerManager;

		void updateMousePosition(double x, double y);
		void updateMouseButton(int button, int action, int mods);
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
		static void glfwScrollCallback(GLFWwindow* window, double xOffset, double yOffset);

		// Controller Manager Callback
		static void onButtonPressed(ControllerID id, IO::XBOX_360::BUTTON button);
		static void onButtonReleased(ControllerID id, IO::XBOX_360::BUTTON button);
		static void onAxisMoved(ControllerID id, IO::XBOX_360::AXIS axis, const float value);
		static void onControllerConnected(ControllerID id);
		static void onControllerDisconnected(ControllerID id);

		// InputHandler functions. 
		// Get mouse position. Pass x, y as reference
		void getMousePosition(double& x, double& y);
		// Check if key is down. Set tick true to check if it's pressed on current tick(frame)
		bool getKeyDown(int key, const bool tick = false);
		bool getKeyUp(int key, const bool tick = false);
		bool getKeyRepeat(int key, const bool tick = false);
		int getMods();
		bool getMouseDown(int button, const bool tick = false);
		bool getMouseUp(int button, const bool tick = false);
		bool getMouseRepeat(int button, const bool tick = false);

		// mouse scroll. up = 1, down = -1
		int getMouseScrollValue();

		// TODO: Controller id can be change at connection and disctionection. save id to player and update
		// For now, assume it's 0
		// L_AXIS_X. Left = -1, Right = 1
		float getAxisValue(IO::XBOX_360::AXIS axis);
		bool hasController();

		void update();
		void postUpdate();

		void setCursorToCenter();

		void setBufferMode(const float enabled);
		std::string getBuffer();
		std::string glfwKeyToString(const int key, const int mod);
	};
}

#endif