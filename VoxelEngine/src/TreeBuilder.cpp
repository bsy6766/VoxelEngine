#include "TreeBuilder.h"
#include <glm\glm.hpp>
#include <ChunkUtil.h>
#include <ChunkMap.h>
#include <Utility.h>

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
		int trunkHeight = 0;

		if (h == Tree::TrunkHeight::SMALL)
		{
			trunkHeight = 10;
		}

		auto pivot = glm::ivec3(treeLocalPos.x, topY, treeLocalPos.y);
		
		pivot.x += (chunkXZ.x * Constant::CHUNK_SECTION_WIDTH);
		pivot.z += (chunkXZ.y * Constant::CHUNK_SECTION_LENGTH);

		// Start from bottom 10 to make sure it renders trunk even it's on steep mountain
		int trunkY = topY - 10;

		auto p1 = glm::ivec3(pivot.x, trunkY, pivot.z);
		auto p2 = glm::ivec3(p1.x + 1, trunkY, p1.z);
		auto p3 = glm::ivec3(p1.x, trunkY, p1.z + 1);
		auto p4 = glm::ivec3(p1.x + 1, trunkY, p1.z + 1);
		trunkHeight += 10;

		for (int i = 0; i < trunkHeight; i++)
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

		// add root
		int rand = Utility::Random::randomInt100();
		int rootHeight = 0;
		int y = pivot.y;

		if (rand < 50)
		{
			// root is visible. 
			y += 1;
			rootHeight = 7;
		}
		else
		{
			y -= 2;
			rootHeight = 5;
		}

		std::vector<glm::ivec3> rootPos =
		{
			glm::ivec3(p1.x, y, p1.z - 1),
			glm::ivec3(p2.x, y, p2.z - 1),
			glm::ivec3(p2.x + 1, y, p2.z),
			glm::ivec3(p4.x + 1, y, p4.z),
			glm::ivec3(p4.x, y ,p4.z + 1),
			glm::ivec3(p3.x, y, p3.z + 1),
			glm::ivec3(p3.x - 1, y, p3.z),
			glm::ivec3(p1.x - 1, y, p1.z)
		};


		for (int i = 0; i < rootHeight; i++)
		{
			for (auto& rp : rootPos)
			{
				if (rp.y == y)
				{
					if (Utility::Random::randomInt100() < 50)
					{
						rp.y--;
						continue;
					}
				}
				chunkMap->placeBlockAt(rp, Block::BLOCK_ID::OAK_WOOD, nullptr);

				rp.y--;
			}
		}

		// add branch

		// add leaves
		auto l1 = glm::ivec3(p1.x, p1.y + 3, p1.z);

		int leavesWidth = 5;
		int leavesHeight = 3;
		int leavesLength = 3;
		
		float aa = static_cast<float>(leavesWidth * leavesWidth);
		float bb = static_cast<float>(leavesHeight * leavesHeight);
		float cc = static_cast<float>(leavesLength * leavesLength);

		float aabbcc = aa * bb * cc;

		int xStart = -leavesWidth ;
		int yStart = -leavesHeight;
		int zStart = -leavesLength;

		int xEnd = leavesWidth;
		int yEnd = leavesHeight;
		int zEnd = leavesLength;

		for (int x = xStart; x <= xEnd; x++)
		{
			float xx = glm::abs(static_cast<float>(x * x));
			for (int z = zStart; z <= zEnd; z++)
			{
				float zz = glm::abs(static_cast<float>(z * z));
				for (int y = yStart; y <= yEnd; y++)
				{
					float yy = glm::abs(static_cast<float>(y * y));
					float val = (xx * bb * cc) + (yy * aa * cc) + (zz * aa * bb);
					if (val <= aabbcc)
					{
						auto lp = l1 + glm::ivec3(x, y, z);
						chunkMap->placeBlockAt(lp, Block::BLOCK_ID::OAK_LEAVES, nullptr);
					}
				}
			}
		}
	}
}
