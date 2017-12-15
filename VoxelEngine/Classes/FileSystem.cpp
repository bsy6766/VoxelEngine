#include "FileSystem.h"


Voxel::FileSystem::FileSystem()
	: workingDirectory(fs::current_path().string())
{
	// windows = USERNAME, linux = USER
	std::string userName = getEnv("USERNAME");

	if (!userName.empty())
	{
		// save on documents
		auto documentsPathStr = "C:\\Users\\" + userName + "\\Documents";

		if (fs::is_directory(fs::path(documentsPathStr)))
		{
			auto gameFolderPathStr = documentsPathStr + "\\" + gameFolderName;
			if (!fs::exists(fs::path(gameFolderPathStr)))
			{
				// Game folder doesn't exsits. Create one
				fs::create_directory(gameFolderPathStr);
			}

			// Check if game folder exists
			if (fs::exists(fs::path(gameFolderPathStr)))
			{
				// set user directory
				userDirectory = gameFolderPathStr;

				return;
			}
		}
		// Else, failed to check Documents folder. Use working directory
	}
	// Else, userName is empty.

	// At this point, filesystem failed to access Documents folder (windows). Use working directory.
	userDirectory = workingDirectory;
}

bool Voxel::FileSystem::deleteFile(const std::string & path)
{
	if (fs::is_regular_file(path))
	{
		fs::remove(path);
		return true;
	}

	return false;
}

std::string Voxel::FileSystem::getWorkingDirectory() const
{
	return workingDirectory;
}

std::string Voxel::FileSystem::getUserDirectory() const
{
	return userDirectory;
}

std::string Voxel::FileSystem::getEnv(const std::string & value)
{
	const char* path = getenv(value.c_str());
	if (path == nullptr)
	{
		return std::string();
	}
	else
	{
		return std::string(path);
	}
}

bool Voxel::FileSystem::doesPathExists(const std::string & path) const
{
	return fs::exists(fs::path(path));
}

bool Voxel::FileSystem::isDirectory(const std::string & path) const
{
	return fs::is_directory(fs::path(path));
}

bool Voxel::FileSystem::openFile(const std::string & path)
{
	if (ofs.is_open())
	{
		return false;
	}

	auto filePath = fs::path(path);

	if (fs::exists(filePath))
	{
		ofs.open(filePath);
		
		if (ofs.is_open())
		{
			return true;
		}
		else
		{
			ofs.close();
		}
	}

	return false;
}

void Voxel::FileSystem::write(const std::string & data)
{
	if (ofs.is_open())
	{
		ofs << data;
	}
}

bool Voxel::FileSystem::closeFile()
{
	if (ofs.is_open())
	{
		ofs.close();
		return true;
	}

	return false;
}
