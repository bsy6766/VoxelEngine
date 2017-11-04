#include "TreeBuilder.h"
#include <glm\glm.hpp>
#include <ChunkUtil.h>
#include <ChunkMap.h>
#include <Color.h>
#include <Utility.h>
#include <Random.h>

using namespace Voxel;

void Voxel::TreeBuilder::createTree(const TreeBuilder::TreeType type, ChunkMap * chunkMap, const glm::ivec2 & chunkXZ, const glm::ivec3& treeLocalPos, std::mt19937 & engine)
{
	switch (type)
	{
	case TreeBuilder::TreeType::OAK:
		TreeBuilder::createOakTree(chunkMap, chunkXZ, treeLocalPos, engine);
		break;
	case TreeBuilder::TreeType::BIRCH:
		TreeBuilder::createBirchTree(chunkMap, chunkXZ, treeLocalPos, engine);
		break;
	default:
		break;
	}
}

void Voxel::TreeBuilder::createTree(const TreeBuilder::TreeType type, const TreeBuilder::TrunkHeightType h, const TreeBuilder::TrunkWidthType w, ChunkMap* chunkMap, const glm::ivec2& chunkXZ, const glm::ivec3& treeLocalPos, std::mt19937& engine)
{
	switch (type)
	{
	case TreeBuilder::TreeType::OAK:
		TreeBuilder::createOakTree(h, w, chunkMap, chunkXZ, treeLocalPos, engine);
		break;
	case TreeBuilder::TreeType::BIRCH:
		TreeBuilder::createBirchTree(h, w, chunkMap, chunkXZ, treeLocalPos, engine);
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
	int trunkHeight = getRandomTreeTrunkHeight(TreeBuilder::TreeType::OAK, h, engine);

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

	auto trunkBottomColor = oakWoodColor;

	// Add layer 2 (p5 ~ p12)
	addPosLayer(p, 2);
	// Add layer 3 (p13 ~ p20)
	addPosLayer(p, 3);
	// Add layer 4 (p21 ~ p24)
	addPosLayer(p, 4);

	// get leaves size
	int leavesWidth, leavesHeight, leavesLength;
	getRandomLeavesSize(TreeBuilder::TreeType::OAK, w, leavesWidth, leavesHeight, leavesLength, engine);

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
			chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
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
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
					p.at(i).y--;
				}

				for (int i = 13; i <= 20; i++)
				{
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
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
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
					p.at(i).y++;
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
					p.at(i).y -= 2;
				}

				for (int i = 13; i <= 20; i++)
				{
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				}

				for (int i = 21; i <= 24; i++)
				{
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
					p.at(i).y++;
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
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
		for (unsigned int i = 1; i <= 24; i++)
		{
			p.at(i).y--;
		}

		// go 10 block deep
		for (int i = 0; i < rootHeight; i++)
		{
			for (unsigned int i = 1; i <= 24; i++)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
				p.at(i).y--;
			}
		}
		
		// Calculate leave center pos. 
		auto leavesCenterPos = glm::ivec3(p.at(1).x, pivot.y + trunkHeight + leavesHeight / 2, p.at(1).z);

		addOakLeaves(chunkMap, leavesWidth, leavesHeight, leavesLength, leavesCenterPos, engine);

		// Add more blocks around top of trunk, below the leaves
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

		// Small trees can have up to 1 branch and it's 50/50
		int branchRand = std::uniform_int_distribution<>(0, 100)(engine);
		if (branchRand > 50)
		{
			// add branch
			addOakBranch(chunkMap, p, pivot.y + trunkHeight - leavesHeight, engine);
		}
	}
	else
	{
		// Add layer 5 (p25 ~ p40)
		addPosLayer(p, 5);

		if (w == TreeBuilder::TrunkWidthType::MEDIUM)
		{
			// Start from bottom 10 to make sure it renders trunk even it's on steep mountain
			int trunkY = pivot.y - 10;
		
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
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
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
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
					p.at(i).y--;
				}

				for (int i = 25; i <= 40; ++i)
				{
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
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
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
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
				for (unsigned int i = 1; i <= 40; i++)
				{
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
					p.at(i).y--;
				}
			}

			// Calculate leave center pos. 
			auto leavesCenterPos = glm::ivec3(p.at(1).x, pivot.y + trunkHeight + (leavesHeight / 2), p.at(1).z);

			addOakLeaves(chunkMap, leavesWidth, leavesHeight, leavesLength, leavesCenterPos, engine);
		}
		else if (w == TreeBuilder::TrunkWidthType::LARGE)
		{
			addPosLayer(p, 6);

			addOakTrunk(chunkMap, p, oakWoodColor, colorStep, 1, 24, trunkHeight, trunkY);

			for (int i = 25; i <= 60; ++i)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
			}

			if (tRand == 0)
			{
				for (int i = 25; i <= 40; ++i)
				{
					p.at(i).y++;
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
					p.at(i).y--;
				}
			}
			else
			{
				for (int i = 25; i <= 40; ++i)
				{
					p.at(i).y++;
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				}

				p.at(25).y++;
				p.at(26).y++;
				chunkMap->placeBlockAt(p.at(25), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				chunkMap->placeBlockAt(p.at(26), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				p.at(25).y -= 2;
				p.at(26).y -= 2;

				p.at(29).y++;
				p.at(30).y++;
				chunkMap->placeBlockAt(p.at(29), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				chunkMap->placeBlockAt(p.at(30), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				p.at(29).y -= 2;
				p.at(30).y -= 2;

				p.at(33).y++;
				p.at(34).y++;
				chunkMap->placeBlockAt(p.at(33), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				chunkMap->placeBlockAt(p.at(34), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				p.at(33).y -= 2;
				p.at(34).y -= 2;

				p.at(37).y++;
				p.at(38).y++;
				chunkMap->placeBlockAt(p.at(37), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
				chunkMap->placeBlockAt(p.at(38), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
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
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
						p.at(i).y -= 2;
					}

					for (int i = 31; i <= 32; i++)
					{
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
						p.at(i).y -= 2;
					}

					for (int i = 35; i <= 36; i++)
					{
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
						p.at(i).y -= 2;
					}

					for (int i = 39; i <= 40; i++)
					{
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
						p.at(i).y++;
						chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
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
				for (unsigned int i = 1; i <= 60; i++)
				{
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
					p.at(i).y--;
				}
			}

			// Calculate leave center pos. 
			auto leavesCenterPos = glm::ivec3(p.at(1).x, pivot.y + trunkHeight + (leavesHeight / 2), p.at(1).z);

			addOakLeaves(chunkMap, leavesWidth, leavesHeight, leavesLength, leavesCenterPos, engine);
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
	int trunkHeight = getRandomTreeTrunkHeight(TreeBuilder::TreeType::BIRCH, h, engine);

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

	//  build trunk and root.
	/*
		p4 p3
		p2 p1
	*/

	// Start from bottom 10 to make sure it renders trunk even it's on steep mountain
	int trunkY = pivot.y - 10;

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

	// get leaves size
	int leavesWidth, leavesHeight, leavesLength;
	getRandomLeavesSize(TreeBuilder::TreeType::BIRCH, w, leavesWidth, leavesHeight, leavesLength, engine);

	// Poll random.
	int tRand = std::uniform_int_distribution<>(0, 2)(engine);

	int rootHeight = 10;

	if (w == TreeBuilder::TrunkWidthType::SMALL)
	{
		// Add small trunk
		/*
			p4 p3
			p2 p1
		*/
		addBirchTrunk(chunkMap, p, birchWoodWhiteColor, colorStep, birchWoodBlackColor, 1, 4, trunkHeight, trunkY, engine);

		// from p1 ~ p12, add blocks 
		for (int i = 1; i <= 12; ++i)
		{
			chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::BIRCH_WOOD_WHITE, birchWoodWhiteColor, nullptr, true);
		}

		// Based on trunk random, add more blocks around.
		if (tRand < 2)
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
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::BIRCH_WOOD_WHITE, birchWoodWhiteColor, nullptr, true);
					p.at(i).y--;
				}

				for (int i = 13; i <= 20; i++)
				{
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::BIRCH_WOOD_WHITE, birchWoodWhiteColor, nullptr, true);
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
		for (unsigned int i = 1; i <= 24; i++)
		{
			p.at(i).y--;
		}

		// go 10 block deep
		for (int i = 0; i < rootHeight; i++)
		{
			for (unsigned int i = 1; i <= 24; i++)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, birchWoodWhiteColor, nullptr, false);
				p.at(i).y--;
			}
		}

		// Calculate leave center pos. 
		auto leavesCenterPos = glm::ivec3(p.at(1).x, pivot.y + trunkHeight + leavesHeight / 2, p.at(1).z);

		addBirchLeaves(chunkMap, leavesWidth, leavesHeight, leavesLength, leavesCenterPos, engine);

		// Birch tree doens't have additional blocks aroudn top trunk.

	}
	// Birch tree can't have LARGE width type. So it's MEDIUM else than SMALL.
	else
	{

	}
}

std::string Voxel::TreeBuilder::treeTypeToString(TreeBuilder::TreeType type)
{
	switch (type)
	{
		break;
	case Voxel::TreeBuilder::TreeType::OAK:
		return "OAK";
	case Voxel::TreeBuilder::TreeType::BIRCH:
		return "BIRCH";
	case Voxel::TreeBuilder::TreeType::SPRUCE:
		return "SPRUCE";
	case Voxel::TreeBuilder::TreeType::PINE:
		return "PINE";
	case Voxel::TreeBuilder::TreeType::NONE:
	default:
		return "NONE";
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

int Voxel::TreeBuilder::getRandomTreeTrunkHeight(const TreeBuilder::TreeType & treeType, const TreeBuilder::TrunkHeightType& trunkHeight, std::mt19937 & engine)
{
	int height = 0;

	switch (treeType)
	{
	case TreeBuilder::TreeType::OAK:
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
	case TreeBuilder::TreeType::BIRCH:
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
	default:
		break;
	}

	return height;
}

void Voxel::TreeBuilder::getRandomLeavesSize(const TreeBuilder::TreeType & treeType, const TreeBuilder::TrunkWidthType& trunkWidthType, int & width, int & height, int & length, std::mt19937 & engine)
{
	// init
	width = 0;
	height = 0;
	length = 0;

	switch (treeType)
	{
	case TreeBuilder::TreeType::OAK:
	{
		switch (trunkWidthType)
		{
		case TreeBuilder::TrunkWidthType::SMALL:
		{
			auto dist = std::uniform_int_distribution<>(0, 2);

			// Get leaves size
			width = dist(engine) + 8;		// 8 ~ 10
			height = dist(engine) + 5;		// 5 ~ 7
			length = dist(engine) + 8;		// 8 ~ 10
		}
		break;
		case TreeBuilder::TrunkWidthType::MEDIUM:
		{
			auto dist = std::uniform_int_distribution<>(0, 2);

			width = dist(engine) + 12;		// 12 ~ 14
			height = dist(engine) + 6;		// 6 ~ 8
			length = dist(engine) + 12;		// 12 ~ 14
		}
		break;
		case TreeBuilder::TrunkWidthType::LARGE:
		{
			auto dist = std::uniform_int_distribution<>(0, 2);

			width = dist(engine) + 14;		// 14 ~ 16
			height = dist(engine) + 8;		// 8 ~ 10
			length = dist(engine) + 14;		// 14 ~ 16
		}
		break;
		default:
			break;
		}
	}
	break;
	case TreeBuilder::TreeType::BIRCH:
	{
		switch (trunkWidthType)
		{
		case TreeBuilder::TrunkWidthType::SMALL:
		{
			auto dist = std::uniform_int_distribution<>(0, 2);

			// Get leaves size
			width = dist(engine) + 4;		// 8 ~ 10
			height = dist(engine) + 8;		// 5 ~ 7
			length = dist(engine) + 4;		// 8 ~ 10
		}
		break;
		case TreeBuilder::TrunkWidthType::MEDIUM:
		{
			auto dist = std::uniform_int_distribution<>(0, 2);

			width = dist(engine) + 6;		// 12 ~ 14
			height = dist(engine) + 12;		// 6 ~ 8
			length = dist(engine) + 6;		// 12 ~ 14
		}
		break;
		case TreeBuilder::TrunkWidthType::LARGE:
		{
			auto dist = std::uniform_int_distribution<>(0, 2);

			width = dist(engine) + 8;		// 14 ~ 16
			height = dist(engine) + 14;		// 8 ~ 10
			length = dist(engine) + 8;		// 14 ~ 16
		}
		break;
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
	int sizeHalf = size / 2;
	
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

void Voxel::TreeBuilder::addOakLeaves(ChunkMap * map, const int w, const int h, const int l, const glm::ivec3& pos, std::mt19937& engine)
{
	auto dist = std::uniform_int_distribution<>(0, 100);

	int mlRand = dist(engine);

	auto l1 = pos;

	if (mlRand < 50)
	{
		addOakLeave(map, w, h, l, l1, engine);
	}
	else
	{
		l1.x -= 1;
		l1.y -= 2;
		addOakLeave(map, w, h / 3 * 2, l, l1, engine);

		l1.x += 1;
		
		l1.z += std::uniform_int_distribution<>(3, 4)(engine) * ((std::uniform_int_distribution<>(0, 1)(engine)) ? -1 : 1);
		l1.y += 3;

		addOakLeave(map, w, h / 3 * 2, l, l1, engine);
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

		addOakLeave(map, sideLeavesWidth, sideLeavesHeight, sideLeavesLength, sidePos, engine);
	}
}

void Voxel::TreeBuilder::addOakLeave(ChunkMap * map, const int w, const int h, const int l, const glm::ivec3 & pos, std::mt19937& engine)
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

void Voxel::TreeBuilder::addBirchTrunk(ChunkMap * map, std::vector<glm::ivec3>& p, glm::vec3 trunkColor, const glm::vec3 & colorStep, const glm::vec3 & markColor, const int pStart, const int pEnd, const int trunkHeight, const int startY, std::mt19937& engine)
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
			int dirRand = markDist(engine);

			glm::vec3 m1;
			glm::vec3 m2;

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

void Voxel::TreeBuilder::addBirchLeaves(ChunkMap * map, const int w, const int h, const int l, const glm::ivec3 & pos, std::mt19937 & engine)
{
	// main leaves
	addBirchLeave(map, w, h, l, pos, engine);

	// sub leaves

}

void Voxel::TreeBuilder::addBirchLeave(ChunkMap * map, const int w, const int h, const int l, const glm::ivec3 & pos, std::mt19937 & engine)
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