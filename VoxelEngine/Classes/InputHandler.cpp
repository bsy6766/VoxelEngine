#include "InputHandler.h"

// cpp
#include <iostream>
#include <string>

// voxel
#include "Setting.h"
#include "InputField.h"

using namespace Voxel;

InputHandler::InputHandler()
	: controllerManager(ControllerManager::getInstance())
	, curMousePos(0.0f)
	, prevMousePos(0.0f)
	, mods(0)
	, mouseScrollValue(0)
	, inputField(nullptr)
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
		{ KEY_INPUT::TOGGLE_GAME_MENU, GLFW_KEY_ESCAPE },
		{ KEY_INPUT::GLOBAL_ESCAPE, GLFW_KEY_ESCAPE },
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
	prevMousePos = curMousePos;
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

	// Not buffer mode. update key states
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
	//std::cout << "update mouse pos (" << x << ", " << y << ")\n";
	//prevMousePos = curMousePos;
	curMousePos.x = static_cast<float>(x);
	curMousePos.y = static_cast<float>(y);
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
	if (inputField)
	{
		// feed input field
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			updateInputFieldText(key, mods);
		}
	}
	else
	{
		// update key state
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
}

glm::vec2 Voxel::InputHandler::getMousePosition() const
{
	return curMousePos;
}

glm::vec2 Voxel::InputHandler::getPreviousMousePosition() const
{
	return prevMousePos;
}

glm::vec2 Voxel::InputHandler::getMouseMovedDistance() const
{
	return curMousePos - prevMousePos;
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
	prevMousePos = curMousePos = glm::vec2(0.0f);
	Application::getInstance().getGLView()->setCursorPos(0, 0);
}

std::string Voxel::InputHandler::glfwKeyToString(const int key, const int mod) const
{
	if (key >= 65 && key <= 90)
	{
		// Alphabet
		if (mod & GLFW_MOD_SHIFT)
		{
			// upper case
			return std::string(1, static_cast<char>(key));
		}
		else
		{
			// lower case. lower cases are 32 higher than uppper cases
			return std::string(1, static_cast<char>(key + 32));
		}
	}
	else if (key == GLFW_KEY_SPACE)
	{
		// whitespace. 
		return " ";
	}
	else if (GLFW_KEY_0 <= key && key <= GLFW_KEY_9)
	{
		// numbers
		if (mod & GLFW_MOD_SHIFT)
		{
			// shift + number
			switch (key)
			{
			case GLFW_KEY_0:
				// 0. 48 -> 41
				return ")";
				break;
			case GLFW_KEY_1:
				// 1. 49 -> 33
				return "!";
				break;
			case GLFW_KEY_2:
				// 2. 50 -> 64
				return "@";
				break;
			case GLFW_KEY_3:
				// 3. 51 -> 35
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
	else if (key == GLFW_KEY_GRAVE_ACCENT)
	{
		if (mod & GLFW_MOD_SHIFT)
		{
			return "~";
		}
		else
		{
			return "`";
		}
	}
	else if (key == GLFW_KEY_PERIOD)
	{
		if (mod & GLFW_MOD_SHIFT)
		{
			return ">";
		}
		else
		{
			return ".";
		}
	}
	else if (key == GLFW_KEY_MINUS)
	{
		if (mod & GLFW_MOD_SHIFT)
		{
			return "_";
		}
		else
		{
			return "-";
		}
	}
	else if (key == GLFW_KEY_EQUAL)
	{
		if (mod & GLFW_MOD_SHIFT)
		{
			return "+";
		}
		else
		{
			return "=";
		}
	}
	else if (key == GLFW_KEY_SLASH)
	{
		if (mod & GLFW_MOD_SHIFT)
		{
			return "?";
		}
		else
		{
			return "/";
		}
	}
	else if (key == GLFW_KEY_COMMA)
	{
		if (mod & GLFW_MOD_SHIFT)
		{
			return "<";
		}
		else
		{
			return ",";
		}
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
	else if (key == GLFW_KEY_APOSTROPHE)
	{
		if (mod & GLFW_MOD_SHIFT)
		{
			return "\"";
		}
		else
		{
			return "'";
		}
	}
	else if (key == GLFW_KEY_LEFT_BRACKET)
	{
		if (mod & GLFW_MOD_SHIFT)
		{
			return "{";
		}
		else
		{
			return "[";
		}
	}
	else if (key == GLFW_KEY_RIGHT_BRACKET)
	{
		if (mod & GLFW_MOD_SHIFT)
		{
			return "}";
		}
		else
		{
			return "]";
		}
	}
	else if (key == GLFW_KEY_BACKSLASH)
	{
		if (mod & GLFW_MOD_SHIFT)
		{
			return "|";
		}
		else
		{
			return "\\";
		}
	}

	return "";
}

void Voxel::InputHandler::redirectKeyInputToText(Voxel::UI::InputField * inputField)
{
	this->inputField = inputField;
}

void Voxel::InputHandler::updateInputFieldText(const int key, const int mod)
{
	if (inputField)
	{
		if (key == GLFW_KEY_BACKSPACE)
		{
			inputField->removeLastCharacter();
		}
		else if (key == GLFW_KEY_ENTER)
		{
			inputField->finishEdit();
		}
		else if (key == GLFW_KEY_ESCAPE)
		{
			inputField->cancelEdit();
		}
		else
		{
			inputField->appendStr(glfwKeyToString(key, mod));
		}
	}
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
