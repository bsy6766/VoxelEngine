#include "TreeBuilder.h"
#include <glm\glm.hpp>
#include <ChunkUtil.h>
#include <ChunkMap.h>
#include <Color.h>
#include <Utility.h>

using namespace Voxel;

void Voxel::TreeBuilder::createTree(const TreeBuilder::TreeType type, const TreeBuilder::TrunkHeight h, const TreeBuilder::TrunkWidth w, ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec2& treeLocalXZ, const int treeY)
{
	switch (type)
	{
	case TreeBuilder::TreeType::OAK:
		TreeBuilder::createOakTree(h, w, chunkMap, chunkXZ, treeLocalXZ, treeY);
		break;
	default:
		break;
	}
}

void Voxel::TreeBuilder::createOakTree(const TreeBuilder::TrunkHeight h, const TreeBuilder::TrunkWidth w, ChunkMap * chunkMap, const glm::ivec2 & chunkXZ, const glm::ivec2 & treeLocalXZ, const int treeY)
{
	int trunkHeight = 0;

	if (h == TreeBuilder::TrunkHeight::SMALL)
	{
		trunkHeight = Utility::Random::randomInt(10, 14);
	}
	else if (h == TreeBuilder::TrunkHeight::MEDIUM)
	{
		trunkHeight = Utility::Random::randomInt(14, 18);
	}
	else if (h == TreeBuilder::TrunkHeight::LARGE)
	{
		trunkHeight = Utility::Random::randomInt(18, 22);
	}
	else if (h == TreeBuilder::TrunkHeight::MEGA)
	{
		trunkHeight = Utility::Random::randomInt(26, 30);
	}
	else
	{
		return;
	}

	auto pivot = glm::ivec3(treeLocalXZ.x, treeY, treeLocalXZ.y);
	pivot.x += (chunkXZ.x * Constant::CHUNK_SECTION_WIDTH);
	pivot.z += (chunkXZ.y * Constant::CHUNK_SECTION_LENGTH);

	//  build trunk and root.
	if (w == TreeBuilder::TrunkWidth::SMALL)
	{
		// Start from bottom 10 to make sure it renders trunk even it's on steep mountain
		int trunkY = pivot.y - 10;

		/*
			p4 p3
			p2 p1
		*/
		auto p1 = glm::ivec3(pivot.x, trunkY, pivot.z);
		auto p2 = glm::ivec3(p1.x + 1, trunkY, p1.z);
		auto p3 = glm::ivec3(p1.x, trunkY, p1.z + 1);
		auto p4 = glm::ivec3(p1.x + 1, trunkY, p1.z + 1);

		int size = trunkHeight + 10;

		auto oakWoodColor = Color::colorU3TocolorV3(Color::OAK_WOOD);

		for (int i = 0; i < size; i++)
		{
			chunkMap->placeBlockAt(p1, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
			chunkMap->placeBlockAt(p2, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
			chunkMap->placeBlockAt(p3, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
			chunkMap->placeBlockAt(p4, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);

			p1.y++;
			p2.y++;
			p3.y++;
			p4.y++;

			//oakWoodColor *= 0.975f;

			//oakWoodColor = glm::clamp(oakWoodColor, 0.0f, 1.0f);
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

		/*
				r5  r6
			r4  p4  p3  r7
			r3  p2  p1  r8
				r2  r1
		*/
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


		return;
		// add leaves
		auto l1 = glm::ivec3(p1.x, p1.y + 2, p1.z);

		int leavesWidth = Utility::Random::randomInt(8, 10);
		int leavesHeight = Utility::Random::randomInt(3, 4);
		int leavesLength = Utility::Random::randomInt(8, 10);

		float aa = static_cast<float>(leavesWidth * leavesWidth);
		float bb = static_cast<float>(leavesHeight * leavesHeight);
		float cc = static_cast<float>(leavesLength * leavesLength);

		float aabbcc = aa * bb * cc;

		int xStart = -leavesWidth;
		int yStart = -leavesHeight;
		int zStart = -leavesLength;

		int xEnd = leavesWidth;
		int yEnd = leavesHeight;
		int zEnd = leavesLength;

		bool skew = false;

		int yrand = Utility::Random::randomInt100();
		if (yrand < 50)
		{
			skew = true;
		}

		for (int x = xStart; x <= xEnd; x++)
		{
			float xf = static_cast<float>(x) - 0.5f;
			//float xx = glm::abs(static_cast<float>(x * x));
			float xx = xf * xf;
			for (int z = zStart; z <= zEnd; z++)
			{
				float zf = static_cast<float>(z) - 0.5f;
				float zz = zf * zf;
				//float zz = glm::abs(static_cast<float>(z * z));
				for (int y = yStart; y <= yEnd; y++)
				{
					float yf = static_cast<float>(y) + 0.5f;
					float yy = yf * yf;
					//float yy = glm::abs(static_cast<float>(y * y));
					float val = (xx * bb * cc) + (yy * aa * cc) + (zz * aa * bb);
					if (val <= aabbcc)
					{
						auto lp = l1 + glm::ivec3(x, y, z);

						if (y >= 0)
						{
							if (skew)
							{
								lp.x++;
								lp.z++;
							}
						}
						chunkMap->placeBlockAt(lp, Block::BLOCK_ID::OAK_LEAVES, nullptr);
					}
				}
			}
		}
	}
	else if (w == TreeBuilder::TrunkWidth::MEDIUM)
	{
		// Start from bottom 10 to make sure it renders trunk even it's on steep mountain
		int trunkY = pivot.y - 10;

		/*
				p9  p10
			p8  p4  p3  p11
			p7  p2  p1  p12
				p6  p5
		*/

		auto p1 = glm::ivec3(pivot.x, trunkY, pivot.z);
		auto p2 = glm::ivec3(p1.x + 1, trunkY, p1.z);
		auto p3 = glm::ivec3(p1.x, trunkY, p1.z + 1);
		auto p4 = glm::ivec3(p1.x + 1, trunkY, p1.z + 1);
		auto p5 = glm::ivec3(p1.x, trunkY, p1.z - 1);
		auto p6 = glm::ivec3(p2.x, trunkY, p2.z - 1);
		auto p7 = glm::ivec3(p2.x + 1, trunkY, p2.z);
		auto p8 = glm::ivec3(p4.x + 1, trunkY, p4.z);
		auto p9 = glm::ivec3(p4.x, trunkY, p4.z + 1);
		auto p10 = glm::ivec3(p3.x, trunkY, p3.z + 1);
		auto p11 = glm::ivec3(p3.x - 1, trunkY, p3.z);
		auto p12 = glm::ivec3(p1.x - 1, trunkY, p1.z);

		int size = trunkHeight + 10;

		for (int i = 0; i < size; i++)
		{
			chunkMap->placeBlockAt(p1, Block::BLOCK_ID::OAK_WOOD, nullptr);
			chunkMap->placeBlockAt(p2, Block::BLOCK_ID::OAK_WOOD, nullptr);
			chunkMap->placeBlockAt(p3, Block::BLOCK_ID::OAK_WOOD, nullptr);
			chunkMap->placeBlockAt(p4, Block::BLOCK_ID::OAK_WOOD, nullptr);
			chunkMap->placeBlockAt(p5, Block::BLOCK_ID::OAK_WOOD, nullptr);
			chunkMap->placeBlockAt(p6, Block::BLOCK_ID::OAK_WOOD, nullptr);
			chunkMap->placeBlockAt(p7, Block::BLOCK_ID::OAK_WOOD, nullptr);
			chunkMap->placeBlockAt(p8, Block::BLOCK_ID::OAK_WOOD, nullptr);
			chunkMap->placeBlockAt(p9, Block::BLOCK_ID::OAK_WOOD, nullptr);
			chunkMap->placeBlockAt(p10, Block::BLOCK_ID::OAK_WOOD, nullptr);
			chunkMap->placeBlockAt(p11, Block::BLOCK_ID::OAK_WOOD, nullptr);
			chunkMap->placeBlockAt(p12, Block::BLOCK_ID::OAK_WOOD, nullptr);

			p1.y++;
			p2.y++;
			p3.y++;
			p4.y++;
			p5.y++;
			p6.y++;
			p7.y++;
			p8.y++;
			p9.y++;
			p10.y++;
			p11.y++;
			p12.y++;
		}

	}
}