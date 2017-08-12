#include <iostream>
#include <Application.h>

int main()
{
	std::cout << "Hello World!. Voxel Engine. 08.12.2017. Seung Youp Baek." << std::endl;

	Application* app = new Application();
	
	try
	{
		app->init();
		app->run();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	if (app)
	{
		delete app;
	}

	return 0;
}