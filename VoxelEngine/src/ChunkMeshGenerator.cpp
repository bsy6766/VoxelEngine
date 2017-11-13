#include "ChunkMeshGenerator.h"

#include <Chunk.h>
#include <ChunkSection.h>
#include <ChunkMap.h>
#include <ChunkMesh.h>
#include <Block.h>
#include <Cube.h>
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <Utility.h>
#include <Setting.h>

using namespace Voxel;

void Voxel::ChunkMeshGenerator::generateChunkMesh(Chunk * chunk, ChunkMap * chunkMap)
{
	std::vector<float> vertices;
	std::vector<float> colors;
	std::vector<float> normals;
	std::vector<unsigned int> indices;

	//std::cout << "[ChunkMeshGenerator] -> Chunk (" << chunk->position.x << ", " << chunk->position.y << ", " << chunk->position.z << ")\n";
	//std::cout << "[ChunkMeshGenerator] -> Total chunk sections: " << chunk->chunkSections.size() << std::endl;

	//auto chunkStart = Utility::Time::now();

	int shadeMode = Setting::getInstance().getBlockShadeMode();

	// Iterate all chunk sections O(16)
	int indicesOffsetPerBlock = 0;
	for (auto chunkSection : chunk->chunkSections)
	{
		if (chunkSection == nullptr)
		{
			// There is no block in this chunksection
			continue;
		}

		//std::cout << "[ChunkMeshGenerator] -> Generating for chunk section at (" << chunkSection->position.x << ", " << chunkSection->position.y << ", " << chunkSection->position.z << ")\n";

		// Iterate all blocks. O(4096)
		//auto chunkSectionStart = Utility::Time::now();
		
		for (int blockX = 0; blockX < Constant::CHUNK_SECTION_WIDTH; blockX++)
		{
			for (int blockZ = 0; blockZ < Constant::CHUNK_SECTION_LENGTH; blockZ++)
			{
				for (int blockY = Constant::CHUNK_SECTION_HEIGHT - 1; blockY >= 0; blockY--)
				{
					unsigned int blockIndex = chunkSection->localBlockXYZToIndex(blockX, blockY, blockZ);

					if (blockIndex < 0 || blockIndex >= Constant::TOTAL_BLOCKS)
					{
						std::cout << "Out or range (" << blockX << ", " << blockY << ", " << blockZ << ")\n";
						std::cout << "blockIndex = " << blockIndex << std::endl;
						throw std::runtime_error("out of range");
					}

					Block* block = chunkSection->blocks.at(blockIndex);

					if (block == nullptr)
					{
						// Skip air.
						continue;
					}
					else
					{
						// Add face if it's not air.
						unsigned int face = Cube::Face::NONE;
						// Block's world position
						auto worldPos = block->worldCoordinate;
						//auto localPos = block->localCoordinate;

						// To check weight, we need to query 8 blocks around y - 1 and y + 1.
						// Also we need to check for adjacent blocks
						// So total (8 + 8 + 6 - 2(redundant)) = 20 blocks query per block.

						//auto bt1 = Utility::Time::now();
						// Get adjacent block and check if it's transparent or not
						// Up. Up face is different compared to sides. Add only if above block is transparent or chunk section doesn't exists
						ChunkMap::BQR blockUp = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, worldPos.y + 1, worldPos.z);
						if (blockUp == ChunkMap::BQR::EXIST_TRANSPARENT || blockUp == ChunkMap::BQR::NO_CHUNK_SECTION)
						{
							// Block exists and transparent. Add face
							face |= Cube::Face::TOP;
						}

						// Down. If current block is the most bottom block, doesn't have to add face
						if (worldPos.y > 0)
						{
							ChunkMap::BQR blockDown = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, worldPos.y - 1, worldPos.z);
							if (blockDown == ChunkMap::BQR::EXIST_TRANSPARENT)
							{
								// Block exists and transparent. Add face
								face |= Cube::Face::BOTTOM;
							}
						}


						// Sides. Side faces (Left, right, front, back) is different compared to up and down. 
						// Only add faces if side block is transparent or chunk section is nullptr. 
						// If chunk doesn't exist, don't add.
						// Left
						ChunkMap::BQR blockLeft = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, worldPos.y, worldPos.z);
						if (blockLeft == ChunkMap::BQR::EXIST_TRANSPARENT || blockLeft == ChunkMap::BQR::NO_CHUNK_SECTION)
						{
							face |= Cube::Face::LEFT;
						}

						// Right
						ChunkMap::BQR blockRight = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, worldPos.y, worldPos.z);
						if (blockRight == ChunkMap::BQR::EXIST_TRANSPARENT || blockRight == ChunkMap::BQR::NO_CHUNK_SECTION)
						{
							face |= Cube::Face::RIGHT;
						}

						// Front
						ChunkMap::BQR blockFront = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, worldPos.y, worldPos.z - 1);
						if (blockFront == ChunkMap::BQR::EXIST_TRANSPARENT || blockFront == ChunkMap::BQR::NO_CHUNK_SECTION)
						{
							face |= Cube::Face::FRONT;
						}

						// Back
						ChunkMap::BQR blockBack = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, worldPos.y, worldPos.z + 1);
						if (blockBack == ChunkMap::BQR::EXIST_TRANSPARENT || blockBack == ChunkMap::BQR::NO_CHUNK_SECTION)
						{
							face |= Cube::Face::BACK;
						}

						if (face == Cube::Face::NONE)
						{
							// Skip if it's surrounded by blocks.
							continue;
						}

						//auto bt2 = Utility::Time::now();
						//std::cout << "block t: " << Utility::Time::toMicroSecondString(bt1, bt2) << std::endl;
						// block t = 1 ~ 6 micro seconds

						// After checking adjacent, check near by

						// Shadow weight for each vertex point of block. Weight gets added by 1 whenever other opaque blocks touches the vertex point.
						std::vector<unsigned int> shadeWeight;

						//auto st1 = Utility::Time::now();

						if (shadeMode == 2)
						{

							shadeWeight.resize(20, 0);

							/*
							top view
							+z
							below	above		mid
							0 7 6	8 15 14		16 _ 19
							+x		1 - 5   9  + 13		 _	 _	-x
							2 3 4  10 11 12		17 _ 18

							-z
							*/

							// Below first
							{
								const int belowY = worldPos.y - 1;
								if (belowY >= 0)
								{
									ChunkMap::BQR block0 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, belowY, worldPos.z + 1);
									if (block0 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(0) += 1;
									}

									ChunkMap::BQR block1 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, belowY, worldPos.z);
									if (block1 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(1) += 1;
									}

									ChunkMap::BQR block2 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, belowY, worldPos.z - 1);
									if (block2 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(2) += 1;
									}

									ChunkMap::BQR block3 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, belowY, worldPos.z - 1);
									if (block3 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(3) += 1;
									}

									ChunkMap::BQR block4 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, belowY, worldPos.z - 1);
									if (block4 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(4) += 1;
									}

									ChunkMap::BQR block5 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, belowY, worldPos.z);
									if (block5 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(5) += 1;
									}

									ChunkMap::BQR block6 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, belowY, worldPos.z + 1);
									if (block6 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(6) += 1;
									}

									ChunkMap::BQR block7 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, belowY, worldPos.z + 1);
									if (block7 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(7) += 1;
									}
								}
							}

							// Then, above
							{
								const int aboveY = worldPos.y + 1;
								if (aboveY <= Constant::HEIGHEST_BLOCK_Y)
								{
									ChunkMap::BQR block8 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, aboveY, worldPos.z + 1);
									if (block8 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(8) += 1;
									}

									ChunkMap::BQR block9 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, aboveY, worldPos.z);
									if (block9 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(9) += 1;
									}

									ChunkMap::BQR block10 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, aboveY, worldPos.z - 1);
									if (block10 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(10) += 1;
									}

									ChunkMap::BQR block11 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, aboveY, worldPos.z - 1);
									if (block11 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(11) += 1;
									}

									ChunkMap::BQR block12 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, aboveY, worldPos.z - 1);
									if (block12 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(12) += 1;
									}

									ChunkMap::BQR block13 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, aboveY, worldPos.z);
									if (block13 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(13) += 1;
									}

									ChunkMap::BQR block14 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, aboveY, worldPos.z + 1);
									if (block14 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(14) += 1;
									}

									ChunkMap::BQR block15 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, aboveY, worldPos.z + 1);
									if (block15 == ChunkMap::BQR::EXIST_OPAQUE)
									{
										shadeWeight.at(15) += 1;
									}
								}
							}

							// Then on same level
							{
								const int midY = worldPos.y;

								ChunkMap::BQR block16 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, midY, worldPos.z + 1);
								if (block16 == ChunkMap::BQR::EXIST_OPAQUE)
								{
									shadeWeight.at(16) += 1;
								}

								ChunkMap::BQR block17 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, midY, worldPos.z - 1);
								if (block17 == ChunkMap::BQR::EXIST_OPAQUE)
								{
									shadeWeight.at(17) += 1;
								}

								ChunkMap::BQR block18 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, midY, worldPos.z - 1);
								if (block18 == ChunkMap::BQR::EXIST_OPAQUE)
								{
									shadeWeight.at(18) += 1;
								}

								ChunkMap::BQR block19 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, midY, worldPos.z + 1);
								if (block19 == ChunkMap::BQR::EXIST_OPAQUE)
								{
									shadeWeight.at(19) += 1;
								}
							}
						}
						
						//auto st2 = Utility::Time::now();
						//std::cout << "shadow mode t: " << Utility::Time::toMicroSecondString(st1, st2) << std::endl;


						// Check face
						if (face == Cube::Face::NONE)
						{
							// Skip if it's surrounded by blocks
							continue;
						}
						else
						{
							//auto t1 = Utility::Time::now();
							auto worldPosition = block->getWorldPosition();

							auto blockVerticiesSize = Cube::getVertices(static_cast<Cube::Face>(face), worldPosition, vertices);

							Cube::getNormals(static_cast<Cube::Face>(face), worldPosition, normals);
							
							auto blockColor = block->getColor4();

							if (shadeMode == 2)
							{
								// Change color based on shade
								Cube::getColors4WithShade(static_cast<Cube::Face>(face), blockColor, shadeWeight, colors);
							}
							else if (shadeMode == 1)
							{
								Cube::getColors4WithoutShade(static_cast<Cube::Face>(face), blockColor, colors);
							}
							else
							{
								Cube::getColors4WithDefaultShade(static_cast<Cube::Face>(face), blockColor, colors);
							}

							Cube::getIndices(static_cast<Cube::Face>(face), indicesOffsetPerBlock, indices);

							indicesOffsetPerBlock += blockVerticiesSize;

							//auto t2 = Utility::Time::now();
							//std::cout << "build buffer t: " << Utility::Time::toMicroSecondString(t1, t2) << std::endl;
							// build buffer: 1~3 micro s.
						}
					}
				}
			}
		}

		//auto chunkSectionEnd = Utility::Time::now();
		//std::cout << "[ChunkMeshGenerator] -> Chunk section Elapsed time: " << Utility::Time::toMilliSecondString(chunkSectionStart, chunkSectionEnd) << std::endl;

		//std::cout << "[ChunkMeshGenerator] -> Done.\n";
	}

	//std::cout << "[ChunkMeshGenerator] finished building mesh (" << chunk->getPosition().x << ", " << chunk->getPosition().z << ")\n";

	//auto chunkEnd = Utility::Time::now();
	//std::cout << "[ChunkMeshGenerator] -> Chunk Elapsed time: " << Utility::Time::toMilliSecondString(chunkStart, chunkEnd) << std::endl;
	

	//auto bStart = Utility::Time::now();
	chunk->chunkMesh->initBuffer(vertices, colors, normals, indices);
	//auto bEnd = Utility::Time::now();
	//std::cout << "initBuffer t: " << Utility::Time::toMicroSecondString(bStart, bEnd) << std::endl;
	// initbuffer takes 30~10 micro seconds
	//std::cout << "[ChunkMeshGenerator] -> Total vertices: " << vertices.size() << std::endl;
}