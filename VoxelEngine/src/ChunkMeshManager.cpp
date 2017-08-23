#include "ChunkMeshManager.h"
#include <iostream>
#include <ChunkMap.h>
#include <Chunk.h>
#include <ChunkUtil.h>
#include <ChunkSection.h>
#include <ChunkMeshGenerator.h>
#include <ChunkMesh.h>

using namespace Voxel;

ChunkMeshManager::ChunkMeshManager()
{
}

ChunkMeshManager::~ChunkMeshManager()
{
}

void Voxel::ChunkMeshManager::addChunkCoordinate(const glm::ivec2 & coordinate)
{
	{
		// Scope lock
		std::unique_lock<std::mutex> lock(queueMutex);
		chunkQueue.push_back(coordinate);
		cv.notify_one();
		std::cout << "Main thread added (" << coordinate.x << ", " << coordinate.y << ")" << std::endl;
	}
}

void Voxel::ChunkMeshManager::buildMesh(ChunkMap* map, ChunkMeshGenerator* chunkMeshGenerator)
{
	// loop while it's running
	std::cout << "Thraed #" << std::this_thread::get_id() << " started to build mesh " << std::endl;
	while (running)
	{
		{
			// Scope lock
			std::unique_lock<std::mutex> lock(queueMutex);
			// wait if queue is empty and running
			while (running && chunkQueue.empty())
			{
				cv.wait(lock);
			}

			if (running == false)
			{
				// quit
				break;
			}

			std::cout << "Thraed #" << std::this_thread::get_id() << " has (" << chunkQueue.front().x << ", " << chunkQueue.front().y << ")" << std::endl;
			glm::ivec2 coordinate = chunkQueue.front();
			chunkQueue.pop_front();

			// For now, assume chunk is all loaded.
			if (map)
			{
				bool hasChunk = map->hasChunkAtXZ(coordinate.x, coordinate.y);
				if (hasChunk)
				{
					std::cout << "Map has chunk (" << coordinate.x << ", " << coordinate.y << std::endl;
					auto chunk = map->getChunkAtXZ(coordinate.x, coordinate.y);

					if (chunk)
					{
						auto mesh = chunk->getMesh();
						if (mesh)
						{
							if (!mesh->hasBufferToLoad())
							{
								// Mesh doesn't have buffer to load.
								chunkMeshGenerator->generateSingleChunkMesh(chunk, map);
							}
						}
					}
				}
			}
		}
	}
}

void Voxel::ChunkMeshManager::createThread(ChunkMap* map, ChunkMeshGenerator* chunkMeshGenerator)
{
	if (running)
	{
		meshBuilderThread = std::thread(&ChunkMeshManager::buildMesh, this, map, chunkMeshGenerator);
	}
}

void Voxel::ChunkMeshManager::run()
{
	running.store(true);
}

void Voxel::ChunkMeshManager::stop()
{
	running.store(false);
	cv.notify_all();
}

void Voxel::ChunkMeshManager::joinThread()
{
	{
		//std::cout << "Waiting to thread join..." << std::endl;
		// Scope lock
		std::unique_lock<std::mutex> lock(queueMutex);
		if (meshBuilderThread.joinable())
		{
			meshBuilderThread.join();
			std::cout << "joining thread #" << meshBuilderThread.get_id() << std::endl;
		}
	}
}