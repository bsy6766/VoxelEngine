#include "FileSystem.h"

const std::string Voxel::FileSystem::getCurrentPath() const
{
	return std::filesystem::current_path();
}
