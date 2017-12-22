// pch
#include "PreCompiled.h"

#include "Schematic.h"

Voxel::SchematicBlock::SchematicBlock()
	: pos(0)
	, color(0.0f)
{}





Voxel::SchematicMesh::SchematicMesh()
	: vao(0)
{}

Voxel::SchematicMesh::~SchematicMesh()
{
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
}

void Voxel::SchematicMesh::buildMesh(const std::vector<std::vector<std::vector<SchematicBlock*>>>& blocks)
{
	// greedy mesh?
}





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

bool Voxel::Schematic::isPosInRange(const glm::ivec3 & pos)
{
	return (0 <= pos.x && pos.x < dimension.x) && (0 <= pos.y && pos.y < dimension.y) && (0 <= pos.z && pos.z < dimension.z);
}


bool Voxel::Schematic::addBlock(const glm::ivec3 & pos, const glm::vec3 & color)
{
	if (isPosInRange(pos))
	{
		auto newBlock = new SchematicBlock();
		newBlock->pos = pos;
		newBlock->color = glm::clamp(color, 0.0f, 1.0f);

		blocks.at(pos.y).at(pos.x).at(pos.z) = newBlock;

		return true;
	}

	return false;
}

Voxel::SchematicMesh * Voxel::Schematic::buildMesh()
{
	SchematicMesh* mesh = new SchematicMesh();

	mesh->buildMesh(blocks);

	return mesh;
}
