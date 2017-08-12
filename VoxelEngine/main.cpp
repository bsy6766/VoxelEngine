#include <iostream>
#include <Application.h>

int main()
{
	std::cout << "Hello World!. Voxel Engine. 08.12.2017. Seung Youp Baek." << std::endl;

	Application* app = new Application();

	app->run();

	delete app;

	return 0;
}