#include <iostream>

// win32
#include <Windows.h>

// Voxel
#include <Application.h>
#include <FileSystem.h>
#include <Logger.h>


int main(int argc, const char * argv[])
{
	// Initialize some sub system before application
	auto& fs = Voxel::FileSystem::getInstance();
	
	auto& logger = Voxel::Logger::getInstance();
	
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
#endif

	// incase of error
	std::string errorMsg;

	{
		// App scope

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
			errorMsg = e.what();

			// minimize window to show message box
			app.getGLView()->setWindowMinimized();
		}

		// end of app
	}

	if (!errorMsg.empty())
	{
		// log error msg
		auto pos = errorMsg.find_first_of('\n');
		logger.error("Error! " + std::string(errorMsg.substr(0, pos)));

		// Todo: Change message to better message. Ask user to send log to developer...

		std::string message = "The game crashed.                                  \n";

		if (pos == std::string::npos)
		{
			message += ("Error code: " + errorMsg);
		}
		else
		{
			message += ("Error code: " + errorMsg.substr(0, pos));

			logger.error("Additional error info: " + errorMsg.substr(pos + 1));
		}

		MessageBox(NULL, message.c_str(), "Project Voxel crash report", MB_OK | MB_SYSTEMMODAL);
	}

	logger.info("Ternimating...");
	logger.info("End of log.");

	return 0;
}