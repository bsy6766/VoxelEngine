#include <iostream>
#include <Application.h>

int main(int argc, const char * argv[])
{
	std::cout << "Hello world! Initializing application...\n";
	std::cout << "Author: Seung Youp Baek\n";
	std::cout << "Development start date: 08.12.2017\n";
	std::cout << "\nargc: " << argc << std::endl;
	for (int i = 0; i < argc; i++)
	{
		std::cout << "argv[" << i << "]: " << std::string(argv[i]) << "\n";
	}
	std::cout << "\n";

	Voxel::Application& app = Voxel::Application::getInstance();
	
	try
	{
		app.init();
		app.run();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}