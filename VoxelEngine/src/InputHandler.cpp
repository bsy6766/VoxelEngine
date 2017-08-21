#include <InputHandler.h>
#include <iostream>
#include <string>

using namespace Voxel;

InputHandler::InputHandler()
	: curX(0)
	, curY(0)
	, controllerManager(ControllerManager::getInstance())
{
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
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			Application::getInstance().getGLView()->close();
		}
	}

	//std::cout << "key = " << key << ", scancode: " << scancode << ", action: " << action << ", mods: " << mods << std::endl;

	InputHandler::getInstance().updateKeyboard(key, action, mods);
	//std::cout << "char = " << InputHandler::getInstance().glfwKeyToString(key, mods) << std::endl;
}

void InputHandler::glfwCursorPosCallback(GLFWwindow* window, double x, double y)
{
	InputHandler::getInstance().updateMousePosition(x, y);
}

void InputHandler::glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	InputHandler::getInstance().updateMouseButton(button, action, mods);
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
	std::cout << "[InputHandler] Controller #" << id << " is connected" << std::endl;
}

void Voxel::InputHandler::onControllerDisconnected(ControllerID id)
{
	std::cout << "[InputHandler] Controller #" << id << " is disconnected" << std::endl;
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

void Voxel::InputHandler::setCursorToCenter()
{
	curX = 0;
	curY = 0;
}

std::string Voxel::InputHandler::glfwKeyToString(const int key, const int mod)
{
	// GLFW key is same as ascii. However, it doesn't support shifted keys.
	// Alphabet
	if (key >= 65 && key <= 90)
	{
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
	else if (key == 32)
	{
		// whitespace
		return " ";
	}
	else if (key >= 48 && key <= 57)
	{
		if (mod & GLFW_MOD_SHIFT)
		{
			// shift + number
			switch (key)
			{
			case 49:
				// 1
				return "!";
				break;
			case 50:
				// 2
				return "@";
				break;
			case 51:
				// 3
				return "#";
				break;
			case 52:
				// 4
				return "^";
				break;
			case 53:
				// 5
				return "%";
				break;
			case 54:
				// 6
				return "^";
				break;
			case 55:
				// 7
				return "&";
				break;
			case 56:
				// 8
				return "*";
				break;
			case 57:
				// 9
				return "(";
				break;
			case 48:
				// 0
				return ")";
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

	return "";
}

float Voxel::InputHandler::getAxisValue(IO::XBOX_360::AXIS axis)
{
	return controllerManager->getAxisValue(0, axis);
}

bool Voxel::InputHandler::hasController()
{
	return controllerManager->hasController();
}
