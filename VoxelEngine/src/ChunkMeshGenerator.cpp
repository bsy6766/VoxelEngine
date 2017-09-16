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

void Voxel::ChunkMeshGenerator::generateSingleChunkMesh(Chunk * chunk, ChunkMap * chunkMap)
{
	std::vector<float> vertices;
	std::vector<float> colors;
	std::vector<float> normals;
	std::vector<unsigned int> indices;

	//std::cout << "[ChunkMeshGenerator] -> Chunk (" << chunk->position.x << ", " << chunk->position.y << ", " << chunk->position.z << ")" << std::endl;
	//std::cout << "[ChunkMeshGenerator] -> Total chunk sections: " << chunk->chunkSections.size() << std::endl;

	auto chunkStart = Utility::Time::now();

	// Iterate all chunk sections O(16)
	int indicesOffsetPerBlock = 0;
	for (auto chunkSection : chunk->chunkSections)
	{
		if (chunkSection == nullptr)
		{
			// There is no block in this chunksection
			continue;
		}
		//std::cout << "[ChunkMeshGenerator] -> Generating for chunk section at (" << chunkSection->position.x << ", " << chunkSection->position.y << ", " << chunkSection->position.z << ")" << std::endl;

		// Iterate all blocks. O(4096)
		//auto chunkSectionStart = Utility::Time::now();

		int shadeMode = Setting::getInstance().getBlockShadeMode();

		for (int blockX = 0; blockX < Constant::CHUNK_SECTION_WIDTH; blockX++)
		{
			for (int blockZ = 0; blockZ < Constant::CHUNK_SECTION_LENGTH; blockZ++)
			{
				for (int blockY = Constant::CHUNK_SECTION_HEIGHT - 1; blockY >= 0; blockY--)
				{
					unsigned int blockIndex = chunkSection->localBlockXYZToIndex(blockX, blockY, blockZ);

					if (blockIndex < 0 || blockIndex >= Constant::TOTAL_BLOCKS)
					{
						std::cout << "Out or range (" << blockX << ", " << blockY << ", " << blockZ << ")" << std::endl;
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

						{
							// Get adjacent block and check if it's transparent or not
							// Up. Up face is different compared to sides. Add only if above block is transparent or chunk section doesn't exists
							int blockUp = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, worldPos.y + 1, worldPos.z);
							if (blockUp == 0 || blockUp == 2)
							{
								// Block exists and transparent. Add face
								face |= Cube::Face::TOP;
							}

							// Down. If current block is the most bottom block, doesn't have to add face
							if (worldPos.y > 0)
							{
								int blockDown = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, worldPos.y - 1, worldPos.z);
								if (blockDown == 0 || blockDown == 2)
								{
									// Block exists and transparent. Add face
									face |= Cube::Face::BOTTOM;
								}
							}


							// Sides. Side faces (Left, right, front, back) is different compared to up and down. 
							// Only add faces if side block is transparent or chunk section is nullptr. 
							// If chunk doesn't exist, don't add.
							// Left
							auto blockLeft = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, worldPos.y, worldPos.z);
							if (blockLeft == 0 || blockLeft == 2)
							{
								face |= Cube::Face::LEFT;
							}

							// Right
							auto blockRight = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, worldPos.y, worldPos.z);
							if (blockRight == 0 || blockRight == 2)
							{
								face |= Cube::Face::RIGHT;
							}

							// Front
							auto blockFront = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, worldPos.y, worldPos.z - 1);
							if (blockFront == 0 || blockFront == 2)
							{
								face |= Cube::Face::FRONT;
							}

							// Back
							auto blockBack = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, worldPos.y, worldPos.z + 1);
							if (blockBack == 0 || blockBack == 2)
							{
								face |= Cube::Face::BACK;
							}
						}

						//auto bt2 = Utility::Time::now();
						//std::cout << "block t: " << Utility::Time::toMicroSecondString(bt1, bt2) << std::endl;
						// block t = 1 ~ 6 micro seconds

						// After checking adjacent, check near by

						// Shadow weight for each vertex point of block. Weight gets added by 1 whenever other opaque blocks touches the vertex point.
						std::vector<unsigned int> shadowWeight;

						//auto st1 = Utility::Time::now();

						if (shadeMode == 2)
						{

							shadowWeight.resize(16, 0);

							/*
											top view
												+z
										below			above
											0 7 6	8 15 14
									+x		1 - 5   9  + 13		 -x
											2 3 4  10 11 12

												-z
							*/

							// Below first
							{
								const int belowY = worldPos.y - 1;
								if (belowY >= 0)
								{
									int block0 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, belowY, worldPos.z + 1);
									if (block0 == 1)
									{
										shadowWeight.at(0) += 1;
									}

									int block1 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, belowY, worldPos.z);
									if (block1 == 1)
									{
										shadowWeight.at(1) += 1;
									}

									int block2 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, belowY, worldPos.z - 1);
									if (block2 == 1)
									{
										shadowWeight.at(2) += 1;
									}

									int block3 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, belowY, worldPos.z - 1);
									if (block3 == 1)
									{
										shadowWeight.at(3) += 1;
									}

									int block4 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, belowY, worldPos.z - 1);
									if (block4 == 1)
									{
										shadowWeight.at(4) += 1;
									}

									int block5 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, belowY, worldPos.z);
									if (block5 == 1)
									{
										shadowWeight.at(5) += 1;
									}

									int block6 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, belowY, worldPos.z + 1);
									if (block6 == 1)
									{
										shadowWeight.at(6) += 1;
									}

									int block7 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, belowY, worldPos.z + 1);
									if (block7 == 1)
									{
										shadowWeight.at(7) += 1;
									}
								}
							}

							// Then, above
							{
								const int aboveY = worldPos.y + 1;
								if (aboveY <= Constant::HEIGHEST_BLOCK_Y)
								{
									int block8 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, aboveY, worldPos.z + 1);
									if (block8 == 1)
									{
										shadowWeight.at(8) += 1;
									}

									int block9 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, aboveY, worldPos.z);
									if (block9 == 1)
									{
										shadowWeight.at(9) += 1;
									}

									int block10 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, aboveY, worldPos.z - 1);
									if (block10 == 1)
									{
										shadowWeight.at(10) += 1;
									}

									int block11 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, aboveY, worldPos.z - 1);
									if (block11 == 1)
									{
										shadowWeight.at(11) += 1;
									}

									int block12 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, aboveY, worldPos.z - 1);
									if (block12 == 1)
									{
										shadowWeight.at(12) += 1;
									}

									int block13 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, aboveY, worldPos.z);
									if (block13 == 1)
									{
										shadowWeight.at(13) += 1;
									}

									int block14 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, aboveY, worldPos.z + 1);
									if (block14 == 1)
									{
										shadowWeight.at(14) += 1;
									}

									int block15 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, aboveY, worldPos.z + 1);
									if (block15 == 1)
									{
										shadowWeight.at(15) += 1;
									}
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
								Cube::getColors4WithShade(static_cast<Cube::Face>(face), blockColor, shadowWeight, colors);
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

		//std::cout << "[ChunkMeshGenerator] -> Done." << std::endl;
	}

	//std::cout << "[ChunkMeshGenerator] finished building mesh (" << chunk->getPosition().x << ", " << chunk->getPosition().z << ")" << std::endl;

	//auto chunkEnd = Utility::Time::now();
	//std::cout << "[ChunkMeshGenerator] -> Chunk Elapsed time: " << Utility::Time::toMilliSecondString(chunkStart, chunkEnd) << std::endl;
	
	chunk->chunkMesh->initBuffer(vertices, colors, normals, indices);

	//std::cout << "[ChunkMeshGenerator] -> Total vertices: " << vertices.size() << std::endl;
}