#include <InputHandler.h>
#include <iostream>
#include <string>
#include <Setting.h>

using namespace Voxel;

InputHandler::InputHandler()
	: curX(0)
	, curY(0)
	, controllerManager(ControllerManager::getInstance())
	, mods(0)
	, bufferEnabled(false)
	, mouseScrollValue(0)
{
	// Initialize default key map
	defaultKeyBindMap = 
	{ 
		{ KEY_INPUT::MOVE_FOWARD, GLFW_KEY_W },
		{ KEY_INPUT::MOVE_BACKWARD	, GLFW_KEY_S },
		{ KEY_INPUT::MOVE_LEFT, GLFW_KEY_A },
		{ KEY_INPUT::MOVE_RIGHT, GLFW_KEY_D },
		{ KEY_INPUT::JUMP, GLFW_KEY_SPACE },
		{ KEY_INPUT::SNEAK, GLFW_KEY_LEFT_SHIFT },
		{ KEY_INPUT::TOGGLE_WORLD_MAP, GLFW_KEY_M },
		// Debug.
		{ KEY_INPUT::MOVE_UP, GLFW_KEY_SPACE },
		{ KEY_INPUT::MOVE_DOWN, GLFW_KEY_LEFT_SHIFT },
	};
}

Voxel::InputHandler::~InputHandler()
{
	ControllerManager::deleteInstance();
}

void Voxel::InputHandler::update()
{
	controllerManager->update();
}

void Voxel::InputHandler::postUpdate()
{
	mouseButtonTickMap.clear();
	keyTickMap.clear();
	mouseScrollValue = 0;
}


void Voxel::InputHandler::initControllerManager()
{
	controllerManager->onButtonPressed = InputHandler::onButtonPressed;
	controllerManager->onButtonReleased = InputHandler::onButtonReleased;
	controllerManager->onAxisMoved = InputHandler::onAxisMoved;
	controllerManager->onControllerConnected = InputHandler::onControllerConnected;
	controllerManager->onControllerDisconnected = InputHandler::onControllerDisconnected;
}

void InputHandler::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//std::cout << "key = " << key << ", scancode: " << scancode << ", action: " << action << ", mods: " << mods << std::endl;

	auto& instance = InputHandler::getInstance();
	instance.updateKeyboard(key, action, mods);
	//std::cout << "char = " << InputHandler::getInstance().glfwKeyToString(key, mods) << std::endl;

	if (action == GLFW_PRESS)
	{
		if (instance.bufferEnabled)
		{
			instance.buffer += instance.glfwKeyToString(key, mods);
		}
	}
	else if (action == GLFW_REPEAT)
	{
		if (instance.bufferEnabled)
		{
			instance.buffer += instance.glfwKeyToString(key, mods);
		}
	}
}

void InputHandler::glfwCursorPosCallback(GLFWwindow* window, double x, double y)
{
	InputHandler::getInstance().updateMousePosition(x, y);
}

void InputHandler::glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	InputHandler::getInstance().updateMouseButton(button, action, mods);
}

void Voxel::InputHandler::glfwScrollCallback(GLFWwindow * window, double xOffset, double yOffset)
{
	// ignore xOffset
	// up = 1, down = -1
	InputHandler::getInstance().mouseScrollValue = static_cast<int>(yOffset);
}

void Voxel::InputHandler::onButtonPressed(ControllerID id, IO::XBOX_360::BUTTON button)
{
}

void Voxel::InputHandler::onButtonReleased(ControllerID id, IO::XBOX_360::BUTTON button)
{
}

void Voxel::InputHandler::onAxisMoved(ControllerID id, IO::XBOX_360::AXIS axis, const float value)
{
}

void Voxel::InputHandler::onControllerConnected(ControllerID id)
{
	std::cout << "[InputHandler] Controller #" << id << " is connected\n";
}

void Voxel::InputHandler::onControllerDisconnected(ControllerID id)
{
	std::cout << "[InputHandler] Controller #" << id << " is disconnected\n";
}

int Voxel::InputHandler::getKeyFromUserKeyBind(const KEY_INPUT keyInput)
{
	auto find = userKeyBindMap.find(keyInput);
	if (find == userKeyBindMap.end())
	{
		return -1;
	}
	else
	{
		return find->second;
	}
}

void InputHandler::updateMousePosition(double x, double y)
{
	curX = x;
	curY = y;
}

void Voxel::InputHandler::updateMouseButton(int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		mouseButtonMap[button] = GLFW_PRESS;
		mouseButtonTickMap[button] = GLFW_PRESS;
	}
	else if (action == GLFW_RELEASE)
	{
		mouseButtonMap[button] = GLFW_RELEASE;
		mouseButtonTickMap[button] = GLFW_RELEASE;
	}
	/*
	else if (action == GLFW_REPEAT)
	{
		mouseButtonMap[button] = GLFW_REPEAT;
		mouseButtonTickMap[button] = GLFW_REPEAT;
	}
	else
	{
		mouseButtonMap[button] = -1;
		mouseButtonTickMap[button] = -1;
	}
	*/
}

void Voxel::InputHandler::updateKeyboard(int key, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		keyMap[key] = GLFW_PRESS;
		keyTickMap[key] = GLFW_PRESS;
	}
	else if (action == GLFW_RELEASE)
	{
		keyMap[key] = GLFW_RELEASE;
		keyTickMap[key] = GLFW_RELEASE;
	}
	/*
	else if (action == GLFW_REPEAT)
	{
		keyMap[key] = GLFW_REPEAT;
		keyTickMap[key] = GLFW_REPEAT;
	}
	else
	{
		keyMap[key] = -1;
		keyTickMap[key] = -1;
	}
	*/
	this->mods = mods;
	//std::cout << "Key update. key = " << key << ", action = " << action << ", mods = " << mods << std::endl;
}

void Voxel::InputHandler::getMousePosition(double & x, double & y)
{
	x = curX;
	y = curY;
}

bool InputHandler::getKeyDown(int key, const bool tick)
{
	if (tick)
	{
		auto find_key = keyTickMap.find(key);
		if (find_key == keyTickMap.end())
		{
			return false;
		}
		else
		{
			return keyTickMap[key] == GLFW_PRESS;
		}
	}
	else
	{
		auto find_key = keyMap.find(key);
		if (find_key == keyMap.end())
		{
			return false;
		}
		else
		{
			return keyMap[key] == GLFW_PRESS;
		}
	}
}

bool InputHandler::getKeyUp(int key, const bool tick)
{
	if (tick)
	{
		auto find_key = keyTickMap.find(key);
		if (find_key == keyTickMap.end())
		{
			return false;
		}
		else
		{
			return keyTickMap[key] == GLFW_RELEASE;
		}
	}
	else
	{
		auto find_key = keyMap.find(key);
		if (find_key == keyMap.end())
		{
			return false;
		}
		else
		{
			return keyMap[key] == GLFW_RELEASE;
		}
	}
}

bool Voxel::InputHandler::getKeyDown(const KEY_INPUT keyInput, const bool tick)
{
	// Check if user has key bind for this
	int glfwKey = getKeyFromUserKeyBind(keyInput);
	if (glfwKey == -1)
	{
		// Has no bind. Use default key
		auto find = defaultKeyBindMap.find(keyInput);
		if (find == defaultKeyBindMap.end())
		{
			// Wasn't able to find key
			std::cout << "[InputHandler] Wasn't able to find default key for KEY_INPUT: " << (int)keyInput << "\n";
			return false;
		}
		else
		{
			// Found default key
			return getKeyDown(find->second, tick);
		}
	}
	else
	{
		// User bound the key for this KEY_INPUT
		return getKeyDown(glfwKey, tick);
	}
}

/*
bool InputHandler::getKeyRepeat(int key, const bool tick)
{
	if (tick)
	{
		auto find_key = keyTickMap.find(key);
		if (find_key == keyTickMap.end())
		{
			return false;
		}
		else
		{
			return keyTickMap[key] == GLFW_REPEAT;
		}
	}
	else
	{
		auto find_key = keyMap.find(key);
		if (find_key == keyMap.end())
		{
			return false;
		}
		else
		{
			return keyMap[key] == GLFW_REPEAT;
		}
	}
}
*/

int Voxel::InputHandler::getMods()
{
	return mods;
}

bool InputHandler::getMouseDown(int button, const bool tick)
{
	if (tick)
	{
		auto find_button = mouseButtonTickMap.find(button);
		if (find_button == mouseButtonTickMap.end())
		{
			return false;
		}
		else
		{
			return mouseButtonTickMap[button] == GLFW_PRESS;
		}
	}
	else
	{
		auto find_button = mouseButtonMap.find(button);
		if (find_button == mouseButtonMap.end())
		{
			return false;
		}
		else
		{
			return mouseButtonMap[button] == GLFW_PRESS;
		}
	}
}

bool InputHandler::getMouseUp(int button, const bool tick)
{
	if (tick)
	{
		auto find_button = mouseButtonTickMap.find(button);
		if (find_button == mouseButtonTickMap.end())
		{
			return false;
		}
		else
		{
			return mouseButtonTickMap[button] == GLFW_RELEASE;
		}
	}
	else
	{
		auto find_button = mouseButtonMap.find(button);
		if (find_button == mouseButtonMap.end())
		{
			return false;
		}
		else
		{
			return mouseButtonMap[button] == GLFW_RELEASE;
		}
	}
}

/*
bool InputHandler::getMouseRepeat(int button, const bool tick)
{
	if (tick)
	{
		auto find_button = mouseButtonTickMap.find(button);
		if (find_button == mouseButtonTickMap.end())
		{
			return false;
		}
		else
		{
			return mouseButtonTickMap[button] == GLFW_REPEAT;
		}
	}
	else
	{
		auto find_button = mouseButtonMap.find(button);
		if (find_button == mouseButtonMap.end())
		{
			return false;
		}
		else
		{
			return mouseButtonMap[button] == GLFW_REPEAT;
		}
	}
}
*/

int Voxel::InputHandler::getMouseScrollValue()
{
	return mouseScrollValue;
}

void Voxel::InputHandler::setCursorToCenter()
{
	curX = 0;
	curY = 0;
}

void Voxel::InputHandler::setBufferMode(const float enabled)
{
	bufferEnabled = enabled;
}

std::string Voxel::InputHandler::getBuffer()
{
	std::string copy = buffer;
	buffer.clear();
	return copy;
}

std::string Voxel::InputHandler::glfwKeyToString(const int key, const int mod)
{
	// Backspace
	if (key == GLFW_KEY_BACKSPACE)
	{
		return "VOXEL_GLFW_KEY_BACKSPACE";
	}
	else if (key == GLFW_KEY_ENTER)
	{
		return "VOXEL_GLFW_KEY_ENTER";
	}
	else if (key == GLFW_KEY_UP)
	{
		return "VOXEL_GLFW_KEY_UP";
	}
	else if (key >= 65 && key <= 90)
	{
		// GLFW key is same as ascii. However, it doesn't support shifted keys.
		// Alphabet
		if (mod & GLFW_MOD_SHIFT)
		{
			// upper case
			return std::string(1, static_cast<char>(key));
		}
		else
		{
			// lower case
			return std::string(1, static_cast<char>(key + 32));
		}
	}
	else if (key == GLFW_KEY_SPACE)
	{
		// whitespace
		return " ";
	}
	else if (key == GLFW_KEY_PERIOD)
	{
		return ".";
	}
	else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
	{
		if (mod & GLFW_MOD_SHIFT)
		{
			// shift + number
			switch (key)
			{
			case GLFW_KEY_0:
				// 0
				return ")";
				break;
			case GLFW_KEY_1:
				// 1
				return "!";
				break;
			case GLFW_KEY_2:
				// 2
				return "@";
				break;
			case GLFW_KEY_3:
				// 3
				return "#";
				break;
			case GLFW_KEY_4:
				// 4
				return "^";
				break;
			case GLFW_KEY_5:
				// 5
				return "%";
				break;
			case GLFW_KEY_6:
				// 6
				return "^";
				break;
			case GLFW_KEY_7:
				// 7
				return "&";
				break;
			case GLFW_KEY_8:
				// 8
				return "*";
				break;
			case GLFW_KEY_9:
				// 9
				return "(";
				break;
			default:
				return "";
				break;
			}
		}
		else
		{
			// number
			return std::string(1, static_cast<char>(key));
		}
	}
	else if (key == GLFW_KEY_MINUS)
	{
		return "-";
	}
	else if (key == GLFW_KEY_SEMICOLON)
	{
		if (mod & GLFW_MOD_SHIFT)
		{
			return ":";
		}
		else
		{
			return ";";
		}
	}

	return "";
}

float Voxel::InputHandler::getAxisValue(IO::XBOX_360::AXIS axis)
{
	return controllerManager->getAxisValue(0, axis);
}

bool Voxel::InputHandler::isControllerButtonDown(IO::XBOX_360::BUTTON button)
{
	return controllerManager->isButtonPressed(0, button);
}

bool Voxel::InputHandler::hasController()
{
	return controllerManager->hasController();
}
