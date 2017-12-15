#ifndef LOGGER_H
#define LOGGER_H
// cpp
#include <memory>
#include <string>

// voxel
#include "Config.h"

// foward delcaration
namespace spdlog
{
	class logger;
}

namespace Voxel
{

	class Logger
	{
	private:
		// Constructor
		Logger();

		// Destructor
		~Logger() = default;

		// basic logger
		std::shared_ptr<spdlog::logger> logger;

#if V_DEBUG && V_DEBUG_LOG_CONSOLE
		// output console only when debug. Console will not appera on release build
		std::shared_ptr<spdlog::logger> console;
#endif

	public:
		static Logger& getInstance()
		{
			static Logger instance;
			return instance;
		}

		Logger(Logger const&) = delete;             // Copy construct
		Logger(Logger&&) = delete;                  // Move construct
		Logger& operator=(Logger const&) = delete;  // Copy assign
		Logger& operator=(Logger &&) = delete;      // Move assign
		
		// log info
		void info(const std::string& log);

		// log warn
		void warn(const std::string& log);

		// log error
		void error(const std::string& log);

		// flush
		void flush();

#if V_DEBUG && V_DEBUG_LOG_CONSOLE
		void consoleInfo(const std::string& log);
		void consoleWarn(const std::string& log);
		void consoleError(const std::string& log);
#endif
	};
}

#endif