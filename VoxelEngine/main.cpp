#include <iostream>
#include <Application.h>

int main(int argc, const char * argv[])
{
	std::cout << "Hello World!. Voxel Engine. 08.12.2017. Seung Youp Baek." << std::endl;
	std::cout << "\nargc: " << argc << std::endl;
	std::cout << "argv: " << std::string(argv[0]) << "\n" << std::endl;

	Voxel::Application* app = new Voxel::Application();
	
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