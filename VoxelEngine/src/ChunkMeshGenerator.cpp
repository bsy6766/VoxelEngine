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
	std::vector<float> vertices;
	std::vector<float> colors;
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
		auto chunkSectionStart = Utility::Time::now();
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

					auto blockIndices = Cube::getIndices(static_cast<Cube::Face>(face), indicesOffsetPerBlock);
					indicesOffsetPerBlock += (blockVertices.size() / 3);
					for (auto index : blockIndices)
					{
						indices.push_back(index);
					}
				}
			}
		}
		auto chunkSectionEnd = Utility::Time::now();
		//std::cout << "[ChunkMeshGenerator] -> Chunk section Elapsed time: " << Utility::Time::toMilliSecondString(chunkSectionStart, chunkSectionEnd) << std::endl;

		//std::cout << "[ChunkMeshGenerator] -> Done." << std::endl;
	}

	//std::cout << "[ChunkMeshGenerator] finished building mesh (" << chunk->getPosition().x << ", " << chunk->getPosition().z << ")" << std::endl;

	//auto chunkEnd = Utility::Time::now();
	//std::cout << "[ChunkMeshGenerator] -> Chunk Elapsed time: " << Utility::Time::toMilliSecondString(chunkStart, chunkEnd) << std::endl;

	// init chunkMesh
	//auto newChunkMesh = new ChunkMesh();
	//newChunkMesh->initBuffer(vertices, colors, indices);
	//newChunkMesh->initOpenGLObjects();
	//chunk->chunkMesh = newChunkMesh;

	chunk->chunkMesh->initBuffer(vertices, colors, indices);

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
