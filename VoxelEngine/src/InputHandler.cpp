#include <InputHandler.h>
#include <iostream>

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

	InputHandler::getInstance().updateKeyboard(key, action, mods);
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
		mouseButtonMap[button] = true;
		mouseButtonTickMap[button] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		mouseButtonMap[button] = false;
		mouseButtonTickMap[button] = false;
	}
	// Else, action is GLFW_REPEAT, which we don't need
}

void Voxel::InputHandler::updateKeyboard(int key, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		keyMap[key] = true;
		keyTickMap[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		keyMap[key] = false;
		keyTickMap[key] = false;
	}
	// Else, action is GLFW_REPEAT, which we don't need
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
		return keyTickMap[key] == true;
	}
	else
	{
		return keyMap[key] == true;
	}
}

bool InputHandler::getKeyUp(int key, const bool tick)
{
	if (tick)
	{
		return keyTickMap[key] == false;
	}
	else
	{
		return keyMap[key] == false;
	}
}

bool InputHandler::getMouseDown(int button, const bool tick)
{
	return mouseButtonMap[button] == true;
}

bool InputHandler::getMouseUp(int button, const bool tick)
{
	return mouseButtonMap[button] == false;
}

void Voxel::InputHandler::setCursorToCenter()
{
	curX = 0;
	curY = 0;
}

float Voxel::InputHandler::getAxisValue(IO::XBOX_360::AXIS axis)
{
	return controllerManager->getAxisValue(0, axis);
}

bool Voxel::InputHandler::hasController()
{
	return controllerManager->hasController();
}
