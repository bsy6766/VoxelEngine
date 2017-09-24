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
		trunkHeight = Utility::Random::randomInt(14, 16);
	}
	else if (h == TreeBuilder::TrunkHeight::MEDIUM)
	{
		trunkHeight = Utility::Random::randomInt(16, 20);
	}
	else if (h == TreeBuilder::TrunkHeight::LARGE)
	{
		trunkHeight = Utility::Random::randomInt(20, 24);
	}
	else if (h == TreeBuilder::TrunkHeight::MEGA)
	{
		trunkHeight = Utility::Random::randomInt(30, 34);
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
		int sizeHalf = size / 2;

		auto oakWoodColor = Color::colorU3TocolorV3(Color::OAK_WOOD);
		auto colorStep = oakWoodColor * 0.025f;

		oakWoodColor = oakWoodColor - (colorStep * (static_cast<float>(trunkHeight) * 0.5f));

		auto trunkBottomColor = oakWoodColor;

		/*
		glm::ivec3 offset = glm::ivec3(0);
		int offsetIndex = 10 + (trunkHeight / 2);

		if (h == TreeBuilder::TrunkHeight::LARGE)
		{
			offset = glm::ivec3(1, 0, 0);
		}
		*/
		

		for (int i = 0; i < size; i++)
		{
			if (i >= 10)
			{
				if (i <= 17)
				{
					oakWoodColor += colorStep;
				}
				else
				{
					oakWoodColor -= colorStep;
				}

				oakWoodColor = glm::clamp(oakWoodColor, 0.0f, 1.0f);
			}

			/*
			if (i >= offsetIndex)
			{

				chunkMap->placeBlockAt(p1 + offset, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				chunkMap->placeBlockAt(p2 + offset, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				chunkMap->placeBlockAt(p3 + offset, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				chunkMap->placeBlockAt(p4 + offset, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
			}
			else
			{
				chunkMap->placeBlockAt(p1, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				chunkMap->placeBlockAt(p2, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				chunkMap->placeBlockAt(p3, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				chunkMap->placeBlockAt(p4, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
			}
			*/

			chunkMap->placeBlockAt(p1, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
			chunkMap->placeBlockAt(p2, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
			chunkMap->placeBlockAt(p3, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
			chunkMap->placeBlockAt(p4, Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);

			p1.y++;
			p2.y++;
			p3.y++;
			p4.y++;
		}


		// Add more blocks around bottom of trunk
		int trunkBotY = pivot.y;

		p1.y = trunkBotY;
		p2.y = trunkBotY;
		p3.y = trunkBotY;
		p4.y = trunkBotY;

		auto p5 = glm::ivec3(p1.x, trunkBotY, p1.z - 1);
		auto p6 = glm::ivec3(p2.x, trunkBotY, p2.z - 1);
		auto p7 = glm::ivec3(p2.x + 1, trunkBotY, p2.z);
		auto p8 = glm::ivec3(p4.x + 1, trunkBotY, p4.z);
		auto p9 = glm::ivec3(p4.x, trunkBotY, p4.z + 1);
		auto p10 = glm::ivec3(p3.x, trunkBotY, p3.z + 1);
		auto p11 = glm::ivec3(p3.x - 1, trunkBotY, p3.z);
		auto p12 = glm::ivec3(p1.x - 1, trunkBotY, p1.z);

		auto p13 = glm::ivec3(p5.x, trunkBotY, p5.z - 1);
		auto p14 = glm::ivec3(p6.x, trunkBotY, p6.z - 1);
		auto p15 = glm::ivec3(p7.x + 1, trunkBotY, p7.z);
		auto p16 = glm::ivec3(p8.x + 1, trunkBotY, p8.z);
		auto p17 = glm::ivec3(p9.x, trunkBotY, p9.z + 1);
		auto p18 = glm::ivec3(p10.x, trunkBotY, p10.z + 1);
		auto p19 = glm::ivec3(p11.x - 1, trunkBotY, p11.z);
		auto p20 = glm::ivec3(p12.x - 1, trunkBotY, p12.z);

		auto p21 = glm::ivec3(p1.x - 1, trunkBotY, p1.z - 1);
		auto p22 = glm::ivec3(p2.x + 1, trunkBotY, p2.z - 1);
		auto p23 = glm::ivec3(p4.x + 1, trunkBotY, p4.z + 1);
		auto p24 = glm::ivec3(p3.x - 1, trunkBotY, p3.z + 1);

		chunkMap->placeBlockAt(p5, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
		chunkMap->placeBlockAt(p6, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
		chunkMap->placeBlockAt(p7, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
		chunkMap->placeBlockAt(p8, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
		chunkMap->placeBlockAt(p9, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
		chunkMap->placeBlockAt(p10, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
		chunkMap->placeBlockAt(p11, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
		chunkMap->placeBlockAt(p12, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);

		int tRand = Utility::Random::randomInt(0, 3);

		if (tRand == 0)
		{
			/*
					p9  p10
				p8  p4  p3  p11
				p7  p2  p1  p12
					p6  p5
			*/
			// done
		}
		else if (tRand == 1)
		{
			/*
						p17 p18
						p9  p10
				p16 p8  p4  p3  p11 p19
				p15 p7  p2  p1  p12 p20
						p6  p5
						p14 p13
			*/


			p5.y++;
			p6.y++;
			p7.y++;
			p8.y++;
			p9.y++;
			p10.y++;
			p11.y++;
			p12.y++;

			chunkMap->placeBlockAt(p5, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p6, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p7, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p8, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p9, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p10, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p11, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p12, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);

			p5.y--;
			p6.y--;
			p7.y--;
			p8.y--;
			p9.y--;
			p10.y--;
			p11.y--;
			p12.y--;

			chunkMap->placeBlockAt(p13, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p14, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p15, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p16, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p17, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p18, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p19, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p20, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
		}
		else if (tRand == 2)
		{
			/*
						p17 p18
					p23	p9  p10 p24
				p16 p8  p4  p3  p11 p19
				p15 p7  p2  p1  p12 p20
					p22	p6  p5  p21
						p14 p13
			*/
			p5.y++;
			p6.y++;
			p7.y++;
			p8.y++;
			p9.y++;
			p10.y++;
			p11.y++;
			p12.y++;

			chunkMap->placeBlockAt(p5, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p6, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p7, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p8, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p9, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p10, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p11, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p12, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);

			p5.y++;
			p6.y++;
			p7.y++;
			p8.y++;
			p9.y++;
			p10.y++;
			p11.y++;
			p12.y++;

			chunkMap->placeBlockAt(p5, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p6, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p7, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p8, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p9, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p10, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p11, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p12, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);

			p5.y -= 2;
			p6.y -= 2;
			p7.y -= 2;
			p8.y -= 2;
			p9.y -= 2;
			p10.y -= 2;
			p11.y -= 2;
			p12.y -= 2;

			chunkMap->placeBlockAt(p13, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p14, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p15, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p16, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p17, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p18, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p19, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p20, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);

			chunkMap->placeBlockAt(p21, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p22, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p23, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p24, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);

			p21.y++;
			p22.y++;
			p23.y++;
			p24.y++;

			chunkMap->placeBlockAt(p21, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p22, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p23, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			chunkMap->placeBlockAt(p24, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);

			p21.y--;
			p22.y--;
			p23.y--;
			p24.y--;
		}
		// Else, tRand is 3. No additioanl block around bttom of trunk

		int rootHeight = 10;
		
		/*
					p17 p18
				p23	p9  p10 p24
			p16 p8  p4  p3  p11 p19
			p15 p7  p2  p1  p12 p20
				p22	p6  p5  p21
					p14 p13
		*/

		p1.y--;
		p2.y--;
		p3.y--;
		p4.y--;

		p5.y--;
		p6.y--;
		p7.y--;
		p8.y--;
		p9.y--;
		p10.y--;
		p11.y--;
		p12.y--;

		p13.y--;
		p14.y--;
		p15.y--;
		p16.y--;
		p17.y--;
		p18.y--;
		p19.y--;
		p20.y--;

		p21.y--;
		p22.y--;
		p23.y--;
		p24.y--;

		for (int i = 0; i < rootHeight; i++)
		{
			chunkMap->placeBlockAt(p1, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p2, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p3, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p4, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);

			chunkMap->placeBlockAt(p5, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p6, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p7, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p8, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p9, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p10, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p11, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p12, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);

			chunkMap->placeBlockAt(p13, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p14, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p15, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p16, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p17, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p18, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p19, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p20, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);

			chunkMap->placeBlockAt(p21, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p22, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p23, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
			chunkMap->placeBlockAt(p24, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);

			p1.y--;
			p2.y--;
			p3.y--;
			p4.y--;

			p5.y--;
			p6.y--;
			p7.y--;
			p8.y--;
			p9.y--;
			p10.y--;
			p11.y--;
			p12.y--;

			p13.y--;
			p14.y--;
			p15.y--;
			p16.y--;
			p17.y--;
			p18.y--;
			p19.y--;
			p20.y--;

			p21.y--;
			p22.y--;
			p23.y--;
			p24.y--;
		}

		// add main leave above trunk
		int leavesWidth = Utility::Random::randomInt(8, 10);
		int leavesHeight = Utility::Random::randomInt(6, 7);
		int leavesLength = Utility::Random::randomInt(8, 10);

		//auto l1 = glm::ivec3(p1.x, pivot.y + trunkHeight + (leavesHeight / 2), p1.z);
		auto l1 = glm::ivec3(p1.x, pivot.y + trunkHeight + 2, p1.z);

		int mlRand = Utility::Random::randomInt100();
		if (mlRand < 50)
		{
			addOakLeave(chunkMap, leavesWidth, leavesHeight, leavesLength, l1);
		}
		else
		{
			l1.x -= 1;
			l1.y -= 2;
			addOakLeave(chunkMap, leavesWidth, leavesHeight / 3 * 2, leavesLength, l1);

			l1.x += 1;
			l1.z += Utility::Random::randomInt(3, 4) * Utility::Random::random_int_minus_1_1();
			l1.y += 3;
			addOakLeave(chunkMap, leavesWidth, leavesHeight / 3 * 2, leavesLength, l1);
		}

		// Add additional leaves near main leaves
		int totalSideLeaves = Utility::Random::randomInt(1, 3);

		for (int i = 0; i < totalSideLeaves; i++)
		{
			int sideLeavesWidth = Utility::Random::randomInt(3, 4);
			int sideLeavesHeight = Utility::Random::randomInt(2, 3);
			int sideLeavesLength = Utility::Random::randomInt(3, 4);

			int x = 0;
			int y = 0;
			int z = 0;

			glm::ivec3 pos(0);

			if (mlRand)
			{
				x = Utility::Random::randomInt(leavesWidth / 2, (leavesWidth / 4) * 3) * Utility::Random::random_int_minus_1_1();
				y = Utility::Random::randomInt(0, leavesHeight / 2);
				z = Utility::Random::randomInt(leavesLength / 2, (leavesLength / 4) * 3) * Utility::Random::random_int_minus_1_1();
				pos = glm::ivec3(x, y, z) + l1;
			}
			else
			{
				x = Utility::Random::randomInt((leavesWidth / 4), leavesWidth / 2) * Utility::Random::random_int_minus_1_1();
				y = Utility::Random::randomInt(0, leavesHeight / 2);
				z = Utility::Random::randomInt((leavesLength / 4) * 3, leavesWidth / 2) * Utility::Random::random_int_minus_1_1();
				pos = glm::ivec3(x, y, z) + l1;
			}

			addOakLeave(chunkMap, sideLeavesWidth, sideLeavesHeight, sideLeavesLength, pos);
		}

		// Add more blocks around top of trunk, below the leaves
		{
			int newY = pivot.y + trunkHeight;		
			
			p5.y = newY;
			p6.y = newY;
			p7.y = newY;
			p8.y = newY;
			p9.y = newY;
			p10.y = newY;
			p11.y = newY;
			p12.y = newY;

			for (int i = 0; i < 7; i++)
			{
				chunkMap->placeBlockAt(p5, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				chunkMap->placeBlockAt(p6, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				chunkMap->placeBlockAt(p7, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				chunkMap->placeBlockAt(p8, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				chunkMap->placeBlockAt(p9, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				chunkMap->placeBlockAt(p10, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				chunkMap->placeBlockAt(p11, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				chunkMap->placeBlockAt(p12, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);

				p5.y--;
				p6.y--;
				p7.y--;
				p8.y--;
				p9.y--;
				p10.y--;
				p11.y--;
				p12.y--;
			}
		}

		// add branch
		int totalBranch = Utility::Random::randomInt(1, 4);
		/*
					p17 p18
					p9  p10 
			p16 p8  p4  p3  p11 p19
			p15 p7  p2  p1  p12 p20
					p6  p5
					p14 p13
		*/

		if (totalBranch)
		{
			// one branch
			int branchY = pivot.y + trunkHeight + 2 - 3 - leavesHeight - Utility::Random::randomInt(1, 2);

			auto b1 = glm::ivec3(0);
			auto b2 = glm::ivec3(0);
			
			int randDir = Utility::Random::randomInt(0, 3);

			if (randDir == 0)
			{
				b1 = p11;
				b2 = p12;
			}
			else if (randDir == 1)
			{
				b1 = p9;
				b2 = p10;
			}
			else if (randDir == 2)
			{
				b1 = p7;
				b2 = p8;
			}
			else
			{
				b1 = p5;
				b2 = p6;
			}

			b1.y = branchY;
			b2.y = branchY;

			for (int i = 0; i < 4; i++)
			{
				chunkMap->placeBlockAt(b1, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				chunkMap->placeBlockAt(b2, Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);

				if (randDir == 0)
				{
					b1.x--;
					b2.x--;
				}
				else if (randDir == 1)
				{
					b1.z++;
					b2.z++;
				}
				else if (randDir == 2)
				{
					b1.x++;
					b2.x++;
				}
				else
				{
					b1.z--;
					b2.z--;
				}
				
				b1.y++;
				b2.y++;
			}

			int branchLeaveWidth = Utility::Random::randomInt(2, 3);
			int branchLeavesHeight = Utility::Random::randomInt(1, 2);
			int branchLeavesLength = Utility::Random::randomInt(2, 3);

			b1.y + (branchLeavesHeight / 2);
			
			addOakLeave(chunkMap, branchLeaveWidth, branchLeavesHeight, branchLeavesLength, b1);
		}
		else if (totalBranch == 2)
		{
			int branchY1 = pivot.y + trunkHeight + 2 - 3 - leavesHeight - Utility::Random::randomInt(1, 2);
			int branchY2 = pivot.y + trunkHeight + 2 - 3 - leavesHeight - Utility::Random::randomInt(1, 2);

			auto b1 = glm::ivec3(0);
			auto b2 = glm::ivec3(0);
			auto b3 = glm::ivec3(0);
			auto b4 = glm::ivec3(0);
		}
	}
	else if (w == TreeBuilder::TrunkWidth::MEDIUM)
	{
	}
}

void Voxel::TreeBuilder::addOakLeave(ChunkMap * map, const int w, const int h, const int l, const glm::ivec3 & pos)
{
	float aa = static_cast<float>(w * w);
	float bb = static_cast<float>(h * h);
	float cc = static_cast<float>(l * l);

	float aabbcc = aa * bb * cc;

	int xStart = -w;
	int yStart = -h;
	int zStart = -l;

	int xEnd = w;
	int yEnd = h;
	int zEnd = l;

	bool skew = false;

	int yrand = Utility::Random::randomInt100();
	glm::ivec3 leaveOffset = glm::ivec3(0);

	if (yrand < 40)
	{
		skew = true;
		int xzRand = Utility::Random::randomInt100();
		int dirRand = Utility::Random::randomInt100();
		if (xzRand < 50)
		{
			if (dirRand)
			{
				leaveOffset.x++;
			}
			else
			{
				leaveOffset.x--;
			}
		}
		else
		{
			if (dirRand)
			{
				leaveOffset.z++;
			}
			else
			{
				leaveOffset.z--;
			}
		}
	}

	auto leaveColor = Color::colorU3TocolorV3(Color::OAK_LEAVES);
	auto leaveColorStep = leaveColor * 0.05f;

	leaveColor -= (leaveColorStep * (static_cast<float>(h)));

	for (int y = yStart; y <= yEnd; y++)
	{
		float yf = static_cast<float>(y) + 0.5f;
		float yy = yf * yf;

		for (int x = xStart; x <= xEnd; x++)
		{
			float xf = static_cast<float>(x) - 0.5f;
			float xx = xf * xf;

			for (int z = zStart; z <= zEnd; z++)
			{
				float zf = static_cast<float>(z) - 0.5f;
				float zz = zf * zf;

				float val = (xx * bb * cc) + (yy * aa * cc) + (zz * aa * bb);
				if (val <= aabbcc)
				{
					auto lp = pos + glm::ivec3(x, y, z);

					if (y >= 0)
					{
						if (skew)
						{
							map->placeBlockAt(lp + leaveOffset, Block::BLOCK_ID::OAK_LEAVES, leaveColor, nullptr, false);
						}
						else
						{
							map->placeBlockAt(lp, Block::BLOCK_ID::OAK_LEAVES, leaveColor, nullptr, false);
						}
					}
					else
					{
						map->placeBlockAt(lp, Block::BLOCK_ID::OAK_LEAVES, leaveColor, nullptr, false);
					}
				}
			}
		}

		leaveColor += leaveColorStep;
		leaveColor = glm::clamp(leaveColor, 0.0f, 1.0f);
	}
}