#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H


// cpp
#include <unordered_map>
#include <unordered_map>
#include <functional>

// voxel
#include "ControllerManager.h"
#include "Application.h"

// gl
#include <GLFW\glfw3.h>

namespace Voxel
{
	/**
	*	@class InputHandler
	*	@brief Manages input received from GLFW. Used as singleton
	*/
	class InputHandler
	{
	public:
		enum class KEY_INPUT
		{
			MOVE_FOWARD = 0,
			MOVE_BACKWARD,
			MOVE_LEFT,
			MOVE_RIGHT,
			JUMP,
			SNEAK,
			TOGGLE_WORLD_MAP,	

			// Debug
			MOVE_UP,
			MOVE_DOWN,
		};
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

		// Default key setting
		std::unordered_map<KEY_INPUT, int/*GLFW key*/> defaultKeyBindMap;
		// User key setting. Empty means default.
		// Note: may be use vector?
		std::unordered_map<KEY_INPUT, int/*user GLFW key*/> userKeyBindMap;

		// glfw modifier bit. 
		int mods;

		// input buffer. For debug console
		bool bufferEnabled;
		std::string buffer;

		// Controller
		ControllerManager* controllerManager;

		// Checks if player has custom key bind. Returns -1 if didn't.
		int getKeyFromUserKeyBind(const KEY_INPUT keyInput);

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
		glm::vec2 getMousePosition();

		/**
		*	Checks if key is down
		*	@param [in] key GLFW key code to check
		*	@param [in] tick If it's true, it checks if key was down on current frame. Else, generally checks if key is down
		*	@return True if key is down. Else, false.
		*/
		bool getKeyDown(int key, const bool tick = false);

		/**
		*	Checks if key is up
		*	@param [in] key GLFW key code to check
		*	@param [in] tick If it's true, it checks if key was up on current frame. Else, generally checks if key is up
		*	@return True if key is up. Else, false.
		*/
		bool getKeyUp(int key, const bool tick = false);

		/**
		*	Check is key for specific action is down
		*	@param [in] keyInput KEY_INPUT to check.
		*	@param [in] tick If it's true, it checks if key was down on current frame. Else, generally checks if key is down
		*	@return True if key is down. Else, false.
		*/
		bool getKeyDown(const KEY_INPUT keyInput, const bool tick = false);

		//bool getKeyRepeat(int key, const bool tick = false);

		/**
		*	Get currently pressed modifier keys.
		*	GLFW_MOD_SHIFT = 1
		*	GLFW_MOD_CONTROL = 2
		*	GLFW_MOD_ALT = 4
		*	@return Value of mod. 0 means nothing. 7 means all.
		*/
		int getMods();

		/**
		*	Checks if mouse is down
		*	@param [in] button GLFW mouse button to check
		*	@param [in] tick If it's true, it checks if mouse button was down on current frame. Else, generally checks if mouse button is down
		*	@return True if mouse button is down. Else, false.
		*/
		bool getMouseDown(int button, const bool tick = false);

		/**
		*	Checks if mouse is up
		*	@param [in] button GLFW mouse button to check
		*	@param [in] tick If it's true, it checks if mouse button was up on current frame. Else, generally checks if mouse button is up
		*	@return True if mouse button is up. Else, false.
		*/
		bool getMouseUp(int button, const bool tick = false);

		//bool getMouseRepeat(int button, const bool tick = false);

		/**
		*	Get mouse scroll value
		*	@return 1 if scrolled up. -1 if scrolled down. 0 if none.
		*/
		int getMouseScrollValue();

		// TODO: Controller id can be change at connection and disctionection. save id to player and update
		// For now, assume it's 0
		// L_AXIS_X. Left = -1, Right = 1
		float getAxisValue(IO::XBOX_360::AXIS axis);

		// Check if controller button is down
		bool isControllerButtonDown(IO::XBOX_360::BUTTON button);

		// check if controller is connected
		bool hasController();

		// For now, simple updates controller manager
		void update();

		/**
		*	Post updates input states.
		*	Wipes all inputs that were pressed current frame. 
		*/
		void postUpdate();

		void setCursorToCenter();

		void setBufferMode(const float enabled);
		std::string getBuffer();
		std::string glfwKeyToString(const int key, const int mod);
	};
}

#endif