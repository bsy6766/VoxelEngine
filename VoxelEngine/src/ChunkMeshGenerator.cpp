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

void Voxel::ChunkMeshGenerator::generateSingleChunkMesh(Chunk * chunk, ChunkMap * chunkMap)
{
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

		std::vector<float> vertices;
		std::vector<float> colors;
		std::vector<unsigned int> indices;

		// Iterate all blocks. O(4096)
		//auto chunkSectionStart = Utility::Time::now();
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
					auto blockVertices = Cube::getVertices(static_cast<Cube::Face>(face), block->getWorldPosition());
					for (auto vertex : blockVertices)
					{
						vertices.push_back(vertex);
					}

					// temporary function for fake lighting. 
					//auto blockColors = Cube::getColors3(static_cast<Cube::Face>(face), block->color);
					auto blockColors = Cube::getColors4(static_cast<Cube::Face>(face), glm::vec4(block->getColor(), 1.0f));
					//auto blockColors = Cube::getColors4(static_cast<Cube::Face>(face), glm::vec4(0, 1, 0, 1));
					for (auto color : blockColors)
					{
						colors.push_back(color);
					}
					/*
					int triangleCount = blockVertices.size() / 3;
					for (int i = 0; i < triangleCount; i++)
					{
						colors.push_back(block->color.x);
						colors.push_back(block->color.y);
						colors.push_back(block->color.z);
					}
					*/

					auto blockIndices = Cube::getIndices(static_cast<Cube::Face>(face), indicesOffsetPerBlock);
					indicesOffsetPerBlock += (blockVertices.size() / 3);
					for (auto index : blockIndices)
					{
						indices.push_back(index);
					}
				}
			}
		}

		indicesOffsetPerBlock = 0;

		if (vertices.size() == 0 || colors.size() == 0 || indices.size() == 0)
		{
			chunkSection->setMesh(nullptr);
			chunkSection->setVisibility(false);
		}
		else
		{
			// init chunkMesh
			auto newChunkMesh = new ChunkMesh();
			newChunkMesh->initBuffer(vertices, colors, indices);
			//newChunkMesh->initOpenGLObjects();
			chunkSection->setMesh(newChunkMesh);

		}


		//auto chunkSectionEnd = Utility::Time::now();

		//std::cout << "[ChunkMeshGenerator] -> Chunk section Elapsed time: " << Utility::Time::toMilliSecondString(chunkSectionStart, chunkSectionEnd) << std::endl;

		//std::cout << "[ChunkMeshGenerator] -> Done." << std::endl;
	}

	//auto chunkEnd = Utility::Time::now();
	//std::cout << "[ChunkMeshGenerator] -> Chunk Elapsed time: " << Utility::Time::toMilliSecondString(chunkStart, chunkEnd) << std::endl;


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

void Voxel::ChunkMeshGenerator::updateMesh(ChunkLoader * chunkLoader, ChunkMap * chunkMap, const glm::ivec2 & mod)
{
	// This function updates mesh for any chunk section that need to rebuild the mesh.
	if (chunkLoader == nullptr) return;
	if (chunkMap == nullptr) return;

	for (auto chunkListX : chunkLoader->activeChunks)
	{
		// Iterate active chunk in Z axis O(Render distnace)
		for (auto chunk : chunkListX)
		{
			if (chunk != nullptr)
			{
				if (chunk->hasChunkSectionNeedMesh())
				{
					//std::cout << "[ChunkMeshGenerator] Generate mesh for new active chunk (" << chunk->position.x << ", " << chunk->position.z << ")" << std::endl;
					generateSingleChunkMesh(chunk, chunkMap);
				}
			}
		}
	}
}
