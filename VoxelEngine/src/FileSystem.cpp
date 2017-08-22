#include "FileSystem.h"
#include <iostream>
#include <Windows.h>
#include <exception>
#include <boost\filesystem.hpp>
#include <unordered_set>
#include <Utility.h>

using namespace Voxel;

FileSystem::FileSystem()
	: wd("")
	, curWorldFolder("")
	, curWorldName("")
{
}

FileSystem::~FileSystem()
{
}

void Voxel::FileSystem::init()
{
	/*
	std::string worldFolderDir = "saves";
	if (CreateDirectory(worldFolderDir.c_str(), nullptr) || ERROR_ALREADY_EXISTS == GetLastError())
	{
		std::cout << "[FileSystem] saves directory already exists" << std::endl;
		return;
	}
	else
	{
		std::cout << "[FileSystem] Failed to create saves directory" << std::endl;
		throw std::runtime_error("Failed to create saves directory");
	}
	*/

	wd = boost::filesystem::current_path().string();
	auto path = boost::filesystem::path(wd + "/saves");

	if (!boost::filesystem::exists(path))
	{
		boost::filesystem::create_directory(path);
		if (boost::filesystem::exists(path))
		{
			std::cout << "[FileSystem] Created saves directory" << std::endl;
		}
	}
	/*
	else
	{
		if (boost::filesystem::is_regular_file(path))
		{
			std::cout << "It's regular file" << std::endl;
		}
		else if (boost::filesystem::is_directory(path))
		{
			std::cout << "It's directory" << std::endl;
		}
	}
	*/
}

void Voxel::FileSystem::createNewWorldSave(const std::string & worldName)
{
	auto savesPath = boost::filesystem::path(wd + "/saves");

	std::unordered_set<std::string> subFolders;

	auto dir_it = boost::filesystem::directory_iterator(savesPath);

	for (auto& entry : dir_it)
	{
		if (boost::filesystem::is_directory(entry))
		{
			subFolders.emplace(entry.path().filename().string());
		}
	}
	
	// world name remains as display name
	auto displayWorldName = worldName;
	// If there is a subdir in saves folder with same name, append dash(-)
	auto worldFolderName = worldName;

	bool found = false;
	while (!found)
	{
		auto find_it = subFolders.find(worldFolderName);
		if (find_it == subFolders.end())
		{
			found = true;
			break;
		}
		else
		{
			worldFolderName += "-";
		}
	}

	curWorldFolder = worldFolderName;
	curWorldName = displayWorldName;

	bool result = boost::filesystem::create_directory(boost::filesystem::path(wd + "/saves/" + worldFolderName));
	if (result)
	{
		// Create meta data
		std::ofstream metaFile(wd + "/saves/" + worldFolderName + "/meta", std::ofstream::out | std::ofstream::binary | std::ofstream::app);
		if (metaFile)
		{
			// In metafile, we add display world name(player defined world name), version, data&time 
			std::string version = "00 00"; // version major and minor
			std::string date = Utility::Time::getDate();
			std::string metaData = displayWorldName + " " + version + " " + date;

			//metaFile.write
			metaFile.write(metaData.c_str(), metaData.size());
			metaFile.flush();
			metaFile.close();

			std::cout << "[FileSystem] World dir set to: \"" + wd + "/saves/" + worldFolderName + ", World Name: " << curWorldName << std::endl;
		}
		else
		{
			throw std::runtime_error("Failed to create meta file");
		}
	}
	else
	{
		throw std::runtime_error("Boost Filesystem failed to create folder: \"" + wd + "/saves/" + worldFolderName + "\"");
	}
}

void Voxel::FileSystem::createRegionFile(const int x, const int z)
{
	std::string fileName = std::to_string(x) + "." + std::to_string(z) + ".r";

	std::string folderPath = wd + "/saves/" + curWorldFolder;

	if (boost::filesystem::is_directory(boost::filesystem::path(folderPath)))
	{
		std::unordered_set<std::string> files;

		auto dir_it = boost::filesystem::directory_iterator(folderPath);

		for (auto& entry : dir_it)
		{
			if (boost::filesystem::is_regular_file(entry))
			{
				files.emplace(entry.path().filename().string());
			}
		}

		if (!files.empty())
		{
			// There is some files already exists in folder. 
			if (files.find(fileName) != files.end())
			{
				// Same file exists
				return;
			}
		}
		std::ofstream regionFile(folderPath + "/" + fileName, std::ofstream::out | std::ofstream::binary | std::ofstream::app);
		if (regionFile)
		{
			regionFile.flush();
			regionFile.close();
		}
	}
}

/*
void Voxel::FileSystem::createWorldFolder(const std::string & worldName)
{
	auto worldPath = "./saves/" + worldName;
	if (CreateDirectory(worldPath.c_str(), nullptr))
	{
		std::cout << "[FileSystem] Created world folder at \"" + worldPath + "\"" << std::endl;
	}
	else 
	{
		auto lastError = GetLastError();
		if (ERROR_ALREADY_EXISTS == lastError)
		{
			std::cout << "[FileSystem] World folder \"" + worldPath + "\" already exists." << std::endl;
			return;
		}
		else
		{
			std::cout << "[FileSystem] Failed to create world folder:\"" + worldPath + "\". ErrorCode: " << lastError << std::endl;
		}
	}
}

void Voxel::FileSystem::createFile(const std::string & filePath)
{
	std::ifstream ifs(filePath.c_str(), std::ifstream::out | std::ifstream::binary);
	if (ifs)
	{
		std::cout << "[FileSystem] File \"" + filePath + "\" already exists" << std::endl;
		ifs.close();
		return;
	}
	else
	{
		//File doesn't exists
		std::ofstream ofs;
		ofs.open(filePath.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::app);
		if (ofs)
		{
			// created a file

			std::cout << "created file: " << filePath << std::endl;

			auto fullHeader = "VOXEL ENGINE 00 00";
			auto len = strlen(fullHeader);
			auto size = sizeof(len);
			ofs.write(fullHeader, strlen(fullHeader));
			ofs.close();
		}
		// Else, failed to create file
		return;
	}
}

bool Voxel::FileSystem::readFile(const std::string & filePath)
{
	rFile.open(filePath.c_str(), std::ifstream::in | std::ifstream::binary);

	if (rFile)
	{
		std::cout << "Read file: " << filePath << std::endl;

		rFile.seekg(0, rFile.end);
		int len = rFile.tellg();
		rFile.seekg(0, rFile.beg);

		char* buffer = new char[28];

		rFile.read(&buffer[0], 28);

		std::cout << "buffer = " << buffer << std::endl;

		delete[] buffer;

		return true;
	}
	else
	{
		std::cout << "Failed to find file: \"" << filePath << "\" to read." << std::endl;

		return false;
	}
}

bool Voxel::FileSystem::writeFile(const std::string & filePath)
{
	wFile.open(filePath.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::app);
	if (wFile)
	{
		std::cout << "Writing to file: " << filePath << std::endl;

		return true;
	}
	else
	{
		std::cout << "Failed to find file: \"" << filePath << "\" to write." << std::endl;

		return false;
	}
}

void Voxel::FileSystem::writeByte(const char byte)
{
	if (wFile)
	{
		wFile.write(&byte, sizeof(char));
		wFile.flush();
	}
	else
	{
		return;
	}
}

char Voxel::FileSystem::readByte()
{
	if (rFile)
	{

	}
	else
	{
		return 0;
	}
}

void Voxel::FileSystem::closeRead()
{
	if (rFile.is_open())
	{
		rFile.close();
	}
}

void Voxel::FileSystem::closeWrite()
{
	if (wFile.is_open())
	{
		wFile.close();
	}
}

*/