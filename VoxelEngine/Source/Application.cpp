#include "Application.h"
#include <iostream>

Application::Application()
{
	std::cout << "Creating Application" << std::endl;
}

Application::~Application()
{
	std::cout << "Destroying Application" << std::endl;
}

void Application::run()
{
}

