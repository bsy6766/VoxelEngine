#include "FileSystem.h"
#include <iostream>
#include <Windows.h>
#include <exception>
#include <boost\filesystem.hpp>
#include <unordered_set>
#include <Utility.h>
#include <Block.h>

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
		std::cout << "[FileSystem] saves directory already exists\n";
		return;
	}
	else
	{
		std::cout << "[FileSystem] Failed to create saves directory\n";
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
			std::cout << "[FileSystem] Created saves directory\n";
		}
	}
	/*
	else
	{
		if (boost::filesystem::is_regular_file(path))
		{
			std::cout << "It's regular file\n";
		}
		else if (boost::filesystem::is_directory(path))
		{
			std::cout << "It's directory\n";
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
			unsigned char byte = static_cast<unsigned char>(x);
			regionFile.write((char *)&byte, 1);
			byte = static_cast<unsigned char>(z);
			regionFile.write((char *)&byte, 1);

			regionFile.flush();
			regionFile.close();
		}
	}
}

void Voxel::FileSystem::saveToRegionFile(const glm::ivec2& regionCoordinate, const glm::ivec2 & chunkCoordinate, const std::vector<Block*>& blocks)
{
	std::string fileName = std::to_string(regionCoordinate.x) + "." + std::to_string(regionCoordinate.y) + ".r";

	std::string folderPath = wd + "/saves/" + curWorldFolder;

	if (boost::filesystem::is_directory(boost::filesystem::path(folderPath)))
	{
		std::ofstream wFile(folderPath + "/" + fileName, std::ofstream::out | std::ofstream::binary);

		if (wFile)
		{
			wFile.seekp(wFile.end);
			auto pos = wFile.tellp();
			std::cout << "pos = " << pos << std::endl;

			char data[4096 * 4] = { 0 };

			int index = 0;

			for (auto block : blocks)
			{
				char id = static_cast<unsigned char>(block->getBlockID());
				auto color = block->getColor3();
				char r = static_cast<char>(color.r * 255.0f);
				char g = static_cast<char>(color.g * 255.0f);
				char b = static_cast<char>(color.b * 255.0f);

				data[index * 4] = id;
				data[index * 4 + 1] = r;
				data[index * 4 + 2] = g;
				data[index * 4 + 3] = b;

				index++;
			}

			wFile.write(data, sizeof(data));
			wFile.flush();
			wFile.close();

			/*
			wFile.flush();
			wFile.seekp(wFile.beg);
			pos = wFile.tellp();
			std::cout << "pos = " << pos << std::endl;
			wFile.seekp(1, wFile.beg);
			pos = wFile.tellp();

			std::cout << "pos = " << pos << std::endl;

			char temp = static_cast<char>(10);

			wFile.write(&temp, 1);
			wFile.flush();


			*/
			wFile.close();
		}
	}

}

void Voxel::FileSystem::readFromRegionFile(const glm::ivec2& regionCoordinate, const glm::ivec2 & chunkCoordinate, std::vector<Block*>& blocks)
{
	std::string fileName = std::to_string(regionCoordinate.x) + "." + std::to_string(regionCoordinate.y) + ".r";

	std::string folderPath = wd + "/saves/" + curWorldFolder;

	if (boost::filesystem::is_directory(boost::filesystem::path(folderPath)))
	{
		std::ifstream rFile(folderPath + "/" + fileName, std::ofstream::out | std::ofstream::binary);

		if (rFile)
		{
			rFile.seekg(rFile.beg);

			char regionX;
			rFile.read(&regionX, 1);
			char regionZ;
			rFile.read(&regionZ, 1);

			std::cout << "Reading region file " << static_cast<int>(regionX) << "." << static_cast<int>(regionZ) << ".r\n";

			char data[4096 * 4] = { 0 };

			int index = 0;

			rFile.read(data, sizeof(data));

			for (int i = 0; i < 1024; i++)
			{
				int blockId = static_cast<int>(data[i * 4]);
				float r = static_cast<unsigned char>(data[i * 4 + 1]);
				float g = static_cast<unsigned char>(data[i * 4 + 2]);
				float b = static_cast<unsigned char>(data[i * 4 + 3]);

				auto block = blocks.at(i);
			}
		}

	}
}

/*
void Voxel::FileSystem::createWorldFolder(const std::string & worldName)
{
	auto worldPath = "./saves/" + worldName;
	if (CreateDirectory(worldPath.c_str(), nullptr))
	{
		std::cout << "[FileSystem] Created world folder at \"" + worldPath + "\"\n";
	}
	else 
	{
		auto lastError = GetLastError();
		if (ERROR_ALREADY_EXISTS == lastError)
		{
			std::cout << "[FileSystem] World folder \"" + worldPath + "\" already exists.\n";
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
		std::cout << "[FileSystem] File \"" + filePath + "\" already exists\n";
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
		std::cout << "Failed to find file: \"" << filePath << "\" to read.\n";

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
		std::cout << "Failed to find file: \"" << filePath << "\" to write.\n";

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