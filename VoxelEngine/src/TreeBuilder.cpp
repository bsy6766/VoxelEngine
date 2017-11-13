#include "TreeBuilder.h"
#include <glm\glm.hpp>
#include <ChunkUtil.h>
#include <ChunkMap.h>
#include <Color.h>
#include <Utility.h>
#include <Random.h>
#include <glm/gtx/rotate_vector.hpp>

using namespace Voxel;

void Voxel::TreeBuilder::createTree(const Voxel::Vegitation::Tree type, ChunkMap * chunkMap, const glm::ivec2 & chunkXZ, const glm::ivec3& treeLocalPos, std::mt19937 & engine)
{
	switch (type)
	{
	case Voxel::Vegitation::Tree::OAK:
		TreeBuilder::createOakTree(chunkMap, chunkXZ, treeLocalPos, engine);
		break;
	case Voxel::Vegitation::Tree::BIRCH:
		TreeBuilder::createBirchTree(chunkMap, chunkXZ, treeLocalPos, engine);
		break;
	case Voxel::Vegitation::Tree::SPRUCE:
		TreeBuilder::createSpruceTree(chunkMap, chunkXZ, treeLocalPos, engine);
		break;
	case Voxel::Vegitation::Tree::PINE:
		TreeBuilder::createPineTree(chunkMap, chunkXZ, treeLocalPos, engine);
		break;
	default:
		break;
	}
}

void Voxel::TreeBuilder::createTree(const Voxel::Vegitation::Tree type, const TreeBuilder::TrunkHeightType h, const TreeBuilder::TrunkWidthType w, ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec3& treeLocalPos, std::mt19937& engine)
{
	switch (type)
	{
	case Voxel::Vegitation::Tree::OAK:
		TreeBuilder::createOakTree(h, w, chunkMap, chunkXZ, treeLocalPos, engine);
		break;
	case Voxel::Vegitation::Tree::BIRCH:
		TreeBuilder::createBirchTree(h, w, chunkMap, chunkXZ, treeLocalPos, engine);
		break;
	case Voxel::Vegitation::Tree::SPRUCE:
		TreeBuilder::createSpruceTree(h, w, chunkMap, chunkXZ, treeLocalPos, engine);
		break;
	case Voxel::Vegitation::Tree::PINE:
		TreeBuilder::createPineTree(h, w, chunkMap, chunkXZ, treeLocalPos, engine);
		break;
	default:
		break;
	}
}

void Voxel::TreeBuilder::createOakTree(ChunkMap * chunkMap, const glm::ivec2 & chunkXZ, const glm::ivec3& treeLocalPos, std::mt19937 & engine)
{
	TreeBuilder::TrunkHeightType trunkHeight;

	std::uniform_int_distribution<int> dist = std::uniform_int_distribution<>(0, 100);

	int hRand = dist(engine);

	if (hRand > 60)
	{
		trunkHeight = TreeBuilder::TrunkHeightType::SMALL;
	}
	else if (hRand <= 60 && hRand > 10)
	{
		trunkHeight = TreeBuilder::TrunkHeightType::MEDIUM;
	}
	else if (hRand <= 10)
	{
		trunkHeight = TreeBuilder::TrunkHeightType::LARGE;
	}

	// Get tree width. 
	TreeBuilder::TrunkWidthType trunkWidth;

	int wRand = dist(engine);

	if (wRand > 90)
	{
		trunkWidth = TreeBuilder::TrunkWidthType::SMALL;
	}
	else if (wRand <= 90 && wRand > 33)
	{
		trunkWidth = TreeBuilder::TrunkWidthType::MEDIUM;
	}
	else if (wRand <= 33)
	{
		trunkWidth = TreeBuilder::TrunkWidthType::LARGE;
		
		// If width is large, promote height to at least medium
		if (trunkHeight == TreeBuilder::TrunkHeightType::SMALL)
		{
			trunkHeight = TreeBuilder::TrunkHeightType::MEDIUM;
		}
	}

	createOakTree(trunkHeight, trunkWidth, chunkMap, chunkXZ, treeLocalPos, engine);
}

void Voxel::TreeBuilder::createOakTree(const TreeBuilder::TrunkHeightType h, const TreeBuilder::TrunkWidthType w, ChunkMap * chunkMap, const glm::ivec2 & chunkXZ, const glm::ivec3& treeLocalPos, std::mt19937& engine)
{
	// get height of trunk
	int trunkHeight = getRandomTreeTrunkHeight(Voxel::Vegitation::Tree::OAK, h, engine);

	if (trunkHeight == 0)
	{
		// Something is wrong. cancel.
		return;
	}

	// Copy local pos as pivot
	auto pivot = treeLocalPos;
	// shift to chunk's position
	pivot.x += (chunkXZ.x * Constant::CHUNK_SECTION_WIDTH);
	pivot.z += (chunkXZ.y * Constant::CHUNK_SECTION_LENGTH);
	
	// Start from bottom 10 to make sure it renders trunk and root even it's on steep mountain
	const int rootHeight = 10;
	int trunkY = pivot.y - rootHeight;

	// Add pivot
	std::vector<glm::ivec3> p = { pivot	};

	// Add layer 1 (p1 ~ p4)
	addPosLayer(p, 1);

	// Get oak wood color
	auto oakWoodColor = Color::colorU3TocolorV3(Color::OAK_WOOD);
	auto colorStep = oakWoodColor * 0.05f;

	oakWoodColor = oakWoodColor - (colorStep * (static_cast<float>(trunkHeight) * 0.5f));
	
	// Add layer 2 (p5 ~ p12)
	addPosLayer(p, 2);
	// Add layer 3 (p13 ~ p20)
	addPosLayer(p, 3);
	// Add layer 4 (p21 ~ p24)
	addPosLayer(p, 4);

	// Poll random.
	int tRand = std::uniform_int_distribution<>(0, 3)(engine);

	if (w == TreeBuilder::TrunkWidthType::SMALL)
	{
		// Add small trunk
		/*
			p4 p3
			p2 p1
		*/
		addOakTrunk(chunkMap, p, oakWoodColor, colorStep, 1, 4, trunkHeight, trunkY);

		// from p1 ~ p12, add blocks 
		for (int i = 1; i <= 12; ++i)
		{
			chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
		}

		// Based on trunk random, add more blocks around.
		if (tRand < 3)
		{
			if (tRand == 0)
			{
				/*
						p9  p10
					p8  p4  p3  p11
					p7  p2  p1  p12
						p6  p5
				*/
				// All 3 type of rand needs p1 ~ p12. So already done.
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

				for (int i = 1; i <= 12; ++i)
				{
					p.at(i).y++;
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
					p.at(i).y--;
				}

				for (int i = 13; i <= 20; i++)
				{
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				}

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

				for (int i = 1; i <= 12; ++i)
				{
					p.at(i).y++;
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
					p.at(i).y++;
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
					p.at(i).y -= 2;
				}

				for (int i = 13; i <= 20; i++)
				{
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				}

				for (int i = 21; i <= 24; i++)
				{
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
					p.at(i).y++;
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
					p.at(i).y--;
				}
			}
		}
		// Else, tRand is 3. No additioanl block around bttom of trunk
				
		// Root time
		/*
					p17 p18
				p23	p9  p10 p24
			p16 p8  p4  p3  p11 p19
			p15 p7  p2  p1  p12 p20
				p22	p6  p5  p21
					p14 p13
		*/
		
		// Current y is the first block of tree trunk. Decrement by 1 to start adding root

		// go 10 block deep
		for (unsigned int i = 1; i <= 24; i++)
		{
			p.at(i).y--;
		}

		for (int i = 0; i < rootHeight; i++)
		{
			for (unsigned int j = 1; j <= 24; j++)
			{
				chunkMap->placeBlockAt(p.at(j), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, false);
				p.at(j).y--;
			}
		}
		
		// Calculate leave center pos. 
		auto leavesCenterPos = glm::ivec3(p.at(1).x, pivot.y + trunkHeight, p.at(1).z);

		addOakLeaves(chunkMap, w, h, leavesCenterPos, engine);

		// Add more blocks around top of trunk, below the leaves
		/*
		int newY = pivot.y + trunkHeight;

		// Reset y to top of the trunk
		for (int i = 5; i <= 12; ++i)
		{
			p.at(i).y = newY;
		}

		// Add additional trunks on top. May hidden by leaves though.
		int count = leavesHeight - 1;
		for (int i = 0; i < count - 1; i++)
		{
			for (int j = 5; j<= 12; ++j)
			{
				chunkMap->placeBlockAt(p.at(j), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				p.at(j).y--;
			}
		}
		*/

		// Small trees can have up to 1 branch and it's 50/50
		int branchRand = std::uniform_int_distribution<>(0, 100)(engine);
		if (branchRand > 50)
		{
			// add branch
			//addOakBranch(chunkMap, p, pivot.y + trunkHeight - leavesHeight, engine);
		}
	}
	else
	{
		// Add layer 5 (p25 ~ p40)
		addPosLayer(p, 5);

		if (w == TreeBuilder::TrunkWidthType::MEDIUM)
		{		
			/*
							p33 p34
						p32	p17 p18 p35
					p31	p23	p9  p10 p24 p36
				p30	p16 p8  p4  p3  p11 p19 p37
				p29	p15 p7  p2  p1  p12 p20 p38
					p28	p22	p6  p5  p21 p39
						p27	p14 p13 p40
							p26 p25
			*/
					
			addOakTrunk(chunkMap, p, oakWoodColor, colorStep, 1, 12, trunkHeight, trunkY);

			for (int i = 13; i <= 24; ++i)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
			}

			if (tRand == 0)
			{
				/*
							p17 p18
						p23	p9  p10 p24
					p16 p8  p4  p3  p11 p19
					p15 p7  p2  p1  p12 p20
						p22	p6  p5  p21
							p14 p13
				*/
				// done
			}
			else
			{
				for (int i = 13; i <= 24; ++i)
				{
					p.at(i).y++;
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
					p.at(i).y--;
				}

				for (int i = 25; i <= 40; ++i)
				{
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				}

				if (tRand == 1)
				{
					// done
				}
				else if (tRand == 2)
				{
					for (int i = 21; i <= 24; ++i)
					{
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
						p.at(i).y -= 2;
					}
				}
			}

			// add root
			for (unsigned int i = 1; i <= 40; i++)
			{
				p.at(i).y--;
			}

			for (int i = 0; i < rootHeight; i++)
			{
				for (unsigned int j = 1; j <= 40; j++)
				{
					chunkMap->placeBlockAt(p.at(j), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, false);
					p.at(j).y--;
				}
			}

			// Calculate leave center pos. 
			auto leavesCenterPos = glm::ivec3(p.at(1).x, pivot.y + trunkHeight, p.at(1).z);

			addOakLeaves(chunkMap, w, h, leavesCenterPos, engine);
		}
		else if (w == TreeBuilder::TrunkWidthType::LARGE)
		{
			addPosLayer(p, 6);

			addOakTrunk(chunkMap, p, oakWoodColor, colorStep, 1, 24, trunkHeight, trunkY);

			for (int i = 25; i <= 60; ++i)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
			}

			if (tRand == 0)
			{
				for (int i = 25; i <= 40; ++i)
				{
					p.at(i).y++;
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
					p.at(i).y--;
				}
			}
			else
			{
				for (int i = 25; i <= 40; ++i)
				{
					p.at(i).y++;
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				}

				p.at(25).y++;
				p.at(26).y++;
				chunkMap->placeBlockAt(p.at(25), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				chunkMap->placeBlockAt(p.at(26), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				p.at(25).y -= 2;
				p.at(26).y -= 2;

				p.at(29).y++;
				p.at(30).y++;
				chunkMap->placeBlockAt(p.at(29), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				chunkMap->placeBlockAt(p.at(30), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				p.at(29).y -= 2;
				p.at(30).y -= 2;

				p.at(33).y++;
				p.at(34).y++;
				chunkMap->placeBlockAt(p.at(33), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				chunkMap->placeBlockAt(p.at(34), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				p.at(33).y -= 2;
				p.at(34).y -= 2;

				p.at(37).y++;
				p.at(38).y++;
				chunkMap->placeBlockAt(p.at(37), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				chunkMap->placeBlockAt(p.at(38), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
				p.at(37).y -= 2;
				p.at(38).y -= 2;

				if (tRand == 1)
				{
					// done
				}
				else if (tRand == 2)
				{
					for (int i = 27; i <= 28; i++)
					{
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
						p.at(i).y -= 2;
					}

					for (int i = 31; i <= 32; i++)
					{
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
						p.at(i).y -= 2;
					}

					for (int i = 35; i <= 36; i++)
					{
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
						p.at(i).y -= 2;
					}

					for (int i = 39; i <= 40; i++)
					{
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, true);
						p.at(i).y -= 2;
					}
				}
			}

			// add root
			for (unsigned int i = 1; i <= 60; i++)
			{
				p.at(i).y--;
			}

			for (int i = 0; i < rootHeight; i++)
			{
				for (unsigned int j = 1; j <= 60; j++)
				{
					chunkMap->placeBlockAt(p.at(j), Block::BLOCK_ID::OAK_WOOD, oakWoodColor, nullptr, false);
					p.at(j).y--;
				}
			}

			// Calculate leave center pos. 
			auto leavesCenterPos = glm::ivec3(p.at(1).x, pivot.y + trunkHeight, p.at(1).z);

			addOakLeaves(chunkMap, w, h, leavesCenterPos, engine);
		}
	}
}

void Voxel::TreeBuilder::createBirchTree(ChunkMap * chunkMap, const glm::ivec2 & chunkXZ, const glm::ivec3 & treeLocalPos, std::mt19937 & engine)
{
	TreeBuilder::TrunkHeightType trunkHeight;

	std::uniform_int_distribution<int> dist = std::uniform_int_distribution<>(0, 100);
	int hRand = dist(engine);
	if (hRand > 90)
	{
		trunkHeight = TreeBuilder::TrunkHeightType::SMALL;
	}
	else if (hRand <= 90 && hRand > 40)
	{
		trunkHeight = TreeBuilder::TrunkHeightType::MEDIUM;
	}
	else if (hRand <= 40)
	{
		trunkHeight = TreeBuilder::TrunkHeightType::LARGE;
	}

	// Get tree width. 
	TreeBuilder::TrunkWidthType trunkWidth;

	int wRand = dist(engine);
	if (wRand > 10)
	{
		trunkWidth = TreeBuilder::TrunkWidthType::SMALL;
	}
	else if (wRand <= 10)
	{
		trunkWidth = TreeBuilder::TrunkWidthType::MEDIUM;
	}

	createBirchTree(trunkHeight, trunkWidth, chunkMap, chunkXZ, treeLocalPos, engine);
}

void Voxel::TreeBuilder::createBirchTree(const TreeBuilder::TrunkHeightType h, const TreeBuilder::TrunkWidthType w, ChunkMap * chunkMap, const glm::ivec2 & chunkXZ, const glm::ivec3 & treeLocalPos, std::mt19937 & engine)
{
	// get height of trunk
	int trunkHeight = getRandomTreeTrunkHeight(Voxel::Vegitation::Tree::BIRCH, h, engine);

	if (trunkHeight == 0)
	{
		// Something is wrong. cancel.
		return;
	}

	// Copy local pos as pivot
	auto pivot = treeLocalPos;
	// shift to chunk's position
	pivot.x += (chunkXZ.x * Constant::CHUNK_SECTION_WIDTH);
	pivot.z += (chunkXZ.y * Constant::CHUNK_SECTION_LENGTH);
	
	// Start from bottom 10 to make sure it renders trunk even it's on steep mountain
	const int rootHeight = 10;
	const int trunkY = pivot.y - rootHeight;

	// Add pivot
	std::vector<glm::ivec3> p = { pivot };

	// Add layer 1 (p1 ~ p4)
	addPosLayer(p, 1);

	// Get birch wood color
	auto birchWoodWhiteColor = Color::colorU3TocolorV3(Color::BIRCH_WOOD_WHITE);
	auto birchWoodBlackColor = Color::colorU3TocolorV3(Color::BIRCH_WOOD_BLACK);
	auto colorStep = birchWoodWhiteColor * 0.025f;

	birchWoodWhiteColor = birchWoodWhiteColor - (colorStep * (static_cast<float>(trunkHeight) * 0.5f));

	// Add layer 2 (p5 ~ p12)
	addPosLayer(p, 2);
	// Add layer 3 (p13 ~ p20)
	addPosLayer(p, 3);
	// Add layer 4 (p21 ~ p24)
	addPosLayer(p, 4);
	
	// Poll random.
	int tRand = std::uniform_int_distribution<>(0, 5)(engine);

	// Calculate leave center pos. 
	auto trunkTopPos = glm::ivec3(p.at(1).x, pivot.y + trunkHeight, p.at(1).z);
	auto trunkMidPos = glm::ivec3(p.at(1).x, pivot.y + (trunkHeight / 2) + 1, p.at(1).z);

	if (w == TreeBuilder::TrunkWidthType::SMALL)
	{
		// Add small trunk
		/*
			p4 p3
			p2 p1
		*/
		addBirchTrunk(chunkMap, p, w, birchWoodWhiteColor, colorStep, birchWoodBlackColor, 1, 4, trunkHeight, trunkY, engine);

		// Based on trunk random, add more blocks around.
		if (tRand == 0)
		{
			/*
					p9  p10
				p8  p4  p3  p11
				p7  p2  p1  p12
					p6  p5
			*/		
			// from p1 ~ p12, add blocks 
			for (int i = 1; i <= 12; ++i)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::BIRCH_WOOD_WHITE, birchWoodWhiteColor, nullptr, true);
			}

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

			for (int i = 1; i <= 12; ++i)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::BIRCH_WOOD_WHITE, birchWoodWhiteColor, nullptr, true);
				p.at(i).y++;
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::BIRCH_WOOD_WHITE, birchWoodWhiteColor, nullptr, true);
				p.at(i).y--;
			}

			for (int i = 13; i <= 20; i++)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::BIRCH_WOOD_WHITE, birchWoodWhiteColor, nullptr, true);
			}

		}
		// Else, tRand is 3 or higher. No additioanl block around bttom of trunk
				
		// Root time
		/*
					p17 p18
				p23	p9  p10 p24
			p16 p8  p4  p3  p11 p19
			p15 p7  p2  p1  p12 p20
				p22	p6  p5  p21
					p14 p13
		*/
		
		// Current y is the first block of tree trunk. Decrement by 1 to start adding root
		for (unsigned int i = 1; i <= 24; i++)
		{
			p.at(i).y--;
		}

		// go 10 block deep
		for (int i = 0; i < rootHeight; i++)
		{
			for (unsigned int j = 1; j <= 24; j++)
			{
				chunkMap->placeBlockAt(p.at(j), Block::BLOCK_ID::BIRCH_WOOD_WHITE, birchWoodWhiteColor, nullptr, false);
				p.at(j).y--;
			}
		}

		addBirchLeaves(chunkMap, w, trunkTopPos, trunkMidPos, engine);

		// Birch tree doens't have additional blocks aroudn top trunk.
	}
	// Birch tree can't have LARGE width type. So it's MEDIUM else than SMALL.
	else
	{
		// Add layer 5 (p25 ~ p40)
		addPosLayer(p, 5);

		addBirchTrunk(chunkMap, p, w, birchWoodWhiteColor, colorStep, birchWoodBlackColor, 1, 12, trunkHeight, trunkY, engine);

		if (tRand == 0)
		{
			/*
						p17 p18
					p23	p9  p10 p24
				p16 p8  p4  p3  p11 p19
				p15 p7  p2  p1  p12 p20
					p22	p6  p5  p21
						p14 p13
			*/

			for (int i = 13; i <= 24; ++i)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::BIRCH_WOOD_WHITE, birchWoodWhiteColor, nullptr, true);
			}

		}
		else
		{
			for (int i = 13; i <= 24; ++i)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::BIRCH_WOOD_WHITE, birchWoodWhiteColor, nullptr, true);
				p.at(i).y++;
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::BIRCH_WOOD_WHITE, birchWoodWhiteColor, nullptr, true);
				p.at(i).y--;
			}

			for (int i = 25; i <= 40; ++i)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::BIRCH_WOOD_WHITE, birchWoodWhiteColor, nullptr, true);
			}
		}

		// add root
		for (unsigned int i = 1; i <= 40; i++)
		{
			p.at(i).y--;
		}

		for (int i = 0; i < rootHeight; i++)
		{
			for (unsigned int j = 1; j <= 40; j++)
			{
				chunkMap->placeBlockAt(p.at(j), Block::BLOCK_ID::BIRCH_WOOD_WHITE, birchWoodWhiteColor, nullptr, false);
				p.at(j).y--;
			}
		}

		addBirchLeaves(chunkMap, w, trunkTopPos, trunkMidPos, engine);

		// Birch tree doens't have additional blocks aroudn top trunk.
	}
}

void Voxel::TreeBuilder::createSpruceTree(ChunkMap * chunkMap, const glm::ivec2 & chunkXZ, const glm::ivec3 & treeLocalPos, std::mt19937 & engine)
{
	TreeBuilder::TrunkHeightType trunkHeight;

	std::uniform_int_distribution<int> dist = std::uniform_int_distribution<>(0, 100);

	int hRand = dist(engine);

	if (hRand > 40)
	{
		trunkHeight = TreeBuilder::TrunkHeightType::MEDIUM;
	}
	else if (hRand <= 40)
	{
		trunkHeight = TreeBuilder::TrunkHeightType::LARGE;
	}

	// Get tree width. 
	TreeBuilder::TrunkWidthType trunkWidth;

	int wRand = dist(engine);

	if (wRand > 50)
	{
		trunkWidth = TreeBuilder::TrunkWidthType::SMALL;
	}
	else if (wRand <= 50)
	{
		trunkWidth = TreeBuilder::TrunkWidthType::MEDIUM;
	}

	createSpruceTree(trunkHeight, trunkWidth, chunkMap, chunkXZ, treeLocalPos, engine);
}

void Voxel::TreeBuilder::createSpruceTree(const TreeBuilder::TrunkHeightType h, const TreeBuilder::TrunkWidthType w, ChunkMap * chunkMap, const glm::ivec2 & chunkXZ, const glm::ivec3 & treeLocalPos, std::mt19937 & engine)
{
	// get height of trunk
	int trunkHeight = getRandomTreeTrunkHeight(Voxel::Vegitation::Tree::SPRUCE, h, engine);

	if (trunkHeight == 0)
	{
		// Something is wrong. cancel.
		return;
	}

	// Copy local pos as pivot
	auto pivot = treeLocalPos;
	// shift to chunk's position
	pivot.x += (chunkXZ.x * Constant::CHUNK_SECTION_WIDTH);
	pivot.z += (chunkXZ.y * Constant::CHUNK_SECTION_LENGTH);

	// Start from bottom 10 to make sure it renders trunk and root even it's on steep mountain
	const int rootHeight = 10;
	const int trunkY = pivot.y - rootHeight;

	// Add pivot
	std::vector<glm::ivec3> p = { pivot };

	// Add layer 1 (p1 ~ p4)
	addPosLayer(p, 1);

	// Get oak wood color
	auto spruceWoodColor = Color::colorU3TocolorV3(Color::SPRUCE_WOOD);
	auto colorStep = spruceWoodColor * 0.05f;

	spruceWoodColor = spruceWoodColor - (colorStep * (static_cast<float>(trunkHeight) * 0.5f));
	
	// Add layer 2 (p5 ~ p12)
	addPosLayer(p, 2);
	// Add layer 3 (p13 ~ p20)
	addPosLayer(p, 3);
	// Add layer 4 (p21 ~ p24)
	addPosLayer(p, 4);
	
	// Poll random.
	int tRand = std::uniform_int_distribution<>(0, 5)(engine); 
	
	if (w == TreeBuilder::TrunkWidthType::SMALL)
	{
		// Add small trunk
		/*
			p4 p3
			p2 p1
		*/
		addSpruceTrunk(chunkMap, p, spruceWoodColor, colorStep, 1, 4, trunkHeight, trunkY);

		// Based on trunk random, add more blocks around.
		if (tRand == 0)
		{
			/*
					p9  p10
				p8  p4  p3  p11
				p7  p2  p1  p12
					p6  p5
			*/		
			// from p1 ~ p12, add blocks 
			for (int i = 1; i <= 12; ++i)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::BIRCH_WOOD_WHITE, spruceWoodColor, nullptr, true);
			}

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

			for (int i = 1; i <= 12; ++i)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::BIRCH_WOOD_WHITE, spruceWoodColor, nullptr, true);
				p.at(i).y++;
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::BIRCH_WOOD_WHITE, spruceWoodColor, nullptr, true);
				p.at(i).y--;
			}

			for (int i = 13; i <= 20; i++)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::BIRCH_WOOD_WHITE, spruceWoodColor, nullptr, true);
			}

		}
		// Else, tRand is 3 or higher. No additioanl block around bttom of trunk
				
		// Root time
		/*
					p17 p18
				p23	p9  p10 p24
			p16 p8  p4  p3  p11 p19
			p15 p7  p2  p1  p12 p20
				p22	p6  p5  p21
					p14 p13
		*/
		
		// Current y is the first block of tree trunk. Decrement by 1 to start adding root
		for (unsigned int i = 1; i <= 24; i++)
		{
			p.at(i).y--;
		}

		// go 10 block deep
		for (int i = 0; i < rootHeight; i++)
		{
			for (unsigned int j = 1; j <= 24; j++)
			{
				chunkMap->placeBlockAt(p.at(j), Block::BLOCK_ID::OAK_WOOD, spruceWoodColor, nullptr, false);
				p.at(j).y--;
			}
		}

		// Calculate leave center pos. 
		auto trunkTopPos = glm::ivec3(p.at(1).x, pivot.y + trunkHeight, p.at(1).z);

		addSpruceLeaves(chunkMap, w, trunkTopPos, trunkHeight, engine);

	}
	// There is no large spruce tree
	else
	{
		addPosLayer(p, 5);

		addSpruceTrunk(chunkMap, p, spruceWoodColor, colorStep, 1, 12, trunkHeight, trunkY);

		if (tRand == 0)
		{
			/*
					p17 p18
				p23	p9  p10 p24
			p16 p8  p4  p3  p11 p19
			p15 p7  p2  p1  p12 p20
				p22	p6  p5  p21
					p14 p13
			*/

			for (int i = 13; i <= 24; ++i)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::SPRUCE_WOOD, spruceWoodColor, nullptr, true);
			}

		}
		else
		{
			for (int i = 13; i <= 24; ++i)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::SPRUCE_WOOD, spruceWoodColor, nullptr, true);
				p.at(i).y++;
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::SPRUCE_WOOD, spruceWoodColor, nullptr, true);
				p.at(i).y--;
			}

			for (int i = 25; i <= 40; ++i)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::SPRUCE_WOOD, spruceWoodColor, nullptr, true);
			}
		}

		// add root
		for (unsigned int i = 1; i <= 40; i++)
		{
			p.at(i).y--;
		}

		for (int i = 0; i < rootHeight; i++)
		{
			for (unsigned int j = 1; j <= 40; j++)
			{
				chunkMap->placeBlockAt(p.at(j), Block::BLOCK_ID::SPRUCE_WOOD, spruceWoodColor, nullptr, false);
				p.at(j).y--;
			}
		}

		// Calculate leave center pos. 
		auto trunkTopPos = glm::ivec3(p.at(1).x, pivot.y + trunkHeight, p.at(1).z);

		addSpruceLeaves(chunkMap, w, trunkTopPos, trunkHeight, engine);
	}
}

void Voxel::TreeBuilder::createPineTree(ChunkMap * chunkMap, const glm::ivec2 & chunkXZ, const glm::ivec3 & treeLocalPos, std::mt19937 & engine)
{
	TreeBuilder::TrunkHeightType trunkHeight;

	std::uniform_int_distribution<int> dist = std::uniform_int_distribution<>(0, 100);

	int hRand = dist(engine);

	if (hRand > 40)
	{
		trunkHeight = TreeBuilder::TrunkHeightType::MEDIUM;
	}
	else if (hRand <= 40)
	{
		trunkHeight = TreeBuilder::TrunkHeightType::LARGE;
	}

	// Get tree width. 
	TreeBuilder::TrunkWidthType trunkWidth;

	int wRand = dist(engine);

	if (wRand > 50)
	{
		trunkWidth = TreeBuilder::TrunkWidthType::SMALL;
	}
	else if (wRand <= 50)
	{
		trunkWidth = TreeBuilder::TrunkWidthType::MEDIUM;
	}

	createPineTree(trunkHeight, trunkWidth, chunkMap, chunkXZ, treeLocalPos, engine);
}

void Voxel::TreeBuilder::createPineTree(const TreeBuilder::TrunkHeightType h, const TreeBuilder::TrunkWidthType w, ChunkMap * chunkMap, const glm::ivec2 & chunkXZ, const glm::ivec3 & treeLocalPos, std::mt19937 & engine)
{
	// get height of trunk
	int trunkHeight = getRandomTreeTrunkHeight(Voxel::Vegitation::Tree::PINE, h, engine);

	if (trunkHeight == 0)
	{
		// Something is wrong. cancel.
		return;
	}

	// Copy local pos as pivot
	auto pivot = treeLocalPos;
	// shift to chunk's position
	pivot.x += (chunkXZ.x * Constant::CHUNK_SECTION_WIDTH);
	pivot.z += (chunkXZ.y * Constant::CHUNK_SECTION_LENGTH);

	// Start from bottom 10 to make sure it renders trunk and root even it's on steep mountain
	const int rootHeight = 10;
	int trunkY = pivot.y - rootHeight;

	// Add pivot
	std::vector<glm::ivec3> p = { pivot };

	// Add layer 1 (p1 ~ p4)
	addPosLayer(p, 1);

	// Get oak wood color
	auto pineWoodColor = Color::colorU3TocolorV3(Color::PINE_WOOD);
	auto colorStep = pineWoodColor * 0.05f;

	pineWoodColor = pineWoodColor - (colorStep * (static_cast<float>(trunkHeight) * 0.5f));

	// Add layer 2 (p5 ~ p12)
	addPosLayer(p, 2);
	// Add layer 3 (p13 ~ p20)
	addPosLayer(p, 3);
	// Add layer 4 (p21 ~ p24)
	addPosLayer(p, 4);

	// Poll random.
	int tRand = std::uniform_int_distribution<>(0, 3)(engine);

	if (w == TreeBuilder::TrunkWidthType::SMALL)
	{
		// Add small trunk
		/*
			p4 p3
			p2 p1
		*/
		addPineTrunk(chunkMap, p, pineWoodColor, colorStep, 1, 4, trunkHeight, trunkY);

		// from p1 ~ p12, add blocks 
		for (int i = 1; i <= 12; ++i)
		{
			chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::PINE_WOOD, pineWoodColor, nullptr, true);
		}

		// Based on trunk random, add more blocks around.
		if (tRand < 3)
		{
			if (tRand == 0)
			{
				/*
						p9  p10
					p8  p4  p3  p11
					p7  p2  p1  p12
						p6  p5
				*/
				// All 3 type of rand needs p1 ~ p12. So already done.
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

				for (int i = 1; i <= 12; ++i)
				{
					p.at(i).y++;
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::PINE_WOOD, pineWoodColor, nullptr, true);
					p.at(i).y--;
				}

				for (int i = 13; i <= 20; i++)
				{
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::PINE_WOOD, pineWoodColor, nullptr, true);
				}

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

				for (int i = 1; i <= 12; ++i)
				{
					p.at(i).y++;
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::PINE_WOOD, pineWoodColor, nullptr, true);
					p.at(i).y++;
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::PINE_WOOD, pineWoodColor, nullptr, true);
					p.at(i).y -= 2;
				}

				for (int i = 13; i <= 20; i++)
				{
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::PINE_WOOD, pineWoodColor, nullptr, true);
				}

				for (int i = 21; i <= 24; i++)
				{
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::PINE_WOOD, pineWoodColor, nullptr, true);
					p.at(i).y++;
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::PINE_WOOD, pineWoodColor, nullptr, true);
					p.at(i).y--;
				}
			}
		}
		// Else, tRand is 3. No additioanl block around bttom of trunk
				
		// Root time
		/*
					p17 p18
				p23	p9  p10 p24
			p16 p8  p4  p3  p11 p19
			p15 p7  p2  p1  p12 p20
				p22	p6  p5  p21
					p14 p13
		*/
		
		// Current y is the first block of tree trunk. Decrement by 1 to start adding root

		// go 10 block deep
		for (unsigned int i = 1; i <= 24; i++)
		{
			p.at(i).y--;
		}

		for (int i = 0; i < rootHeight; i++)
		{
			for (unsigned int j = 1; j <= 24; j++)
			{
				chunkMap->placeBlockAt(p.at(j), Block::BLOCK_ID::OAK_WOOD, pineWoodColor, nullptr, false);
				p.at(j).y--;
			}
		}

		auto trunkTopPos = glm::ivec3(p.at(1).x, pivot.y + trunkHeight, p.at(1).z);

		addPineLeaves(chunkMap, trunkTopPos, trunkHeight, engine);
	}
}

void Voxel::TreeBuilder::addPosLayer(std::vector<glm::ivec3>& p, const int level)
{
	auto pivot = p.front();

	switch (level)
	{
	case 1:
	{
		/*
			p4 p3
			p2 p1
		*/
		p.push_back(glm::ivec3(pivot.x, pivot.y, pivot.z));			// p1
		p.push_back(glm::ivec3(pivot.x + 1, pivot.y, pivot.z));		// p2
		p.push_back(glm::ivec3(pivot.x, pivot.y, pivot.z + 1));		// p3
		p.push_back(glm::ivec3(pivot.x + 1, pivot.y, pivot.z + 1));	// p4
	}
	break;
	case 2:
	{		
		/*
				p9  p10
			p8  p4  p3  p11
			p7  p2  p1  p12
				p6  p5
		*/
		p.push_back(glm::ivec3(p.at(1).x, pivot.y, p.at(1).z - 1));		// p5
		p.push_back(glm::ivec3(p.at(2).x, pivot.y, p.at(2).z - 1));		// p6
		p.push_back(glm::ivec3(p.at(2).x + 1, pivot.y, p.at(2).z));		// p7
		p.push_back(glm::ivec3(p.at(4).x + 1, pivot.y, p.at(4).z));		// p8
		p.push_back(glm::ivec3(p.at(4).x, pivot.y, p.at(4).z + 1));		// p9
		p.push_back(glm::ivec3(p.at(3).x, pivot.y, p.at(3).z + 1));		// p10
		p.push_back(glm::ivec3(p.at(3).x - 1, pivot.y, p.at(3).z));		// p11
		p.push_back(glm::ivec3(p.at(1).x - 1, pivot.y, p.at(1).z));		// p12
	}
	break;
	case 3:
	{		
		/*
					p17 p18
					p9  p10
			p16 p8  p4  p3  p11 p19
			p15 p7  p2  p1  p12 p20
					p6  p5
					p14 p13
		*/
		p.push_back(glm::ivec3(p.at(5).x, pivot.y, p.at(5).z - 1));		// p13
		p.push_back(glm::ivec3(p.at(6).x, pivot.y, p.at(6).z - 1));		// p14
		p.push_back(glm::ivec3(p.at(7).x + 1, pivot.y, p.at(7).z));		// p15
		p.push_back(glm::ivec3(p.at(8).x + 1, pivot.y, p.at(8).z));		// p16
		p.push_back(glm::ivec3(p.at(9).x, pivot.y, p.at(9).z + 1));		// p17
		p.push_back(glm::ivec3(p.at(10).x, pivot.y, p.at(10).z + 1));	// p18
		p.push_back(glm::ivec3(p.at(11).x - 1, pivot.y, p.at(11).z));	// p19
		p.push_back(glm::ivec3(p.at(12).x - 1, pivot.y, p.at(12).z));	// p20
	}
	break;
	case 4:
	{
		/*
					p17 p18
				p23	p9  p10 p24
			p16 p8  p4  p3  p11 p19
			p15 p7  p2  p1  p12 p20
				p22	p6  p5  p21
					p14 p13
		*/
		p.push_back(glm::ivec3(p.at(1).x - 1, pivot.y, p.at(1).z - 1));	// p21
		p.push_back(glm::ivec3(p.at(2).x + 1, pivot.y, p.at(2).z - 1));	// p22
		p.push_back(glm::ivec3(p.at(4).x + 1, pivot.y, p.at(4).z + 1));	// p23
		p.push_back(glm::ivec3(p.at(3).x - 1, pivot.y, p.at(3).z + 1));	// p24
	}
	break;
	case 5:
	{			
		/*
						p33 p34
					p32	p17 p18 p35
				p31	p23	p9  p10 p24 p36
			p30	p16 p8  p4  p3  p11 p19 p37
			p29	p15 p7  p2  p1  p12 p20 p38
				p28	p22	p6  p5  p21 p39
					p27	p14 p13 p40
						p26 p25
		*/
		p.push_back(glm::ivec3(p.at(13).x, pivot.y, p.at(13).z - 1));	// p25
		p.push_back(glm::ivec3(p.at(14).x, pivot.y, p.at(14).z - 1));	// p26
		p.push_back(glm::ivec3(p.at(22).x, pivot.y, p.at(22).z - 1));	// p27
		p.push_back(glm::ivec3(p.at(15).x, pivot.y, p.at(15).z - 1));	// p28
		p.push_back(glm::ivec3(p.at(15).x + 1, pivot.y, p.at(15).z));	// p29
		p.push_back(glm::ivec3(p.at(16).x + 1, pivot.y, p.at(16).z));	// p30
		p.push_back(glm::ivec3(p.at(16).x, pivot.y, p.at(16).z + 1));	// p31
		p.push_back(glm::ivec3(p.at(23).x, pivot.y, p.at(23).z + 1));	// p32
		p.push_back(glm::ivec3(p.at(17).x, pivot.y, p.at(17).z + 1));	// p33
		p.push_back(glm::ivec3(p.at(18).x, pivot.y, p.at(18).z + 1));	// p34
		p.push_back(glm::ivec3(p.at(24).x, pivot.y, p.at(24).z + 1));	// p35
		p.push_back(glm::ivec3(p.at(19).x, pivot.y, p.at(19).z + 1));	// p36
		p.push_back(glm::ivec3(p.at(19).x - 1, pivot.y, p.at(19).z));	// p37
		p.push_back(glm::ivec3(p.at(20).x - 1, pivot.y, p.at(20).z));	// p38
		p.push_back(glm::ivec3(p.at(20).x, pivot.y, p.at(20).z - 1));	// p39
		p.push_back(glm::ivec3(p.at(21).x, pivot.y, p.at(21).z - 1));	// p40
	}
	break;
	case 6:
	{
		/*
							p51 p52
						p50	p33 p34 p53
					p49	p32	p17 p18 p35 p54
				p48	p31	p23	p9  p10 p24 p36 p55
			p47	p30	p16 p8  p4  p3  p11 p19 p37 p56
			p46	p29	p15 p7  p2  p1  p12 p20 p38 p57
				p45	p28	p22	p6  p5  p21 p39 p58
					p44	p27	p14 p13 p40 p59
						p43	p26 p25 p60
							p42 p41
		*/
		p.push_back(glm::ivec3(p.at(25).x, pivot.y, p.at(25).z - 1));	// p41
		p.push_back(glm::ivec3(p.at(26).x, pivot.y, p.at(26).z - 1));	// p42
		p.push_back(glm::ivec3(p.at(27).x, pivot.y, p.at(27).z - 1));	// p43
		p.push_back(glm::ivec3(p.at(28).x, pivot.y, p.at(28).z - 1));	// p44
		p.push_back(glm::ivec3(p.at(29).x, pivot.y, p.at(29).z - 1));	// p45
		p.push_back(glm::ivec3(p.at(29).x + 1, pivot.y, p.at(29).z));	// p46
		p.push_back(glm::ivec3(p.at(30).x + 1, pivot.y, p.at(30).z));	// p47
		p.push_back(glm::ivec3(p.at(30).x, pivot.y, p.at(30).z + 1));	// p48
		p.push_back(glm::ivec3(p.at(31).x, pivot.y, p.at(31).z + 1));	// p49
		p.push_back(glm::ivec3(p.at(32).x, pivot.y, p.at(32).z + 1));	// p50
		p.push_back(glm::ivec3(p.at(33).x, pivot.y, p.at(33).z + 1));	// p51
		p.push_back(glm::ivec3(p.at(34).x, pivot.y, p.at(34).z + 1));	// p52
		p.push_back(glm::ivec3(p.at(35).x, pivot.y, p.at(35).z + 1));	// p53
		p.push_back(glm::ivec3(p.at(36).x, pivot.y, p.at(36).z + 1));	// p54
		p.push_back(glm::ivec3(p.at(37).x, pivot.y, p.at(37).z + 1));	// p55
		p.push_back(glm::ivec3(p.at(37).x - 1, pivot.y, p.at(37).z));	// p56
		p.push_back(glm::ivec3(p.at(38).x - 1, pivot.y, p.at(38).z));	// p57
		p.push_back(glm::ivec3(p.at(38).x, pivot.y, p.at(38).z - 1));	// p58
		p.push_back(glm::ivec3(p.at(39).x, pivot.y, p.at(39).z - 1));	// p59
		p.push_back(glm::ivec3(p.at(40).x, pivot.y, p.at(40).z - 1));	// p60
	}
	break;
	case 0:
	default:
		break;
	}
}

int Voxel::TreeBuilder::getRandomTreeTrunkHeight(const Voxel::Vegitation::Tree & treeType, const TreeBuilder::TrunkHeightType& trunkHeight, std::mt19937 & engine)
{
	int height = 0;

	switch (treeType)
	{
	case Voxel::Vegitation::Tree::OAK:
	{
		std::uniform_int_distribution<int> dist = std::uniform_int_distribution<>(0, 2);

		if (trunkHeight == TreeBuilder::TrunkHeightType::SMALL)
		{
			// note: originally it was 14 ~ 16
			height = dist(engine) + 12;	// 12 ~ 16
		}
		else if (trunkHeight == TreeBuilder::TrunkHeightType::MEDIUM)
		{
			height = dist(engine) + 16;	// 16 ~ 20
		}
		else if (trunkHeight == TreeBuilder::TrunkHeightType::LARGE)
		{
			height = dist(engine) + 20;	// 20 ~ 24
		}
		else if (trunkHeight == TreeBuilder::TrunkHeightType::MEGA)
		{
			height = dist(engine) + 30;	// 30 ~ 34
		}
	}
	break;
	case Voxel::Vegitation::Tree::BIRCH:
	{
		std::uniform_int_distribution<int> dist = std::uniform_int_distribution<>(0, 2);

		if (trunkHeight == TreeBuilder::TrunkHeightType::SMALL)
		{
			height = dist(engine) + 14;
		}
		else if (trunkHeight == TreeBuilder::TrunkHeightType::MEDIUM)
		{
			height = dist(engine) + 18;
		}
		else if (trunkHeight == TreeBuilder::TrunkHeightType::LARGE)
		{
			height = dist(engine) + 22;
		}
		else if (trunkHeight == TreeBuilder::TrunkHeightType::MEGA)
		{
			height = dist(engine) + 30;
		}
	}
	break;
	case Voxel::Vegitation::Tree::SPRUCE:
	{
		std::uniform_int_distribution<int> dist = std::uniform_int_distribution<>(0, 2);

		if (trunkHeight == TreeBuilder::TrunkHeightType::SMALL)
		{
			height = dist(engine) + 17;
		}
		else if (trunkHeight == TreeBuilder::TrunkHeightType::MEDIUM)
		{
			height = dist(engine) + 21;
		}
		else if (trunkHeight == TreeBuilder::TrunkHeightType::LARGE)
		{
			height = dist(engine) + 25;
		}
		else if (trunkHeight == TreeBuilder::TrunkHeightType::MEGA)
		{
			height = dist(engine) + 35;
		}
	}
	break;
	case Voxel::Vegitation::Tree::PINE:
	{
		std::uniform_int_distribution<int> dist = std::uniform_int_distribution<>(0, 2);

		if (trunkHeight == TreeBuilder::TrunkHeightType::SMALL)
		{
			height = dist(engine) + 17;
		}
		else if (trunkHeight == TreeBuilder::TrunkHeightType::MEDIUM)
		{
			height = dist(engine) + 21;
		}
		else if (trunkHeight == TreeBuilder::TrunkHeightType::LARGE)
		{
			height = dist(engine) + 25;
		}
		else if (trunkHeight == TreeBuilder::TrunkHeightType::MEGA)
		{
			height = dist(engine) + 35;
		}
	}
	break;
	default:
		break;
	}

	return height;
}

void Voxel::TreeBuilder::getRandomLeavesSize(const Voxel::Vegitation::Tree & treeType, const TreeBuilder::TrunkWidthType& trunkWidthType, int & width, int & height, int & length, std::mt19937 & engine)
{
	// init
	width = 0;
	height = 0;
	length = 0;
	
	switch (treeType)
	{
	case Voxel::Vegitation::Tree::OAK:
	{
		switch (trunkWidthType)
		{
		case TreeBuilder::TrunkWidthType::SMALL:
		{
			auto dist = std::uniform_int_distribution<>(6, 8);
			auto hDist = std::uniform_int_distribution<>(3, 4);

			width = dist(engine);
			height = hDist(engine);
			length = dist(engine);
		}
		break;
		case TreeBuilder::TrunkWidthType::MEDIUM:
		{
			auto dist = std::uniform_int_distribution<>(7, 9);
			auto hDist = std::uniform_int_distribution<>(4, 5);

			width = dist(engine);
			height = hDist(engine);
			length = dist(engine);
		}
		break;
		case TreeBuilder::TrunkWidthType::LARGE:
		{
			auto dist = std::uniform_int_distribution<>(8, 10);
			auto hDist = std::uniform_int_distribution<>(5, 6);

			width = dist(engine);
			height = hDist(engine);
			length = dist(engine);
		}
		break;
		default:
			break;
		}
	}
	break;
	case Voxel::Vegitation::Tree::BIRCH:
	{
		switch (trunkWidthType)
		{
		case TreeBuilder::TrunkWidthType::SMALL:
		{
			auto dist = std::uniform_int_distribution<>(0, 2);

			// Get leaves size
			width = dist(engine) + 2;
			height = ((dist(engine) <= 1) ? 4 : 5);
			length = dist(engine) + 2;
		}
		break;
		case TreeBuilder::TrunkWidthType::MEDIUM:
		{
			auto dist = std::uniform_int_distribution<>(0, 2);

			width = dist(engine) + 3;
			height = ((dist(engine) <= 1) ? 5 : 6);
			length = dist(engine) + 3;
		}
		break;
		case TreeBuilder::TrunkWidthType::LARGE:
		default:
			break;
		}
	}
	break;
	default:
		break;
	}
}

void Voxel::TreeBuilder::addOakTrunk(ChunkMap * map, std::vector<glm::ivec3>& p, glm::vec3 color, const glm::vec3& colorStep, const int pStart, const int pEnd, const int trunkHeight, const int startY)
{
	int size = trunkHeight + 10;
	
	auto indexMid = 10 + (trunkHeight / 2);

	int originalY = p.at(0).y;

	for (int i = pStart; i <= pEnd; ++i)
	{
		p.at(i).y = startY;
	}

	for (int i = 0; i < size; i++)
	{
		if (i >= 10)
		{
			if (i <= indexMid)
			{
				color += colorStep;
			}
			else
			{
				color -= colorStep;
			}

			color = glm::clamp(color, 0.0f, 1.0f);
		}

		for (int i = pStart; i <= pEnd; i++)
		{
			map->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, color, nullptr, true);

			p.at(i).y++;
		}
	}
	
	for (int i = pStart; i <= pEnd; ++i)
	{
		p.at(i).y = originalY;
	}
}

void Voxel::TreeBuilder::addOakLeaves(ChunkMap * map, const TreeBuilder::TrunkWidthType widthType, const TreeBuilder::TrunkHeightType heightType, const glm::ivec3& pos, std::mt19937& engine)
{
	// Add oak leaves

	float randAngle0 = std::uniform_real_distribution<float>(10.0f, 80.0f)(engine);
	float randAngle1 = std::uniform_real_distribution<float>(100.0f, 170.0f)(engine);
	float randAngle2 = std::uniform_real_distribution<float>(190.0f, 260.0f)(engine);
	float randAngle3 = std::uniform_real_distribution<float>(280.0f, 350.0f)(engine);

	glm::ivec3 leavesPos = pos;

	switch (heightType)
	{
	case TreeBuilder::TrunkHeightType::MEDIUM:
		leavesPos.y -= 3;
		break;
	case TreeBuilder::TrunkHeightType::LARGE:
		leavesPos.y -= 5;
		break;
	case TreeBuilder::TrunkHeightType::SMALL:
	default:
		leavesPos.y -= 1;
		break;
	}

	auto yDist = std::uniform_int_distribution<>(-3, 2);

	int width = 0;
	int height = 0;
	int length = 0;
	
	{
		glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(randAngle0), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 transMat = glm::translate(glm::mat4(1.0f), glm::vec3(std::uniform_int_distribution<>(2, 4)(engine), 0, 0));

		glm::vec4 posF = glm::vec4(leavesPos, 1.0f);
		glm::vec4 shift = rotMat * transMat * glm::vec4(1.0f);
		posF.x += shift.x;
		posF.z += shift.z;
		posF.y += yDist(engine);

		getRandomLeavesSize(Voxel::Vegitation::Tree::OAK, widthType, width, height, length, engine);
		addOakLeaf(map, width, height, length, glm::ivec3(posF), engine);
	}

	{
		glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(randAngle1), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 transMat = glm::translate(glm::mat4(1.0f), glm::vec3(std::uniform_int_distribution<>(2, 4)(engine), 0, 0));

		glm::vec4 posF = glm::vec4(leavesPos, 1.0f);
		glm::vec4 shift = rotMat * transMat * glm::vec4(1.0f);
		posF.x += shift.x;
		posF.z += shift.z;
		posF.y += yDist(engine);

		getRandomLeavesSize(Voxel::Vegitation::Tree::OAK, widthType, width, height, length, engine);
		addOakLeaf(map, width, height, length, glm::ivec3(posF), engine);
	}

	{
		glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(randAngle2), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 transMat = glm::translate(glm::mat4(1.0f), glm::vec3(std::uniform_int_distribution<>(2, 4)(engine), 0, 0));

		glm::vec4 posF = glm::vec4(leavesPos, 1.0f);
		glm::vec4 shift = rotMat * transMat * glm::vec4(1.0f);
		posF.x += shift.x;
		posF.z += shift.z;
		posF.y += yDist(engine);

		getRandomLeavesSize(Voxel::Vegitation::Tree::OAK, widthType, width, height, length, engine);
		addOakLeaf(map, width, height, length, glm::ivec3(posF), engine);
	}

	{
		glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(randAngle3), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 transMat = glm::translate(glm::mat4(1.0f), glm::vec3(std::uniform_int_distribution<>(2, 4)(engine), 0, 0));

		glm::vec4 posF = glm::vec4(leavesPos, 1.0f);
		glm::vec4 shift = rotMat * transMat * glm::vec4(1.0f);
		posF.x += shift.x;
		posF.z += shift.z;
		posF.y += yDist(engine);

		getRandomLeavesSize(Voxel::Vegitation::Tree::OAK, widthType, width, height, length, engine);
		addOakLeaf(map, width, height, length, glm::ivec3(posF), engine);
	}

	getRandomLeavesSize(Voxel::Vegitation::Tree::OAK, widthType, width, height, length, engine);
	addOakLeaf(map, width + 1, height + 1, length + 1, pos, engine);

	/*
	// Oak tree usually has round shape of leaves around the trunk.
	// Oak tree has main leaves and small sub leaves around.
	// Sometimes branches have their own leaves too.

	auto dist = std::uniform_int_distribution<>(0, 100);

	int mlRand = dist(engine);

	auto l1 = pos;

	if (mlRand < 50)
	{
		addOakLeaf(map, w, h, l, l1, engine);
	}
	else
	{
		l1.x -= 1;
		l1.y -= 2;
		addOakLeaf(map, w, h / 3 * 2, l, l1, engine);

		l1.x += 1;
		
		l1.z += std::uniform_int_distribution<>(3, 4)(engine) * ((std::uniform_int_distribution<>(0, 1)(engine)) ? -1 : 1);
		l1.y += 3;

		addOakLeaf(map, w, h / 3 * 2, l, l1, engine);
	}

	// Add additional leaves near main leaves
	dist = std::uniform_int_distribution<>(1, 3);
	int totalSideLeaves = dist(engine);

	for (int i = 0; i < totalSideLeaves; i++)
	{
		int sideLeavesWidth = std::uniform_int_distribution<>(3, 4)(engine);
		int sideLeavesHeight = std::uniform_int_distribution<>(2, 3)(engine);
		int sideLeavesLength = std::uniform_int_distribution<>(3, 4)(engine);


		int x = 0;
		int y = 0;
		int z = 0;

		glm::ivec3 sidePos(0);

		if (mlRand)
		{
			x = (std::uniform_int_distribution<>(w / 2, (w / 4) * 3)(engine)) * ((std::uniform_int_distribution<>(0, 1)(engine)) ? -1 : 1);
			y = (std::uniform_int_distribution<>(0, h / 2)(engine));
			z = (std::uniform_int_distribution<>(l / 2, (l / 4) * 3)(engine)) * ((std::uniform_int_distribution<>(0, 1)(engine)) ? -1 : 1);

			sidePos = glm::ivec3(x, y, z) + l1;
		}
		else
		{
			x = (std::uniform_int_distribution<>((w / 4), w / 2)(engine)) * ((std::uniform_int_distribution<>(0, 1)(engine)) ? -1 : 1);
			y = (std::uniform_int_distribution<>(0, h / 2)(engine));
			z = (std::uniform_int_distribution<>(l / 2, (l / 4) * 3)(engine)) * ((std::uniform_int_distribution<>(0, 1)(engine)) ? -1 : 1);
			sidePos = glm::ivec3(x, y, z) + l1;
		}

		addOakLeaf(map, sideLeavesWidth, sideLeavesHeight, sideLeavesLength, sidePos, engine);
	}
	*/
}

void Voxel::TreeBuilder::addOakLeaf(ChunkMap * map, const int w, const int h, const int l, const glm::ivec3 & pos, std::mt19937& engine)
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

	auto dist100 = std::uniform_int_distribution<>(0, 100);
	int yrand = dist100(engine);
	glm::ivec3 leaveOffset = glm::ivec3(0);

	if (yrand < 40)
	{
		skew = true;

		int xzRand = dist100(engine);
		int dirRand = dist100(engine);

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

	if (dist100(engine) < 50)
	{
		leaveColor += (leaveColor * (((std::uniform_int_distribution<>(0, 1)(engine)) ? -1.0f : 1.0f) * (std::uniform_real_distribution<float>(0.0f, 0.05f))(engine)));
	}

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

void Voxel::TreeBuilder::addOakBranch(ChunkMap * map, std::vector<glm::ivec3>& p, const int branchBaseY, std::mt19937& engine)
{
	// Get total branches
	int totalBranches = std::uniform_int_distribution<>(1, 4)(engine);

	/*
				p17 p18
				p9  p10 
		p16 p8  p4  p3  p11 p19
		p15 p7  p2  p1  p12 p20
				p6  p5
				p14 p13
	*/
	if (totalBranches)
	{
		// one branch
		auto b1 = glm::ivec3(0);
		auto b2 = glm::ivec3(0);
			
		// Get random direction of branch
		int randDir = std::uniform_int_distribution<>(0, 3)(engine);

		if (randDir == 0)
		{
			b1 = p.at(11);
			b2 = p.at(12);
		}
		else if (randDir == 1)
		{
			b1 = p.at(9);
			b2 = p.at(10);
		}
		else if (randDir == 2)
		{
			b1 = p.at(7);
			b2 = p.at(8);
		}
		else
		{
			b1 = p.at(5);
			b2 = p.at(6);
		}

		b1.y = branchBaseY;
		b2.y = branchBaseY;

		// Add branch blocks
		for (int i = 0; i < 4; i++)
		{
			map->placeBlockAt(b1, Block::BLOCK_ID::OAK_WOOD, Color::OAK_WOOD, nullptr, true);
			map->placeBlockAt(b2, Block::BLOCK_ID::OAK_WOOD, Color::OAK_WOOD, nullptr, true);

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

		// Get branch leave size
		auto dist = std::uniform_int_distribution<>(0, 1);
		int branchLeaveWidth = dist(engine) + 2;
		int branchLeavesHeight = dist(engine) + 1;
		int branchLeavesLength = dist(engine) + 2;

		b1.y += (branchLeavesHeight / 2);
			
		//addOakLeave(map, branchLeaveWidth, branchLeavesHeight, branchLeavesLength, b1);
	}
	else if (totalBranches == 2)
	{
	}
}

void Voxel::TreeBuilder::addBirchTrunk(ChunkMap * map, std::vector<glm::ivec3>& p, const TreeBuilder::TrunkWidthType widthType, glm::vec3 trunkColor, const glm::vec3 & colorStep, const glm::vec3 & markColor, const int pStart, const int pEnd, const int trunkHeight, const int startY, std::mt19937& engine)
{
	int size = trunkHeight + 10;
	int sizeHalf = size / 2;

	auto indexMid = 10 + (trunkHeight / 2);

	int originalY = p.at(0).y;

	for (int i = pStart; i <= pEnd; ++i)
	{
		p.at(i).y = startY;
	}

	auto dist = std::uniform_int_distribution<>(3, 4);

	int markIndex = 0;

	auto markDist = std::uniform_int_distribution<>(0, 100);

	int prevDirMod = 0;

	for (int i = 0; i < size; i++)
	{
		if (i >= 10)
		{
			if (i <= indexMid)
			{
				trunkColor += colorStep;
			}
			else
			{
				trunkColor -= colorStep;
			}

			trunkColor = glm::clamp(trunkColor, 0.0f, 1.0f);
		}

		for (int j = pStart; j <= pEnd; j++)
		{
			map->placeBlockAt(p.at(j), Block::BLOCK_ID::OAK_WOOD, trunkColor, nullptr, true);
		}

		if (i == markIndex)
		{
			int sizeRand = markDist(engine);

			glm::vec3 m1;
			glm::vec3 m2;

			if (widthType == TreeBuilder::TrunkWidthType::SMALL)
			{
				int dirRand = markDist(engine);

				if (dirRand <= 25)
				{
					m1 = p.at(1);
					m2 = p.at(2);
				}
				else if (dirRand >= 25 && dirRand < 50)
				{
					m1 = p.at(2);
					m2 = p.at(3);
				}
				else if (dirRand >= 50 && dirRand < 75)
				{
					m1 = p.at(3);
					m2 = p.at(4);
				}
				else if (dirRand >= 75)
				{
					m1 = p.at(4);
					m2 = p.at(1);
				}
				else
				{
					m1 = p.at(1);
					m2 = p.at(2);
				}

				map->placeBlockAt(m1, Block::BLOCK_ID::OAK_WOOD, markColor, nullptr, true);
				if (sizeRand > 40)
				{
					map->placeBlockAt(m2, Block::BLOCK_ID::OAK_WOOD, markColor, nullptr, true);
				}

				markIndex += dist(engine);
			}
			else
			{
				int countRand = std::uniform_int_distribution<>(0, 100)(engine);
				int count = 1;

				if (countRand < 50)
				{
					count = 2;
				}

				for (int i = 0; i < count; i++)
				{
					int dirRand = markDist(engine);

					int mod = dirRand % 8;

					if (prevDirMod == mod)
					{
						if (std::uniform_int_distribution<>(0, 100)(engine) < 70)
						{
							mod += 4;
							if (mod > 7)
							{
								mod -= 7;
							}
						}
					}

					prevDirMod = mod;

					switch (mod)
					{
					case 1:
						m1 = p.at(6);
						m2 = p.at(7);
						break;
					case 2:
						m1 = p.at(7);
						m2 = p.at(8);
						break;
					case 3:
						m1 = p.at(8);
						m2 = p.at(9);
						break;
					case 4:
						m1 = p.at(9);
						m2 = p.at(10);
						break;
					case 5:
						m1 = p.at(10);
						m2 = p.at(11);
						break;
					case 6:
						m1 = p.at(11);
						m2 = p.at(12);
						break;
					case 7:
						m1 = p.at(12);
						m2 = p.at(5);
						break;
					case 0:
						m1 = p.at(5);
						m2 = p.at(6);
					default:
						break;
					}

					map->placeBlockAt(m1, Block::BLOCK_ID::OAK_WOOD, markColor, nullptr, true);
					map->placeBlockAt(m2, Block::BLOCK_ID::OAK_WOOD, markColor, nullptr, true);
				}

				if (count == 1)
				{
					markIndex += dist(engine) - 1;
				}
				else
				{
					markIndex += dist(engine);
				}
			}
		}

		for (int j = pStart; j <= pEnd; j++)
		{
			p.at(j).y++;
		}
	}

	for (int i = pStart; i <= pEnd; ++i)
	{
		p.at(i).y = originalY;
	}
}

void Voxel::TreeBuilder::addBirchLeaves(ChunkMap * map, const TreeBuilder::TrunkWidthType widthType, const glm::ivec3 & trunkTopPos, const glm::ivec3& trunkMidPos, std::mt19937 & engine)
{
	// Add birch leaves
	// Birch leaves doesn't spread leaves around a lot. 
	// Leaves are close to tue trunk and starts to grow on top to middle point of the trunk
	// Leaves aren't packed too much, easy to see through the trunk.
	// There are no main leaves with birch tree, but multiple small leaves

	// Always add one at top
	auto topLeaveCenterPos = trunkTopPos;

	// get random top leave size.
	int width, height, length;
	getRandomLeavesSize(Voxel::Vegitation::Tree::BIRCH, widthType, width, height, length, engine);

	topLeaveCenterPos.y += (height / 2);
	addBirchLeaf(map, width, height, length, topLeaveCenterPos, engine);

	// from the mid point of tree, randomly add leveas around birch tree
	// Advance y by 2 every time until it reaches top.
	glm::ivec3 curPos = trunkMidPos;

	auto angleDist = std::uniform_real_distribution<float>(0, 359);

	while(curPos.y < trunkTopPos.y)
	{
		// Randomly generate angle
		float randAngle = angleDist(engine);

		// Translate 
		glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(randAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 transMat = glm::mat4(1.0f);

		getRandomLeavesSize(Voxel::Vegitation::Tree::BIRCH, widthType, width, height, length, engine);

		int randOffset = std::uniform_int_distribution<>(2, width)(engine);

		transMat = glm::translate(glm::mat4(1.0f), glm::vec3(randOffset - 2, 0, 0));

		glm::vec4 posF = glm::vec4(curPos, 1.0f);
		glm::vec4 shift = rotMat * transMat * glm::vec4(1.0f);
		posF.x += shift.x;
		posF.z += shift.z;

		if ((std::uniform_int_distribution<>(0, 100)(engine)) < 50)
		{
			addBirchLeaf(map, width, height, length, glm::ivec3(posF), engine);
		}
		else
		{
			addBirchLeaf(map, length, height, width, glm::ivec3(posF), engine);
		}

		if ((std::uniform_int_distribution<>(0, 100)(engine)) < 6)
		{
			continue;
		}

		// advance by 2
		curPos.y += std::uniform_int_distribution<>(1, 2)(engine);
	}
}

void Voxel::TreeBuilder::addBirchLeaf(ChunkMap * map, const int w, const int h, const int l, const glm::ivec3 & pos, std::mt19937 & engine)
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
	
	auto leaveColor = Color::colorU3TocolorV3(Color::BIRCH_LEAVES);
	auto leaveColorStep = leaveColor * 0.05f;

	if (std::uniform_int_distribution<>(0, 100)(engine) < 50)
	{
		leaveColor += (leaveColor * (((std::uniform_int_distribution<>(0, 1)(engine)) ? -1.0f : 1.0f) * (std::uniform_real_distribution<float>(0.0f, 0.05f))(engine)));
	}

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

					map->placeBlockAt(lp, Block::BLOCK_ID::BIRCH_LEAVES, leaveColor, nullptr, false);
				}
			}
		}

		leaveColor += leaveColorStep;
		leaveColor = glm::clamp(leaveColor, 0.0f, 1.0f);
	}
}

void Voxel::TreeBuilder::addSpruceTrunk(ChunkMap * map, std::vector<glm::ivec3>& p, glm::vec3 color, const glm::vec3 & colorStep, const int pStart, const int pEnd, const int trunkHeight, const int startY)
{
	int size = trunkHeight + 10;

	auto indexMid = 10 + (trunkHeight / 2);

	int originalY = p.at(0).y;

	for (int i = pStart; i <= pEnd; ++i)
	{
		p.at(i).y = startY;
	}

	for (int i = 0; i < size; i++)
	{
		if (i >= 10)
		{
			if (i <= indexMid)
			{
				color += colorStep;
			}
			else
			{
				color -= colorStep;
			}

			color = glm::clamp(color, 0.0f, 1.0f);
		}

		for (int i = pStart; i <= pEnd; i++)
		{
			map->placeBlockAt(p.at(i), Block::BLOCK_ID::SPRUCE_WOOD, color, nullptr, true);

			p.at(i).y++;
		}
	}

	for (int i = pStart; i <= pEnd; ++i)
	{
		p.at(i).y = originalY;
	}
}

void Voxel::TreeBuilder::addSpruceLeaves(ChunkMap * map, const TreeBuilder::TrunkWidthType w, const glm::ivec3 & trunkTopPos, const int trunkHeight, std::mt19937 & engine)
{
	// Add spruce leaves
	// From the bottom-mid point of the trunk, add layers of leaves that spread outs 

	glm::ivec3 curPos = trunkTopPos;
	curPos.y -= ((trunkHeight * 3) / 4) - 2;

	std::vector<glm::ivec3> leavePositions;
	leavePositions.push_back(curPos);

	addPosLayer(leavePositions, 1);

	bool diagonal = false;

	int level = 0;

	glm::vec3 leaveColor = Color::colorU3TocolorV3(Color::SPRUCE_LEAVES);
	glm::vec3 leaveColorMix = leaveColor + leaveColor * 0.3f;

	const int limit = trunkTopPos.y - 1;

	while (leavePositions.at(0).y < limit)
	{
		if (diagonal)
		{
			addSpruceLeaf(map, w, leavePositions.at(1), leaveColorMix, 4, level, engine);
			addSpruceLeaf(map, w, leavePositions.at(3), leaveColorMix, 5, level, engine);
			addSpruceLeaf(map, w, leavePositions.at(4), leaveColorMix, 6, level, engine);
			addSpruceLeaf(map, w, leavePositions.at(2), leaveColorMix, 7, level, engine);

			level++;
		}
		else
		{
			addSpruceLeaf(map, w, leavePositions.at(1), leaveColor, 0, level, engine);
			addSpruceLeaf(map, w, leavePositions.at(3), leaveColor, 1, level, engine);
			addSpruceLeaf(map, w, leavePositions.at(4), leaveColor, 2, level, engine);
			addSpruceLeaf(map, w, leavePositions.at(2), leaveColor, 3, level, engine);
		}

		for (auto& pos : leavePositions)
		{
			pos.y += std::uniform_int_distribution<>(2, 3)(engine);
		}

		diagonal = !diagonal;
	}

	for (auto& pos : leavePositions)
	{
		pos.y = trunkTopPos.y - 1;
	}

	level++;

	addSpruceLeaf(map, w, leavePositions.at(1), leaveColor, 0, level, engine);
	addSpruceLeaf(map, w, leavePositions.at(3), leaveColor, 1, level, engine);
	addSpruceLeaf(map, w, leavePositions.at(4), leaveColor, 2, level, engine);
	addSpruceLeaf(map, w, leavePositions.at(2), leaveColor, 3, level, engine);

	for (auto& pos : leavePositions)
	{
		pos.y = trunkTopPos.y;
	}


	// add top leaves

	auto topColor = leaveColor;

	for (int i = 0; i < 3; i++)
	{
		for (auto& pos : leavePositions)
		{
			map->placeBlockAt(pos, Block::BLOCK_ID::SPRUCE_LEAVES, topColor, nullptr, false);
			pos.y++;
		}
		topColor += (topColor * 0.05f);
	}

	addPosLayer(leavePositions, 2);

	for (auto& pos : leavePositions)
	{
		pos.y = trunkTopPos.y;
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 5; j <= 12; j++)
		{
			map->placeBlockAt(leavePositions.at(j), Block::BLOCK_ID::SPRUCE_LEAVES, leaveColor, nullptr, true);
			leavePositions.at(j).y--;
		}
		leaveColor -= (leaveColor * 0.05f);
	}
}

void Voxel::TreeBuilder::addSpruceLeaf(ChunkMap * map, const TreeBuilder::TrunkWidthType w, const glm::ivec3 & leavePos, const glm::vec3& color, const int dir, const int level, std::mt19937 & engine)
{		
	/*
			dir

			  +

		6	  2		5

	+	3	p4 p3	1	-
			p2 p1

		7	  0		4

			  -
	*/

	int width = 0;
	int length = 0;

	glm::ivec3 offset(0);

	float aa = 0.0f;
	float cc = 0.0f;

	float aacc = 0.0f;

	int xStart = 0;
	int zStart = 0;

	int xEnd = 0;
	int zEnd = 0;

	glm::ivec3 curPos = leavePos;

	int repeat = 0;

	if (level < 1)
	{
		repeat = 4;
	}
	else if (level >= 1 && level < 2)
	{
		repeat = 3;
	}
	else if (level >= 2 && level < 3)
	{
		repeat = 2;
	}
	else
	{
		repeat = 1;
	}

	int leaveSize = ((w == TreeBuilder::TrunkWidthType::SMALL) ? 2 : 3);

	for (int i = 0; i < repeat; i++)
	{
		switch (dir)
		{
		case 0:
		case 2:
		{
			width = leaveSize;
			length = std::uniform_int_distribution<>(2, 3)(engine);
		}
			break;
		case 1:
		case 3:
		{
			width = std::uniform_int_distribution<>(2, 3)(engine);
			length = leaveSize;
		}
			break;
		case 4:
		case 5:
		case 6:
		case 7:
		{
			width = leaveSize;
			length = leaveSize;
		}
			break;
		default:
			return;
		}

		offset.y = -1;

		switch (dir)
		{
		case 0:
			offset.x = 0;
			offset.z = -leaveSize;
			break;
		case 2:
			offset.x = 0;
			offset.z = leaveSize;
			break;
		case 1:
			offset.x = -leaveSize;
			offset.z = 0;
			break;
		case 3:
			offset.x = leaveSize;
			offset.z = 0;
			break;
		case 4:
		{
			offset.x = -leaveSize;
			offset.z = -leaveSize;
		}
			break;
		case 5:
		{
			offset.x = leaveSize;
			offset.z = -leaveSize;
		}
			break;
		case 6:
		{
			offset.x = leaveSize;
			offset.z = leaveSize;
		}
			break;
		case 7:
		{
			offset.x = -leaveSize;
			offset.z = leaveSize;
		}
			break;
		default:
			return;
		}

		aa = static_cast<float>(width * width);
		cc = static_cast<float>(length * length);

		aacc = aa * cc;

		xStart = -width;
		zStart = -length;

		xEnd = width;
		zEnd = length;

		for (int x = xStart; x <= xEnd; x++)
		{
			float xf = static_cast<float>(x) - 0.5f;
			float xx = xf * xf;

			for (int z = zStart; z <= zEnd; z++)
			{
				float zf = static_cast<float>(z) - 0.5f;
				float zz = zf * zf;

				auto lp = curPos + glm::ivec3(x, 0, z);

				float val = (xx * cc) + (zz * aa);
				if (val <= aacc)
				{
					map->placeBlockAt(lp, Block::BLOCK_ID::SPRUCE_LEAVES, color, nullptr, false);
				}
			}
		}

		curPos += offset;

		if (w == TreeBuilder::TrunkWidthType::SMALL)
		{
			if (i >= 1)
			{
				if (leaveSize > 2)
				{
					leaveSize--;
				}
			}
		}
		else
		{
			if (leaveSize > 2)
			{
				leaveSize--;
			}
		}
	}
}

void Voxel::TreeBuilder::addPineTrunk(ChunkMap * map, std::vector<glm::ivec3>& p, glm::vec3 color, const glm::vec3 & colorStep, const int pStart, const int pEnd, const int trunkHeight, const int startY)
{
	int size = trunkHeight + 10;

	auto indexMid = 10 + (trunkHeight / 2);

	int originalY = p.at(0).y;

	for (int i = pStart; i <= pEnd; ++i)
	{
		p.at(i).y = startY;
	}

	for (int i = 0; i < size; i++)
	{
		if (i >= 10)
		{
			if (i <= indexMid)
			{
				color += colorStep;
			}
			else
			{
				color -= colorStep;
			}

			color = glm::clamp(color, 0.0f, 1.0f);
		}

		for (int i = pStart; i <= pEnd; i++)
		{
			map->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, color, nullptr, true);

			p.at(i).y++;
		}
	}

	for (int i = pStart; i <= pEnd; ++i)
	{
		p.at(i).y = originalY;
	}
}

void Voxel::TreeBuilder::addPineLeaves(ChunkMap * map, const glm::ivec3& trunkTopPos, const int trunkHeight, std::mt19937 & engine)
{
	glm::ivec3 curPos = trunkTopPos;
	curPos.y -= ((trunkHeight * 3) / 4);

	// 0 = large, 1 = mid, 2 = small
	int type = 0;	

	int width = 0;
	int length = 0;

	glm::vec3 leavesColor = Color::colorU3TocolorV3(Color::PINE_LEAVES);

	int level = 0;

	int levelOffset = 0;

	while (curPos.y <= trunkTopPos.y)
	{
		switch (type)
		{
		case 0:
			width = std::uniform_int_distribution<>(6, 7)(engine) - levelOffset;
			length = std::uniform_int_distribution<>(6, 7)(engine) - levelOffset;
			break;
		case 1:
			width = std::uniform_int_distribution<>(3, 4)(engine) - (levelOffset / 2);
			length = std::uniform_int_distribution<>(3, 4)(engine) - (levelOffset / 2);
			break;
		case 2:
			width = 2;
			length = 2;
			break;
		default:
			return;
		}

		addPineLeaf(map, width, length, curPos, leavesColor, engine);

		type++;

		if (type == 3)
		{
			type = 0;
			levelOffset++;
		}

		curPos.y++;
	}

	auto topColor = leavesColor;

	std::vector<glm::ivec3> leavePositions;
	leavePositions.push_back(trunkTopPos);

	addPosLayer(leavePositions, 1);

	for (int i = 0; i < 3; i++)
	{
		for (auto& pos : leavePositions)
		{
			map->placeBlockAt(pos, Block::BLOCK_ID::PINE_LEAVES, topColor, nullptr, false);
			pos.y++;
		}
		topColor += (topColor * 0.05f);
	}
}

void Voxel::TreeBuilder::addPineLeaf(ChunkMap * map, const int width, const int length, const glm::ivec3& leavesPos, const glm::vec3& color, std::mt19937& engine)
{
	glm::ivec3 offset(0);

	float aa = static_cast<float>(width * width);
	float cc = static_cast<float>(length * length);

	float aacc = aa * cc;

	int xStart = -width;
	int zStart = -length;

	int xEnd = width;
	int zEnd = length;

	for (int x = xStart; x <= xEnd; x++)
	{
		float xf = static_cast<float>(x) - 0.5f;
		float xx = xf * xf;

		for (int z = zStart; z <= zEnd; z++)
		{
			float zf = static_cast<float>(z) - 0.5f;
			float zz = zf * zf;

			auto lp = leavesPos + glm::ivec3(x, ((std::uniform_int_distribution<>(0, 100)(engine) < 30) ? 0 : -1), z);

			float val = (xx * cc) + (zz * aa);

			if (val <= aacc)
			{
				map->placeBlockAt(lp, Block::BLOCK_ID::SPRUCE_LEAVES, color, nullptr, false);
			}
		}
	}
}
