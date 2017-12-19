// pch
#include "PreCompiled.h"

#include "Logger.h"

// voxel
#include "FileSystem.h"

Voxel::Logger::Logger()
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
	: console(spdlog::stdout_color_mt("console"))
#endif
{
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
	console->set_pattern("[%P] [%l] [%c] %v");
#endif

	auto fs = &Voxel::FileSystem::getInstance();

	auto logFilePath = Voxel::FileSystem::getInstance().getWorkingDirectory() + "/log.txt";

	if (fs->doesPathExists(logFilePath))
	{
		fs->deleteFile(logFilePath);
	}
	
	logger = spdlog::basic_logger_mt("Log", logFilePath);

	logger->set_pattern("[%P] [%l] [%c] %v");
}

void Voxel::Logger::info(const std::string & log)
{
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
	console->info(log);
#endif

	logger->info(log);
}

void Voxel::Logger::warn(const std::string & log)
{
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
	console->warn(log);
#endif

	logger->warn(log);
}

void Voxel::Logger::error(const std::string & log)
{
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
	console->error(log);
#endif

	logger->error(log);
}

void Voxel::Logger::flush()
{
	logger->flush();
}

#if V_DEBUG && V_DEBUG_LOG_CONSOLE
void Voxel::Logger::consoleInfo(const std::string & log)
{
	console->info(log);
}

void Voxel::Logger::consoleWarn(const std::string & log)
{
	console->warn(log);
}

void Voxel::Logger::consoleError(const std::string & log)
{
	console->error(log);
}
#endif
