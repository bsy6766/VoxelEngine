#include <InputHandler.h>
//#include <iostream>

using namespace Voxel;

InputHandler::InputHandler()
	: curX(0)
	, curY(0)
{}

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