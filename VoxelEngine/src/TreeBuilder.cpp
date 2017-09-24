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
	/*
		p4 p3
		p2 p1
	*/

	// Start from bottom 10 to make sure it renders trunk even it's on steep mountain
	int trunkY = pivot.y - 10;

	std::vector<glm::ivec3> p = 
	{
		pivot,
		glm::ivec3(pivot.x, pivot.y, pivot.z),			// p1
		glm::ivec3(pivot.x + 1, pivot.y, pivot.z),		// p2
		glm::ivec3(pivot.x, pivot.y, pivot.z + 1),		// p3
		glm::ivec3(pivot.x + 1, pivot.y, pivot.z + 1),	// p4
	};

	// Add more blocks around bottom of trunk
	auto oakWoodColor = Color::colorU3TocolorV3(Color::OAK_WOOD);
	auto colorStep = oakWoodColor * 0.025f;

	oakWoodColor = oakWoodColor - (colorStep * (static_cast<float>(trunkHeight) * 0.5f));

	auto trunkBottomColor = oakWoodColor;

	p.push_back(glm::ivec3(p.at(1).x, pivot.y, p.at(1).z - 1));		// p5
	p.push_back(glm::ivec3(p.at(2).x, pivot.y, p.at(2).z - 1));		// p6
	p.push_back(glm::ivec3(p.at(2).x + 1, pivot.y, p.at(2).z));		// p7
	p.push_back(glm::ivec3(p.at(4).x + 1, pivot.y, p.at(4).z));		// p8
	p.push_back(glm::ivec3(p.at(4).x, pivot.y, p.at(4).z + 1));		// p9
	p.push_back(glm::ivec3(p.at(3).x, pivot.y, p.at(3).z + 1));		// p10
	p.push_back(glm::ivec3(p.at(3).x - 1, pivot.y, p.at(3).z));		// p11
	p.push_back(glm::ivec3(p.at(1).x - 1, pivot.y, p.at(1).z));		// p12

	p.push_back(glm::ivec3(p.at(5).x, pivot.y, p.at(5).z - 1));		// p13
	p.push_back(glm::ivec3(p.at(6).x, pivot.y, p.at(6).z - 1));		// p14
	p.push_back(glm::ivec3(p.at(7).x + 1, pivot.y, p.at(7).z));		// p15
	p.push_back(glm::ivec3(p.at(8).x + 1, pivot.y, p.at(8).z));		// p16
	p.push_back(glm::ivec3(p.at(9).x, pivot.y, p.at(9).z + 1));		// p17
	p.push_back(glm::ivec3(p.at(10).x, pivot.y, p.at(10).z + 1));		// p18
	p.push_back(glm::ivec3(p.at(11).x - 1, pivot.y, p.at(11).z));		// p19
	p.push_back(glm::ivec3(p.at(12).x - 1, pivot.y, p.at(12).z));		// p20

	p.push_back(glm::ivec3(p.at(1).x - 1, pivot.y, p.at(1).z - 1));	// p21
	p.push_back(glm::ivec3(p.at(2).x + 1, pivot.y, p.at(2).z - 1));	// p22
	p.push_back(glm::ivec3(p.at(4).x + 1, pivot.y, p.at(4).z + 1));	// p23
	p.push_back(glm::ivec3(p.at(3).x - 1, pivot.y, p.at(3).z + 1));	// p24

	int tRand = Utility::Random::randomInt(0, 3);

	if (w == TreeBuilder::TrunkWidth::SMALL)
	{
		for (int i = 1; i <= 4; ++i)
		{
			p.at(i).y = trunkY;
		}

		addTrunk(chunkMap, p, oakWoodColor, colorStep, 1, 4, trunkHeight);

		for (int i = 1; i <= 4; ++i)
		{
			p.at(i).y = pivot.y;
		}

		for (int i = 1; i <= 12; ++i)
		{
			chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
		}

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

		for (unsigned int i = 1; i <= 24; i++)
		{
			p.at(i).y--;
		}

		for (int i = 0; i < rootHeight; i++)
		{
			for (unsigned int i = 1; i <= 24; i++)
			{
				chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, false);
				p.at(i).y--;
			}
		}

		// add main leave above trunk
		int leavesWidth = Utility::Random::randomInt(8, 10);
		int leavesHeight = Utility::Random::randomInt(6, 7);
		int leavesLength = Utility::Random::randomInt(8, 10);

		//auto l1 = glm::ivec3(p1.x, pivot.y + trunkHeight + (leavesHeight / 2), p1.z);
		auto l1 = glm::ivec3(p.at(1).x, pivot.y + trunkHeight + 2, p.at(1).z);

		addOakLeaves(chunkMap, p, leavesWidth, leavesHeight, leavesLength, l1);

		// Add more blocks around top of trunk, below the leaves
		{
			int newY = pivot.y + trunkHeight;		
			
			for (int i = 5; i <= 12; ++i)
			{
				p.at(i).y = newY;
			}

			for (int i = 0; i < 7; i++)
			{
				for (int i = 5; i <= 12; ++i)
				{
					chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
					p.at(i).y--;
				}
			}
		}

		addOakBranch(chunkMap, p, pivot.y + trunkHeight + 2 - 3 - leavesHeight);
	}
	else if (w == TreeBuilder::TrunkWidth::MEDIUM)
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

		for (int i = 1; i <= 12; ++i)
		{
			p.at(i).y = trunkY;
		}
		
		addTrunk(chunkMap, p, oakWoodColor, colorStep, 1, 12, trunkHeight);

		for (int i = 1; i <= 12; ++i)
		{
			p.at(i).y = pivot.y;
		}

		for (int i = 13; i <= 24; ++i)
		{
			chunkMap->placeBlockAt(p.at(i), Block::BLOCK_ID::OAK_WOOD, trunkBottomColor, nullptr, true);
		}

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

		int rootHeight = 10;

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

		// add main leave above trunk
		int leavesWidth = Utility::Random::randomInt(12, 14);
		int leavesHeight = Utility::Random::randomInt(6, 8);
		int leavesLength = Utility::Random::randomInt(12, 14);

		auto l1 = glm::ivec3(p.at(1).x, pivot.y + trunkHeight + 2, p.at(1).z);

		addOakLeaves(chunkMap, p, leavesWidth, leavesHeight, leavesLength, l1);
	}
}

void Voxel::TreeBuilder::addTrunk(ChunkMap * map, std::vector<glm::ivec3>& p, glm::vec3 color, const glm::vec3& colorStep, const int pStart, const int pEnd, const int trunkHeight)
{
	int size = trunkHeight + 10;
	int sizeHalf = size / 2;
	
	auto indexMid = 10 + (trunkHeight / 2);

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
}

void Voxel::TreeBuilder::addOakLeaves(ChunkMap * map, std::vector<glm::ivec3>& p, const int w, const int h, const int l, const glm::ivec3& pos)
{
	int mlRand = Utility::Random::randomInt100();
	auto l1 = pos;

	if (mlRand < 50)
	{
		addOakLeave(map, w, h, l, l1);
	}
	else
	{
		l1.x -= 1;
		l1.y -= 2;
		addOakLeave(map, w, h / 3 * 2, l, l1);

		l1.x += 1;
		l1.z += Utility::Random::randomInt(3, 4) * Utility::Random::random_int_minus_1_1();
		l1.y += 3;
		addOakLeave(map, w, h / 3 * 2, l, l1);
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

		glm::ivec3 sidePos(0);

		if (mlRand)
		{
			x = Utility::Random::randomInt(w / 2, (w / 4) * 3) * Utility::Random::random_int_minus_1_1();
			y = Utility::Random::randomInt(0, h / 2);
			z = Utility::Random::randomInt(l / 2, (l / 4) * 3) * Utility::Random::random_int_minus_1_1();
			sidePos = glm::ivec3(x, y, z) + l1;
		}
		else
		{
			x = Utility::Random::randomInt((w / 4), w / 2) * Utility::Random::random_int_minus_1_1();
			y = Utility::Random::randomInt(0, h / 2);
			z = Utility::Random::randomInt((l / 4) * 3, l / 2) * Utility::Random::random_int_minus_1_1();
			sidePos = glm::ivec3(x, y, z) + l1;
		}

		addOakLeave(map, sideLeavesWidth, sideLeavesHeight, sideLeavesLength, sidePos);
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
	if (Utility::Random::randomInt100() < 50)
	{
		leaveColor += (leaveColor * (Utility::Random::random_float_minus_1_1() * Utility::Random::randomReal<float>(0.0f, 0.05f)));
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

void Voxel::TreeBuilder::addOakBranch(ChunkMap * map, std::vector<glm::ivec3>& p, const int branchBaseY)
{
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
		int branchY = branchBaseY - Utility::Random::randomInt(1, 2);

		auto b1 = glm::ivec3(0);
		auto b2 = glm::ivec3(0);
			
		int randDir = Utility::Random::randomInt(0, 3);

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

		b1.y = branchY;
		b2.y = branchY;

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

		int branchLeaveWidth = Utility::Random::randomInt(2, 3);
		int branchLeavesHeight = Utility::Random::randomInt(1, 2);
		int branchLeavesLength = Utility::Random::randomInt(2, 3);

		b1.y += (branchLeavesHeight / 2);
			
		addOakLeave(map, branchLeaveWidth, branchLeavesHeight, branchLeavesLength, b1);
	}
	else if (totalBranch == 2)
	{
	}
}
