#include "TreeBuilder.h"
#include <glm\glm.hpp>
#include <ChunkUtil.h>
#include <ChunkMap.h>

using namespace Voxel;


Voxel::Tree::Tree()
{
}

bool Voxel::Tree::init(Type type, TrunkHeight height, TrunkWidth width)
{
	this->type = type;
	this->height = height;
	this->width = width;

	for (int i = 0; i < 4096; i++)
	{
		//blocks.push_back(Block::BLOCK_ID::AIR);
	}

	auto startXZ = glm::ivec3(8, 0, 8);

	buildTrunk();


	return true;
}

void Voxel::Tree::buildTrunk()
{
	int trunkHeight = 0;

	switch (this->height)
	{
	case TrunkHeight::SMALL:
		trunkHeight = 10;
		break;
	case TrunkHeight::MEDIUM:
		break;
	case TrunkHeight::LARGE:
		break;
	case TrunkHeight::MEGA:
		break;
	default:
		return;
		break;
	}

	if (trunkHeight == 0) return;

	auto pivot = glm::ivec3(8, 0, 8);

	if (width == TrunkWidth::SMALL)
	{
		/**
		*		@@
		*		&@
		*/
		
		auto p1 = pivot;
		auto p2 = glm::ivec3(pivot.x + 1, pivot.y, pivot.z);
		auto p3 = glm::ivec3(pivot.x, pivot.y, pivot.z + 1);
		auto p4 = glm::ivec3(pivot.x + 1, pivot.y, pivot.z + 1);

		for (int y = 0; y < trunkHeight; y++)
		{
			//blocks.at(xyzToIndex(pivot.x, y, pivot.z)) = Block::BLOCK_ID::OAK_WOOD;
		}
		
	}
	else if (width == TrunkWidth::MEDIUM)
	{
		/**
		*		 @@
		*		@@@@
		*		@&@@
		*		 @@
		*/

	}
	else if (width == TrunkWidth::LARGE)
	{
		/**
		*		  @@
		*		 @@@@
		*		@@@@@@
		*		@@&@@@
		*		 @@@@
		*		  @@
		*/

	}
	else if (width == TrunkWidth::MEGA)
	{
		//Todo: add mega tree
	}
}

unsigned int Voxel::Tree::xyzToIndex(const int x, const int y, const int z)
{
	return x + (Constant::CHUNK_SECTION_WIDTH * z) + (y * Constant::CHUNK_SECTION_LENGTH * Constant::CHUNK_SECTION_WIDTH);
}

Voxel::Tree::~Tree()
{
}


Tree * Voxel::TreeBuilder::createOakTree(const Tree::TrunkHeight h, const Tree::TrunkWidth w)
{
	Tree* newTree = new Tree();
	if (newTree->init(Tree::Type::OAK, h, w))
	{
		return newTree;
	}
	
	delete newTree;
	return nullptr;
}

void Voxel::TreeBuilder::createTree(const Tree::Type type, const Tree::TrunkHeight h, const Tree::TrunkWidth w, ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec2& treeLocalPos, const int topY)
{
	//  build trunk and root.
	if (w == Tree::TrunkWidth::SMALL)
	{
		int height = 0;

		if (h == Tree::TrunkHeight::SMALL)
		{
			height = 10;
		}

		auto p1 = glm::ivec3(treeLocalPos.x, topY, treeLocalPos.y);
		
		p1.x += (chunkXZ.x * Constant::CHUNK_SECTION_WIDTH);
		p1.z += (chunkXZ.y * Constant::CHUNK_SECTION_LENGTH);

		auto p2 = glm::ivec3(p1.x + 1, topY, p1.z);
		auto p3 = glm::ivec3(p1.x, topY, p1.z + 1);
		auto p4 = glm::ivec3(p1.x + 1, topY, p1.z + 1);

		for (int i = 0; i < height; i++)
		{
			chunkMap->placeBlockAt(p1, Block::BLOCK_ID::OAK_WOOD, nullptr);
			chunkMap->placeBlockAt(p2, Block::BLOCK_ID::OAK_WOOD, nullptr);
			chunkMap->placeBlockAt(p3, Block::BLOCK_ID::OAK_WOOD, nullptr);
			chunkMap->placeBlockAt(p4, Block::BLOCK_ID::OAK_WOOD, nullptr);

			p1.y++;
			p2.y++;
			p3.y++;
			p4.y++;
		}

		p1.y -= 11;
		p2.y -= 11;
		p3.y -= 11;
		p4.y -= 11;

		// add root
	}
}
