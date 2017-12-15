#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

// cpp (c++17)
#include <filesystem>
#include <string>

namespace Voxel
{
	/**
	*	@class FileSystem
	*	@brief A singleton class that provides file system
	*/
	class FileSystem
	{
	private:
		// Constructor
		FileSystem();

		// Destructor
		~FileSystem();

		// Delete copy, move, assign operators
		FileSystem(FileSystem const&) = delete;             // Copy construct
		FileSystem(FileSystem&&) = delete;                  // Move construct
		FileSystem& operator=(FileSystem const&) = delete;  // Copy assign
		FileSystem& operator=(FileSystem &&) = delete;      // Move assign
	public:
		// Get singleton instance
		static FileSystem& getInstance()
		{
			static FileSystem instance;
			return instance;
		}

		const std::string getCurrentPath() const;

	};
}

#endif