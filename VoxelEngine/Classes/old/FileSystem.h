#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

// cpp
#include <string>
#include <fstream>
#include <unordered_map>
#include <filesystem>

// glm
#include <glm/glm.hpp>

namespace Voxel
{
	class Block;

	/**
	*	@class FileSystem
	*	@brief Singleton class that reads and writes file
	*
	*	
	*/
	class FileSystem2
	{
	public:
		~FileSystem2();
	private:
		FileSystem2();

		// Delete copy, move, assign operators
		FileSystem2(FileSystem2 const&) = delete;             // Copy construct
		FileSystem2(FileSystem2&&) = delete;                  // Move construct
		FileSystem2& operator=(FileSystem2 const&) = delete;  // Copy assign
		FileSystem2& operator=(FileSystem2 &&) = delete;      // Move assign
	public:
		static FileSystem2& getInstance()
		{
			static FileSystem2 instance;
			return instance;
		}
	};

	/**
	*	@class FileSystem
	*	@brief Singleton class that reads and writes file
	*
	*	File system manages file create/read/write/delete functions.
	*	File system only reads single world folder at a time.
	*	
	*	Uses boost filesystem for cross platform
	*/
	class FileSystem
	{
	public:
		~FileSystem();
	private:
		FileSystem();

		// Delete copy, move, assign operators
		FileSystem(FileSystem const&) = delete;             // Copy construct
		FileSystem(FileSystem&&) = delete;                  // Move construct
		FileSystem& operator=(FileSystem const&) = delete;  // Copy assign
		FileSystem& operator=(FileSystem &&) = delete;      // Move assign
		
		std::string wd;

		std::string curWorldFolder;
		std::string curWorldName;
	public:
		static FileSystem& getInstance()
		{
			static FileSystem instance;
			return instance;
		}

		// Checks if it can create saves directory and attempt to make one. Throws runtime error if fails. Game can't be go on without save.
		void init();

		// Creates save files for new world
		// This creates sub directory in the saves folder.
		// Then it creates meta data. Meta data contains the player defined world name(not foldername), version and time&date it created
		void createNewWorldSave(const std::string& worldName);

		// Opens exisiting world folder
		void openWorldSave(const std::string& worldDisplayName);

		// Creates region file.
		void createRegionFile(const int x, const int z);

		// Save all block data to file
		void saveToRegionFile(const glm::ivec2& regionCoordinate, const glm::ivec2& chunkCoordinate, const std::vector<Block*>& blocks);
		// Read all black data from file
		void readFromRegionFile(const glm::ivec2& regionCoordinate, const glm::ivec2& chunkCoordinate, std::vector<Block*>& blocks);
	};
}

#endif