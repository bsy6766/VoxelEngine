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

using namespace Voxel;

void Voxel::ChunkMeshGenerator::generateChunkMesh(ChunkLoader* chunkLoader, ChunkMap* chunkMap)
{
	if (chunkLoader == nullptr) return;

	// size
	auto chunkListXSize = chunkLoader->activeChunks.size();
	auto chunkListZSize = chunkLoader->activeChunks.back().size();

	int x = 0;
	int z = 0;

	const int maxX = static_cast<int>(chunkListXSize) - 1;
	const int maxZ = static_cast<int>(chunkListZSize) - 1;

	std::cout << "[ChunkMeshGenerator] Generating mesh..." << std::endl;

	int toalVerticies = 0;

	// Iterate active chunk in X Axis O(Render distance)
	for (auto chunkListX : chunkLoader->activeChunks)
	{
		// Iterate active chunk in Z axis O(Render distnace)
		for (auto chunk : chunkListX)
		{
			// Check if chunk is on edge
			/*
			bool edgeMinX = (x == 0);
			bool edgeMaxX = (x == maxX);
			bool edgeMinZ = (z == 0);
			bool edgeMaxZ = (z == maxZ);
			*/

			std::vector<float> verticeis;
			std::vector<float> colors;
			std::vector<unsigned int> indicies;

			std::cout << "[ChunkMeshGenerator] -> Chunk (" << chunk->position.x << ", " << chunk->position.y << ", " << chunk->position.z << ")" << std::endl;
			std::cout << "[ChunkMeshGenerator] -> Total chunk sections: " << chunk->chunkSections.size() << std::endl;

			auto chunkStart = Utility::Time::now();

			// Iterate all chunk sections O(16)
			int indiciesOffsetPerBlock = 0;
			for (auto chunkSection : chunk->chunkSections)
			{
				std::cout << "[ChunkMeshGenerator] -> Generating for chunk section at (" << chunkSection->position.x << ", " << chunkSection->position.y << ", " << chunkSection->position.z << ")" << std::endl;
				
				// Iterate all blocks. O(4096)
				auto chunkSectionStart = Utility::Time::now();
				for (auto block : chunkSection->blocks)
				{
					//std::cout << "[ChunkMeshGenerator] -> Generating for block at (" << block->worldCoordinate.x << ", " << block->worldCoordinate.y << ", " << block->worldCoordinate.z << ")" << std::endl;
					// Check block id
					if (block->isEmpty())
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
						auto localPos = block->localCoordinate;

						// Get adjacent block and check if it's transparent or not
						// Up
						auto blockUp = chunkMap->getBlockAtWorldXYZ(worldPos.x, worldPos.y + 1, worldPos.z);
						if (blockUp)
						{
							if (blockUp->isTransparent())
							{
								face |= Cube::Face::TOP;
							}
							// Else, other block is not transparent, can ignore top face
						}
						else
						{
							// there is no block exists above. 
							face |= Cube::Face::TOP;
						}

						// Down
						auto blockDown = chunkMap->getBlockAtWorldXYZ(worldPos.x, worldPos.y - 1, worldPos.z);
						if (blockDown)
						{
							if (blockDown->isTransparent())
							{
								face |= Cube::Face::BOTTOM;
							}
						}
						else
						{
							face |= Cube::Face::BOTTOM;
						}

						// Left
						auto blockLeft = chunkMap->getBlockAtWorldXYZ(worldPos.x - 1, worldPos.y, worldPos.z);
						if (blockLeft)
						{
							if (blockLeft->isTransparent())
							{
								face |= Cube::Face::LEFT;
							}
						}
						else
						{
							face |= Cube::Face::LEFT;
						}

						// Left
						auto blockRight = chunkMap->getBlockAtWorldXYZ(worldPos.x + 1, worldPos.y, worldPos.z);
						if (blockRight)
						{
							if (blockRight->isTransparent())
							{
								face |= Cube::Face::RIGHT;
							}
						}
						else
						{
							face |= Cube::Face::RIGHT;
						}

						// Front
						auto blockFront = chunkMap->getBlockAtWorldXYZ(worldPos.x, worldPos.y, worldPos.z - 1);
						if (blockFront)
						{
							if (blockFront->isTransparent())
							{
								face |= Cube::Face::FRONT;
							}
						}
						else
						{
							face |= Cube::Face::FRONT;
						}

						// Back
						auto blockBack = chunkMap->getBlockAtWorldXYZ(worldPos.x, worldPos.y, worldPos.z + 1);
						if (blockBack)
						{
							if (blockBack->isTransparent())
							{
								face |= Cube::Face::BACK;
							}
						}
						else
						{
							face |= Cube::Face::BACK;
						}

						// Check face
						if (face == Cube::Face::NONE)
						{
							// Skip if it's surrounded by blocks
							continue;
						}
						else
						{
							auto blockVerticies = Cube::getVerticies(static_cast<Cube::Face>(face));
							for (int i = 0; i < blockVerticies.size(); i += 3)
							{
								auto vertex = glm::vec3(blockVerticies.at(i), blockVerticies.at(i + 1), blockVerticies.at(i + 2));
								vertex += block->worldPosition;
								verticeis.push_back(vertex.x);
								verticeis.push_back(vertex.y);
								verticeis.push_back(vertex.z);
							}
							/*
							for (auto vertex : blockVerticies)
							{
								// For cube, all we need is translation to correct position
								verticeis.push_back(vertex);
							}
							*/

							int triangleCount = blockVerticies.size() / 3;
							for (int i = 0; i < triangleCount; i++)
							{
								colors.push_back(block->color.x);
								colors.push_back(block->color.y);
								colors.push_back(block->color.z);
							}

							auto blockIndicies = Cube::getIndicies(static_cast<Cube::Face>(face), indiciesOffsetPerBlock);
							indiciesOffsetPerBlock += (blockVerticies.size() / 3);
							for (auto index : blockIndicies)
							{
								indicies.push_back(index);
							}
						}
					}
				}
				auto chunkSectionEnd = Utility::Time::now();
				
				std::cout << "[ChunkMeshGenerator] -> Chunk section Elapsed time: " << Utility::Time::toMilliSecondString(chunkSectionStart, chunkSectionEnd) << std::endl;

				std::cout << "[ChunkMeshGenerator] -> Done." << std::endl;
			}

			auto chunkEnd = Utility::Time::now();
			std::cout << "[ChunkMeshGenerator] -> Chunk Elapsed time: " << Utility::Time::toMilliSecondString(chunkStart, chunkEnd) << std::endl;

			// Increment counter for z axis
			z++;

			// init chunkMesh
			auto newChunkMesh = new ChunkMesh();
			/*

			auto temp = Cube::getVerticies(Cube::Face::ALL);

			if (temp.size() == verticeis.size())
			{
				for (int i = 0; i < temp.size(); i++)
				{
					if (temp.at(i) != verticeis.at(i))
					{
						std::cout << "!" << std::endl;
					}
				}
			}


			auto temp2 = Cube::getIndicies(Cube::Face::ALL);
			if (temp2.size() == indicies.size())
			{
				for (int i = 0; i < temp2.size(); i++)
				{
					if (temp2.at(i) != indicies.at(i))
					{
						std::cout << "!" << std::endl;
					}
				}
			}
			*/
			newChunkMesh->initBuffer(verticeis, colors, indicies);
			//newChunkMesh->initMatrix(positions);
			newChunkMesh->initOpenGLObjects();
			chunk->chunkMesh = newChunkMesh;
			toalVerticies += verticeis.size();

			std::cout << "[ChunkMeshGenerator] -> Total verticies: " << verticeis.size() << std::endl;
		}

		// Increment counter for x axis
		x++;
		// Reset z counter for another iteration
		z = 0;
	}
	std::cout << "[ChunkMeshGenerator] Total verticies " << toalVerticies <<  std::endl;
	std::cout << "[ChunkMeshGenerator] Done. " << std::endl;
}
