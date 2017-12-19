#include "Schematic.h"

Voxel::Schematic::Schematic(const glm::ivec3 & dimension)
	: dimension(dimension)
{
	for (int y = 0; y < dimension.y; ++y)
	{
		blocks.push_back(std::vector<std::vector<SchematicBlock*>>());
		for (int x = 0; x < dimension.x; ++x)
		{
			blocks.back().push_back(std::vector<SchematicBlock*>());
			blocks.back().back().resize(dimension.z, nullptr);
		}
	}
}

Voxel::Schematic::~Schematic()
{
	for (auto& y : blocks)
	{
		for (auto& x : y)
		{
			for (auto z : x)
			{
				if (z)
				{
					delete z;
				}
			}
		}
	}

	blocks.clear();
}


