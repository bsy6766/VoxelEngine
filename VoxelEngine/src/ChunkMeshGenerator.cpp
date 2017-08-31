#include "ChunkMeshGenerator.h"

#include <Chunk.h>
#include <ChunkSection.h>
#include <ChunkLoader.h>
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

	//auto chunkStart = Utility::Time::now();

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

		bool shadeMode = Setting::getInstance().getBlockShadeMode();

		for (int blockX = 0; blockX < Constant::CHUNK_SECTION_WIDTH; blockX++)
		{
			for (int blockZ = 0; blockZ < Constant::CHUNK_SECTION_LENGTH; blockZ++)
			{
				for (int blockY = Constant::CHUNK_SECTION_HEIGHT - 1; blockY >= 0; blockY--)
				{
					unsigned int blockIndex = chunkSection->XYZToIndex(blockX, blockY, blockZ);

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

						// After checking adjacent, check near by

						// Shadow weight for each vertex point of block. Weight gets added by 1 whenever other opaque blocks touches the vertex point.
						std::vector<unsigned int> shadowWeight;

						if (shadeMode)
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

						// Check face
						if (face == Cube::Face::NONE)
						{
							// Skip if it's surrounded by blocks
							continue;
						}
						else
						{
							auto worldPosition = block->getWorldPosition();

							/*
							if (worldPos == glm::ivec3(2, 79, 16))
							{
								for (auto w : shadowWeight)
								{
									std::cout << w << ", ";
								}
								std::cout << std::endl;
							}
							*/

							auto blockVertices = Cube::getVertices(static_cast<Cube::Face>(face), worldPosition);
							vertices.insert(vertices.end(), blockVertices.begin(), blockVertices.end());

							auto blockNormals = Cube::getNormals(static_cast<Cube::Face>(face), worldPosition);
							normals.insert(normals.end(), blockNormals.begin(), blockNormals.end());

							// temporary function for fake lighting. 
							//auto blockColors = Cube::getColors3(static_cast<Cube::Face>(face), block->color);
							std::vector<float> blockColors;

							auto blockColor = block->getColor4();
							if (shadeMode)
							{
								// Change color based on shade
								blockColors = Cube::getColors4WithShade(static_cast<Cube::Face>(face), blockColor, shadowWeight);
							}
							else
							{
								//blockColors = Cube::getColors4WithoutShade(static_cast<Cube::Face>(face), blockColor);
								blockColors = Cube::getColors4WithDefaultShade(static_cast<Cube::Face>(face), blockColor);
							}
							
							//auto blockColors = Cube::getColors4(static_cast<Cube::Face>(face), glm::vec4(0, 1, 0, 1));
							colors.insert(colors.end(), blockColors.begin(), blockColors.end());

							auto blockIndices = Cube::getIndices(static_cast<Cube::Face>(face), indicesOffsetPerBlock);
							indicesOffsetPerBlock += (blockVertices.size() / 3);
							indices.insert(indices.end(), blockIndices.begin(), blockIndices.end());
						}
					}
				}
			}
		}


		/*
		for (auto block : chunkSection->blocks)
		{
			//std::cout << "[ChunkMeshGenerator] -> Generating for block at (" << block->worldCoordinate.x << ", " << block->worldCoordinate.y << ", " << block->worldCoordinate.z << ")" << std::endl;
			// Check block id
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
				
				// After checking adjacent, check near by

				// Shadow weight for each vertex point of block. Weight gets added by 1 whenever other opaque blocks touches the vertex point.
				std::vector<unsigned int> shadowWeight;
				shadowWeight.resize(15, 0);

					//			top biew
					//			+z
					//	below			above
					//		0 7 6	8 15 14
					//+x		1 - 5   9  + 13		 -x
					//		2 3 4  10 11 12

					//			-z

				// Below first
				{
					const int belowY = worldPos.y - 1;
					if (belowY >= 0)
					{
						int block0 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, belowY, worldPos.z - 1);
						if (block0 == 1)
						{
							shadowWeight.at(0) += 1;
						}

						int block1 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, belowY, worldPos.z);
						if (block1 == 1)
						{
							shadowWeight.at(1) += 1;
						}

						int block2 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x + 1, belowY, worldPos.z + 1);
						if (block2 == 1)
						{
							shadowWeight.at(2) += 1;
						}

						int block3 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, belowY, worldPos.z + 1);
						if (block3 == 1)
						{
							shadowWeight.at(3) += 1;
						}

						int block4 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, belowY, worldPos.z + 1);
						if (block4 == 1)
						{
							shadowWeight.at(4) += 1;
						}

						int block5 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, belowY, worldPos.z);
						if (block5 == 1)
						{
							shadowWeight.at(5) += 1;
						}

						int block6 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x - 1, belowY, worldPos.z - 1);
						if (block6 == 1)
						{
							shadowWeight.at(6) += 1;
						}

						int block7 = chunkMap->isBlockAtWorldXYZOpaque(worldPos.x, belowY, worldPos.z - 1);
						if (block7 == 1)
						{
							shadowWeight.at(7) += 1;
						}
					}
				}

				// Then, above
				{
					const int aboveY = worldPos.y + 1;
				}

				// Check face
				if (face == Cube::Face::NONE)
				{
					// Skip if it's surrounded by blocks
					continue;
				}
				else
				{
					auto worldPosition = block->getWorldPosition();

					auto blockVertices = Cube::getVertices(static_cast<Cube::Face>(face), worldPosition);
					vertices.insert(vertices.end(), blockVertices.begin(), blockVertices.end());

					auto blockNormals = Cube::getNormals(static_cast<Cube::Face>(face), worldPosition);
					normals.insert(normals.end(), blockNormals.begin(), blockNormals.end());

					// temporary function for fake lighting. 
					//auto blockColors = Cube::getColors3(static_cast<Cube::Face>(face), block->color);
					auto blockColors = Cube::getColors4WithShade(static_cast<Cube::Face>(face), glm::vec4(block->getColor(), 1.0f));
					//auto blockColors = Cube::getColors4(static_cast<Cube::Face>(face), glm::vec4(0, 1, 0, 1));
					colors.insert(colors.end(), blockColors.begin(), blockColors.end());

					auto blockIndices = Cube::getIndices(static_cast<Cube::Face>(face), indicesOffsetPerBlock);
					indicesOffsetPerBlock += (blockVertices.size() / 3);
					indices.insert(indices.end(), blockIndices.begin(), blockIndices.end());
				}
			}
		}
		*/
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

void Voxel::ChunkMeshGenerator::generateAllChunkMesh(ChunkLoader* chunkLoader, ChunkMap* chunkMap)
{
	if (chunkLoader == nullptr) return;
	if (chunkMap == nullptr) return;

	// size
	auto chunkListXSize = chunkLoader->activeChunks.size();
	auto chunkListZSize = chunkLoader->activeChunks.back().size();
	
	const int maxX = static_cast<int>(chunkListXSize) - 1;
	const int maxZ = static_cast<int>(chunkListZSize) - 1;

	//std::cout << "[ChunkMeshGenerator] Generating mesh..." << std::endl;

	//int toalVertices = 0;

	// Iterate active chunk in X Axis O(Render distance)
	for (auto chunkListX : chunkLoader->activeChunks)
	{
		// Iterate active chunk in Z axis O(Render distnace)
		for (auto chunk : chunkListX)
		{
			if (chunk != nullptr)
			{
				generateSingleChunkMesh(chunk, chunkMap);

				//toalVertices += chunk->chunkMesh->getVerticesSize();
			}
		}
	}
	//std::cout << "[ChunkMeshGenerator] Total vertices " << toalVertices <<  std::endl;
	//std::cout << "[ChunkMeshGenerator] Done. " << std::endl;
}

void Voxel::ChunkMeshGenerator::generateNewChunkMesh(ChunkLoader * chunkLoader, ChunkMap * chunkMap)
{
	if (chunkLoader == nullptr) return;
	if (chunkMap == nullptr) return;

	for (auto chunkListX : chunkLoader->activeChunks)
	{
		// Iterate active chunk in Z axis O(Render distnace)
		for (auto chunk : chunkListX)
		{
			if (chunk != nullptr)
			{
				if (chunk->chunkMesh == nullptr)
				{
					//std::cout << "[ChunkMeshGenerator] Generate mesh for new active chunk (" << chunk->position.x << ", " << chunk->position.z << ")" << std::endl;
					generateSingleChunkMesh(chunk, chunkMap);
				}
			}
		}
	}
}
