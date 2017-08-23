#include "ChunkLoader.h"
#include "ChunkMap.h"
#include "Chunk.h"
#include <ChunkSection.h>
#include <ChunkUtil.h>
#include <iostream>
#include <Camera.h>
#include <Frustum.h>
#include <Utility.h>

using namespace Voxel;

Voxel::ChunkLoader::ChunkLoader()
	: manualChunks(nullptr)
{
}

Voxel::ChunkLoader::~ChunkLoader()
{
	if (manualChunks)
	{
		delete manualChunks;
	}
}

void Voxel::ChunkLoader::init(const glm::vec3 & playerPosition, ChunkMap* map, const int renderDistance)
{
	glm::ivec3 pos = glm::ivec3(playerPosition);

	int chunkX = pos.x / Constant::CHUNK_SECTION_WIDTH;
	int chunkY = pos.y / Constant::CHUNK_SECTION_HEIGHT;
	int chunkZ = pos.z / Constant::CHUNK_SECTION_LENGTH;

	if (pos.x < 0)
	{
		chunkX -= 1;
	}

	if (pos.y < 0)
	{
		chunkY -= 1;
	}

	if (pos.z < 0)
	{
		chunkZ -= 1;
	}

	currentChunkPos.x = chunkX;
	currentChunkPos.y = chunkY;
	currentChunkPos.z = chunkZ;
	
	auto rdFromCenter = renderDistance - 1;

	int minX = chunkX - rdFromCenter;
	int maxX = chunkX + rdFromCenter;
	int minZ = chunkZ - rdFromCenter;
	int maxZ = chunkZ + rdFromCenter;

	clear();

	for (int x = minX; x <= maxX; x++)
	{
		activeChunks.push_back(std::list<Chunk*>());

		for (int z = minZ; z <= maxZ; z++)
		{
			//auto coordinate = glm::ivec2(x, z);
			if (map->hasChunkAtXZ(x, z))
			{
				//std::cout << "[ChunkLoader] Loading chunk at (" << x << ", " << z << ")" << std::endl;
				// Get chunk
				activeChunks.back().push_back(map->getChunkAtXZ(x, z));

				// Because we are initializing, set all chunk's chunk section visible to generate mesh.
				activeChunks.back().back()->setAllVisibility(true);
				// And make active
				activeChunks.back().back()->setActive(true);
			}
			else
			{
				// For now, assume chunk loader loads chunks that has already generated
				clear();
				throw std::runtime_error("ChunkLoader tried to iniailize chunk that's hasn't initialized by ChunkMap");
			}
		}
	}
}

bool Voxel::ChunkLoader::update(const glm::vec3 & playerPosition, ChunkMap* map, glm::ivec2& mod)
{
	// This function updates chunk when player move to new chunk. 
	// This function doesn't check in y Axis because moving up and down doen't need any new chunk to update
	glm::ivec3 pos = glm::ivec3(playerPosition);

	int chunkX = pos.x / Constant::CHUNK_SECTION_WIDTH;
	int chunkY = pos.y / Constant::CHUNK_SECTION_HEIGHT;
	int chunkZ = pos.z / Constant::CHUNK_SECTION_LENGTH;

	if (pos.x < 0)
	{
		chunkX -= 1;
	}

	if (pos.y < 0)
	{
		chunkY -= 1;
	}

	if (pos.z < 0)
	{
		chunkZ -= 1;
	}
	
	auto newChunkXZ = glm::ivec2(chunkX, chunkZ);
	auto curChunkPos = glm::ivec2(currentChunkPos.x, currentChunkPos.z);

	if (chunkY != currentChunkPos.y)
	{
		currentChunkPos.y = chunkY;
		std::cout << "[ChunkLoader] Player moved to new y (" << chunkY << ") in chunk at (" << chunkX << ", " << chunkZ << ")" << std::endl;
	}

	if (newChunkXZ != curChunkPos)
	{
		// reset mod
		mod = glm::ivec2(0);

		std::cout << "Player pos (" << playerPosition.x << ", " << playerPosition.z << ")" << std::endl;
		// Player moved to new chunk.
		// normally, player should move more than 1 chunk at a time. Moving more than 1 chunk (16 blocks = 16 meter in scale)
		// means player is cheating or in god mode or whatever. 
		
		// Anyway, first we get how far player moved. In chunk distance.
		// Then find which row and col need to be added based on direction player moved.
		// also find which row and col to pop aswell.
		auto d = newChunkXZ - curChunkPos;
		std::cout << "Player moved to new chunk (" << chunkX << ", " << chunkZ << ") from chunk (" << curChunkPos.x << ", " << curChunkPos.y << ")" << std::endl;
		currentChunkPos.x = newChunkXZ.x;
		currentChunkPos.z = newChunkXZ.y;

		// save mod
		mod.x = d.x;

		// Always modify x first. I think...that.. is.. better....right?

		/*
			2D std::list

											^
											| -x (West)
					List 
						List --------------------------------------
						List --------------------------------------
						List --------------------------------------
	-z (North) <-		List --------------------------------------		-> +z (South)
						List --------------------------------------
						List --------------------------------------
						List --------------------------------------
		
											| +x (east)
											v
		*/

		if (d.x != 0)
		{
			// Moved in x axis
			if (d.x < 0)
			{
				// Moved to west
				std::cout << "Player moved to west" << std::endl;
				// pop last list and push new list on front
				for (auto chunk : activeChunks.back())
				{
					// deactivate front list
					//std::cout << "[ChunkLoader] Unloading chunk at (" << chunk->position.x << ", " << chunk->position.z << ")" << std::endl;
					chunk->setActive(false);
					chunk->releaseAllMeshes();
				}

				// Pop from list. Don't need to worry about chunk instance. ChunkMap will take care.
				activeChunks.pop_back();
				// Now because list on back is gone, the chunks that were connected to that list needs new mesh
				for (auto chunk : activeChunks.back())
				{
					chunk->releaseAllMeshes();
				}

				// Get x from first list and -1 for new list to west (negative x)
				int x = activeChunks.front().front()->position.x - 1;
				// Also get first z in list.
				int zStart = activeChunks.back().front()->position.z;

				// Before we add new list on front, release mesh for new mesh
				for (auto chunk : activeChunks.front())
				{
					chunk->releaseAllMeshes();
				}
				//Them, add new list at the front of the list
				activeChunks.push_front(std::list<Chunk*>());

				// get size and add chunk
				int lastZ = activeChunks.back().size() + zStart;
				for (int z = zStart; z < lastZ; z++)
				{
					// Check if map already has chunk generated
					if (map->hasChunkAtXZ(x, z) == false)
					{
						// If not, generate chunk. 
						// Todo: This need to be optimized. Maybe a thread?
						map->generateChunk(x, z);
					}

					// ATM, we have chunk loaded in map! add chunk.
					//std::cout << "[ChunkLoader] Loading chunk at (" << x << ", " << z << ")" << std::endl;
					activeChunks.front().push_back(map->getChunkAtXZ(x, z));
					activeChunks.front().back()->setActive(true);
				}
			}
			else
			{
				// moved to east
				std::cout << "Player moved to east" << std::endl;
				// Pop front list and  push new list on back
				for (auto chunk : activeChunks.front())
				{
					// deactivate front list
					//std::cout << "[ChunkLoader] Unloading chunk at (" << chunk->position.x << ", " << chunk->position.z << ")" << std::endl;
					chunk->setActive(false);
					chunk->releaseAllMeshes();
				}

				// Pop from list. Don't need to worry about chunk instance. ChunkMap will take care.
				activeChunks.pop_front();
				// Because we pop the front chunks, new front chunks needs new mesh
				for (auto chunk : activeChunks.front())
				{
					chunk->releaseAllMeshes();
				}

				// Get x from last list and +1 for new list to east (positive x)
				int x = activeChunks.back().front()->position.x + 1;
				// Also get first z in list.
				int zStart = activeChunks.back().front()->position.z;

				// Before we add new list of chunk on back, make current list on back to have new mesh
				for (auto chunk : activeChunks.back())
				{
					chunk->releaseAllMeshes();
				}
				// New list at the end of list
				activeChunks.push_back(std::list<Chunk*>());

				// get size and add chunk
				int lastZ = activeChunks.front().size() + zStart;
				for (int z = zStart; z < lastZ; z++)
				{
					// Check if map already has chunk generated
					if (map->hasChunkAtXZ(x, z) == false)
					{
						// If not, generate chunk. 
						// Todo: This need to be optimized. Maybe a thread?
						map->generateChunk(x, z);
					}

					// ATM, we have chunk loaded in map! add chunk.
					//std::cout << "[ChunkLoader] Loading chunk at (" << x << ", " << z << ")" << std::endl;
					activeChunks.back().push_back(map->getChunkAtXZ(x, z));
					activeChunks.back().back()->setActive(true);
				}
			}
		}
		// Else, didn't move in X axis

		if (d.y/*z*/ != 0)
		{
			// Moved in z axis
			if (d.y < 0)
			{
				// Move to north
				std::cout << "Player moved to north" << std::endl;

				// Get z
				int z = activeChunks.front().front()->position.z - 1;

				// iterate through list and pop all last chunks. then again, release mesh the new last chunks to update. Also release mesh on front here
				for (auto& chunksZ : activeChunks)
				{
					//std::cout << "[ChunkLoader] Unloading chunk at (" << chunksZ.back()->position.x << ", " << chunksZ.back()->position.z << ")" << std::endl;
					chunksZ.back()->setActive(false);
					chunksZ.back()->releaseAllMeshes();
					chunksZ.pop_back();
					chunksZ.back()->releaseAllMeshes();
					// front also
					chunksZ.front()->releaseAllMeshes();
				}

				// Separating loop just to separate debug output
				// Todo: merge above and below loop together
				// Then add new chunk on front
				for (auto& chunksZ : activeChunks)
				{
					// Check if map already has chunk generated
					int x = chunksZ.front()->position.x;
					if (map->hasChunkAtXZ(x, z) == false)
					{
						// If not, generate chunk. 
						// Todo: This need to be optimized. Maybe a thread?
						map->generateChunk(x, z);
					}

					// ATM, we have chunk loaded in map! add chunk.
					//std::cout << "[ChunkLoader] Loading chunk at (" << x << ", " << z << ")" << std::endl;
					chunksZ.push_front(map->getChunkAtXZ(x, z));
					chunksZ.front()->setActive(true);
				}
			}
			else
			{
				// Moved to sourth
				std::cout << "Player moved to south" << std::endl;

				// Get z
				int z = activeChunks.front().back()->position.z + 1;

				// iterate through list and pop all last chunks. then again, release mesh the new last chunks to update. Also release mesh on front here
				for (auto& chunksZ : activeChunks)
				{
					//std::cout << "[ChunkLoader] Unloading chunk at (" << chunksZ.front()->position.x << ", " << chunksZ.front()->position.z << ")" << std::endl;
					chunksZ.front()->setActive(false);
					chunksZ.front()->releaseAllMeshes();
					chunksZ.pop_front();
					chunksZ.front()->releaseAllMeshes();
					// front also
					chunksZ.back()->releaseAllMeshes();
				}

				// Separating loop just to separate debug output
				// Todo: merge above and below loop together
				// Then add new chunk on front
				for (auto& chunksZ : activeChunks)
				{
					// Check if map already has chunk generated
					int x = chunksZ.front()->position.x;
					if (map->hasChunkAtXZ(x, z) == false)
					{
						// If not, generate chunk. 
						// Todo: This need to be optimized. Maybe a thread?
						map->generateChunk(x, z);
					}

					// ATM, we have chunk loaded in map! add chunk.
					//std::cout << "[ChunkLoader] Loading chunk at (" << x << ", " << z << ")" << std::endl;
					chunksZ.push_back(map->getChunkAtXZ(x, z));
					chunksZ.back()->setActive(true);
				}
			}
		}
		// Else, didn't move in Z axis
		return true;
	}

	return false;
}

int Voxel::ChunkLoader::findVisibleChunk()
{
	auto curChunkPos = glm::ivec2(currentChunkPos.x, currentChunkPos.z);
	int totalVisibleChunk = 0;

	for (auto x : activeChunks)
	{
		for (auto chunk : x)
		{
			if (chunk != nullptr)
			{
				if (glm::ivec2(chunk->position.x, chunk->position.z) == curChunkPos)
				{
					// if we are checking the chunk that player is standing, make chunk section to be always visible
					ChunkSection* cs = chunk->getChunkSectionByY(currentChunkPos.y);
					if (cs != nullptr)
					{
						cs->setVisibility(true);
					}
				}
				// But also other chunk sections might be visible. add.
				// Check all chunk sections in chunk and mark as visible if so.
				totalVisibleChunk += Camera::mainCamera->getFrustum()->isChunkBorderInFrustum(chunk);
			}
		}
	}

	return totalVisibleChunk;
}
/*
void Voxel::ChunkLoader::raycast(const glm::vec3 & rayStart, const glm::vec3 & rayEnd)
{
	std::cout << "RayCasting" << std::endl;
	std::cout << "rayStart = " << rayStart.x << ", " << rayStart.y << ", " << rayStart.z << ")" << std::endl;
	std::cout << "rayEnd = " << rayEnd.x << ", " << rayEnd.y << ", " << rayEnd.z << ")" << std::endl;
	// From rayStart to rayEnd, visit all blocks.
	// Then find the closest block that hits
	auto start = rayStart;
	//start.x = Utility::Math::fastFloor(start.x);
	//start.y = Utility::Math::fastFloor(start.y);
	//start.z = Utility::Math::fastFloor(start.z);

	auto end = rayEnd;
	//end.x = Utility::Math::fastFloor(end.x);
	//end.y = Utility::Math::fastFloor(end.y);
	//end.z = Utility::Math::fastFloor(end.z);

	std::cout << "start = " << start.x << ", " << start.y << ", " << start.z << ")" << std::endl;
	std::cout << "end = " << end.x << ", " << end.y << ", " << end.z << ")" << std::endl;

	float stepValue = 0.5f;

	float sx = end.x > start.x ? stepValue : end.x < start.x ? -stepValue : 0;
	float sy = end.y > start.y ? stepValue : end.y < start.y ? -stepValue : 0;
	float sz = end.z > start.z ? stepValue : end.z < start.z ? -stepValue : 0;

	std::cout << "sx = " << sx << ", sy = " << sy << ", sz = " << sz << std::endl;
	float gx = start.x;
	float gy = start.y;
	float gz = start.z;

	//Planes for each axis that we will next cross
	float gxp = gx + (end.x > start.x ? stepValue : 0);
	float gyp = gy + (end.y > start.y ? stepValue : 0);
	float gzp = gz + (end.z > start.z ? stepValue : 0);

	//Only used for multiplying up the error margins
	float vx = rayEnd.x == rayStart.x ? stepValue : rayEnd.x - rayStart.x;
	float vy = rayEnd.y == rayStart.y ? stepValue : rayEnd.y - rayStart.y;
	float vz = rayEnd.z == rayStart.z ? stepValue : rayEnd.z - rayStart.z;

	//Error is normalized to vx * vy * vz so we only have to multiply up
	float vxvy = vx * vy;
	float vxvz = vx * vz;
	float vyvz = vy * vz;

	//Error from the next plane accumulators, scaled up by vx*vy*vz
	float errx = (gxp - rayStart.x) * vyvz;
	float erry = (gyp - rayStart.y) * vxvz;
	float errz = (gzp - rayStart.z) * vxvy;

	float derrx = sx * vyvz;
	float derry = sy * vxvz;
	float derrz = sz * vxvy;

	std::cout << "v = " << vx << ", " << vy << ", " << vz << ")" << std::endl;
	std::cout << "step = " << sx << ", " << sy << ", " << sz << ")" << std::endl;

	int stepLimit = 20;

	do
	{
		std::cout << "Visiting (" << gx << ", " << gy << ", " << gz << ")" << std::endl;

		if (gx == end.x && gy == end.y && gz == end.z)
		{
			break;
		}

		float xr = abs(errx);
		float yr = abs(erry);
		float zr = abs(errz);

		std::cout << "err (" << errx << ", " << erry << ", " << errz << ")" << std::endl;

		if (sx != 0 && (sy == 0 || xr < yr) && (sz == 0 || xr < zr)) 
		{
			gx += sx;
			errx += derrx;
		}
		else if (sy != 0 && (sz == 0 || yr < zr)) {
			gy += sy;
			erry += derry;
		}
		else if (sz != 0) {
			gz += sz;
			errz += derrz;
		}


	} while (stepLimit-- > 0);
}

*/
void Voxel::ChunkLoader::render()
{
	//auto start = Utility::Time::now();
	for (auto x : activeChunks)
	{
		for (auto chunk : x)
		{
			if (chunk != nullptr)
			{
				chunk->render();
			}
		}
	}
	//auto end = Utility::Time::now();
	//std::cout << " t = " << Utility::Time::toMicroSecondString(start, end) << std::endl;
}


void ChunkLoader::clear()
{
	for (auto x : activeChunks)
	{
		for (auto z : x)
		{
			if (z != nullptr)
			{
				z->setActive(false);
			}
		}
	}

	activeChunks.clear();
}

