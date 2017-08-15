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
			glfwSetWindowShouldClose(window, GL_TRUE);
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
	InputHandler::getInstance().updateMouse(button, action, mods);
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

void Voxel::InputHandler::updateMouse(int button, int action, int mods)
{
	mouseButtonMap[button] = action;
	// Do we really need to update mods here?
}

void Voxel::InputHandler::updateKeyboard(int key, int action, int mods)
{
	keyMap[key] = action;

	//std::cout << "Key update. key = " << key << ", action = " << action << ", mods = " << mods << std::endl;
}

void Voxel::InputHandler::getMousePosition(double & x, double & y)
{
	x = curX;
	y = curY;
}

bool InputHandler::getKeyDown(int key)
{
	return keyMap[key] == true;
}

bool InputHandler::getKeyUp(int key)
{
	return keyMap[key] == false;
}

bool InputHandler::getMouseDown(int button)
{
	return mouseButtonMap[button] == true;
}

bool InputHandler::getMouseUp(int button)
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
