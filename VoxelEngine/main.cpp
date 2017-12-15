#include <iostream>

// Voxel
#include <Application.h>
#include <FileSystem.h>
#include <Logger.h>

int main(int argc, const char * argv[])
{
	// Initialize some sub system before application
	auto& fs = Voxel::FileSystem::getInstance();
	
	auto& logger = Voxel::Logger::getInstance();

	logger.info("===========================================================================");

#if _DEBUG
	std::cout << "Hello world!\n";
	std::cout << "Author: Seung Youp Baek\n";
	std::cout << "Development start date: 08.12.2017\n";
	std::cout << "Version: Development\n";
	std::cout << "\n";
	std::cout << "\nargc: " << argc << std::endl;

	for (int i = 0; i < argc; i++)
	{
		std::cout << "argv[" << i << "]: " << std::string(argv[i]) << "\n";
	}

	std::cout << "\n";

	std::cout << "Working directory: " << fs.getWorkingDirectory() << "\n";
	std::cout << "User directory: " << fs.getUserDirectory() << "\n";
	std::cout << "\n";

	logger.info("===========================================================================");
#endif

	// Create application
	Voxel::Application& app = Voxel::Application::getInstance();
	
	try
	{
		// Initialize application
		app.init();
		// Run application
		app.run();
	}
	catch (std::exception& e)
	{
		// Exception thrown.
		std::cout << e.what() << std::endl;
	}

	logger.info("Ternimating...");
	logger.info("End of log.");
	logger.info("===========================================================================");

	return 0;
}