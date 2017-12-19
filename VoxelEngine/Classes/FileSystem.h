#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

// cpp
#include <string>

// boost
#include <boost\filesystem.hpp>

namespace fs = boost::filesystem;

namespace Voxel
{
	class FileSystem
	{
	private:
		FileSystem();

		// Delete copy, move, assign operators
		FileSystem(FileSystem const&) = delete;             // Copy construct
		FileSystem(FileSystem&&) = delete;                  // Move construct
		FileSystem& operator=(FileSystem const&) = delete;  // Copy assign
		FileSystem& operator=(FileSystem &&) = delete;      // Move assign

		// Game folder name. Todo: Replace project name to game name
		const std::string gameFolderName = "ProjectVoxel";

		// User directory. Windows: C:/User/UserName/Documents/GameName
		std::string userDirectory;
		std::string workingDirectory;

		// working file
		fs::ofstream ofs;
	public:
		~FileSystem() = default;

		static FileSystem& getInstance()
		{
			static FileSystem instance;
			return instance;
		}

		// For logger
		bool deleteFile(const std::string& path);

		// Get working directory
		std::string getWorkingDirectory() const;

		// Get user directory
		std::string getUserDirectory() const;

		// Get env path
		std::string	getEnv(const std::string& value);

		// Check if path exists. Path is case insensitive
		bool doesPathExists(const std::string& path) const;

		// Check if path is regular file
		bool isRegularFile(const std::string& path) const;

		// Check if path is directory.
		bool isDirectory(const std::string& path) const;

		// create empty file
		void createEmptyFile(const std::string& path);

		// open file
		bool openFile(const std::string& path);

		// write file
		void write(const std::string& data);

		// close file
		bool closeFile();
	};
}

#endif