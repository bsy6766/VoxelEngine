#include <iostream>
#include <Application.h>

int main(int argc, const char * argv[])
{
	std::cout << "Hello World!. Voxel Engine. 08.12.2017. Seung Youp Baek.\n";
	std::cout << "\nargc: " << argc << std::endl;
	std::cout << "argv: " << std::string(argv[0]) << "\n\n";

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