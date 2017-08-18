#include "ChunkMap.h"
#include <Chunk.h>
#include <Block.h>
#include <ChunkSection.h>
#include <ChunkUtil.h>
#include <iostream>
#include <Utility.h>

using namespace Voxel;

Voxel::ChunkMap::~ChunkMap()
{
	clear();
}

void Voxel::ChunkMap::initSpawnChunk()
{
	//std::cout << "[ChunkMap] Creating map..." << std::endl;
	
	//std::cout << "[ChunkMap] Initializing spawn chunks.." << std::endl;
	
	// spawn chunk is always loaded.
	//glm::ivec2 spawnChunkMinPos = glm::ivec2(-SPAWN_CHUNK_DISTANCE);
	//glm::ivec2 spawnChunkMaxPos = glm::ivec2(SPAWN_CHUNK_DISTANCE);

	/*
	Chunk* newChunk = Chunk::create(0, 0);
	map.emplace(glm::ivec2(0, 0), newChunk);
	Chunk* newChunk1 = Chunk::create(0, 1);
	map.emplace(glm::ivec2(0, 1), newChunk1);
	Chunk* newChunk2 = Chunk::create(1, 0);
	map.emplace(glm::ivec2(1, 0), newChunk2);
	Chunk* newChunk3 = Chunk::create(1, 1);
	map.emplace(glm::ivec2(1, 1), newChunk3);

	return;
	*/

	int spawnX = -(Constant::SPAWN_CHUNK_DISTANCE - 1);
	int spawnMaxX = (Constant::SPAWN_CHUNK_DISTANCE * 2) - 1 + spawnX;
	int spawnMaxZ = spawnMaxX;
	int spawnZ = spawnX;

	// add spawn chunk. Todo: If this isn't a first initialization, read from map file
	for (int x = spawnX; x < spawnMaxX; x++)
	{
		for (int z = spawnZ; z < spawnMaxZ; z++)
		{
			//std::cout << "[ChunkMap] Adding (" << x << ", " << z << ") as spawn chunk." << std::endl;

			// Add to LUT
			chunkLUT.emplace(glm::ivec2(x, z));

			// Add new spawn chunk
			Chunk* newChunk = Chunk::create(x, z);
			map.emplace(glm::ivec2(x, z), newChunk);
		}
	}

	// Then add chunks near player.
	// Todo: get render distance from setting
	int renderDistnace = 4;

}

void Voxel::ChunkMap::initChunkNearPlayer(const glm::vec3 & playerPosition, const int renderDistance)
{
	// Get chunk X and Z where player is standing
	int chunkX = static_cast<int>(playerPosition.x) / Constant::CHUNK_SECTION_WIDTH;
	int chunkZ = static_cast<int>(playerPosition.z) / Constant::CHUNK_SECTION_LENGTH;

	//std::cout << "[ChunkMap] Initializing chunk near player at (" << chunkX << ", " << chunkZ << ")" << std::endl;

	// excluding the chunk where player stands
	auto rdFromCenter = renderDistance - 1;

	int minX = chunkX - rdFromCenter;
	int maxX = chunkX + rdFromCenter;
	int minZ = chunkZ - rdFromCenter;
	int maxZ = chunkZ + rdFromCenter;

	for (int x = minX; x <= maxX; x++)
	{
		for (int z = minZ; z <= maxZ; z++)
		{
			auto coordinate = glm::ivec2(x, z);
			if (chunkLUT.find(coordinate) == chunkLUT.end())
			{
				// new chunk
				//std::cout << "[ChunkMap] Adding (" << x << ", " << z << ") chunk." << std::endl;
				Chunk* newChunk = Chunk::create(x, z);
				map.emplace(coordinate, newChunk);

				// Add to LUt
				chunkLUT.emplace(coordinate);
			}
			else
			{
				// Chunk is already loaded on map
				continue;
			}
		}
	}
}

void ChunkMap::clear()
{
	for (auto e : map)
	{
		if (e.second)
		{
			delete e.second;
		}
	}

	map.clear();
}

ChunkUnorderedMap & Voxel::ChunkMap::getMapRef()
{
	return map;
}

bool Voxel::ChunkMap::hasChunkAtXZ(int x, int z)
{
	auto v2 = glm::ivec2(x, z);
	return chunkLUT.find(v2) != chunkLUT.end();
}

Chunk * Voxel::ChunkMap::getChunkAtXZ(int x, int z)
{
	auto find_it = map.find(glm::ivec2(x, z));
	if (find_it == map.end())
	{
		return nullptr;
	}
	else
	{
		return find_it->second;
	}
}

void Voxel::ChunkMap::generateChunk(const int x, const int z)
{
	// for sake, just check one more time
	if (!hasChunkAtXZ(x, z))
	{
		Chunk* newChunk = Chunk::create(x, z);
		map.emplace(glm::ivec2(x, z), newChunk);

		// Add to LUt
		chunkLUT.emplace(glm::ivec2(x, z));
	}
}

unsigned int Voxel::ChunkMap::getSize()
{
	return map.size();
}

Block * Voxel::ChunkMap::getBlockAtWorldXYZ(int x, int y, int z)
{
	int chunkX = x / Constant::CHUNK_SECTION_WIDTH;
	int localX = x % Constant::CHUNK_SECTION_WIDTH;

	if (localX < 0)
	{
		localX += Constant::CHUNK_SECTION_WIDTH;
		chunkX -= 1;
	}
	else if (localX >= Constant::CHUNK_SECTION_WIDTH)
	{
		localX -= Constant::CHUNK_SECTION_WIDTH;
		chunkX += 1;
	}

	int chunkY = y / Constant::CHUNK_SECTION_HEIGHT;
	int localY = y % Constant::CHUNK_SECTION_HEIGHT;

	if (chunkY == 0)
	{
		if (localY < 0)
		{
			// There is no chunk lower thak 0 
			return nullptr;
		}
		else if (localY >= Constant::CHUNK_SECTION_HEIGHT)
		{
			localY -= Constant::CHUNK_SECTION_HEIGHT;
			chunkY += 1;
		}
	}
	else
	{
		if (localY < 0)
		{
			localY += Constant::CHUNK_SECTION_HEIGHT;
			chunkY -= 1;
		}
		else if (localY >= Constant::CHUNK_SECTION_HEIGHT)
		{
			localY -= Constant::CHUNK_SECTION_HEIGHT;
			chunkY += 1;
		}
	}

	int chunkZ = z / Constant::CHUNK_SECTION_LENGTH;
	int localZ = z % Constant::CHUNK_SECTION_LENGTH;

	if (localZ < 0)
	{
		localZ += Constant::CHUNK_SECTION_LENGTH;
		chunkZ -= 1;
	}
	else if (localZ >= Constant::CHUNK_SECTION_LENGTH)
	{
		localZ -= Constant::CHUNK_SECTION_LENGTH;
		chunkZ += 1;
	}

	auto find_it = map.find(glm::ivec2(chunkX, chunkZ));
	if (find_it == map.end())
	{
		// Chunk hasn't generated yet. 
		// Todo: generate new chunk? for block query that is out of render distance?
		return nullptr;
	}
	else
	{
		// target chunk
		auto chunk = find_it->second;
		if (chunk->isActive())
		{
			// target chunk section
			auto chunkSection = chunk->getChunkSectionByY(chunkY);
			if (chunkSection)
			{
				// return block
				return chunkSection->getBlockAt(localX, localY, localZ);
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			// Can't access block that is in inactive chunk
			return nullptr;
		}
	}
}

bool Voxel::ChunkMap::attempChunkLoad(int x, int z)
{
	return false;
}

void Voxel::ChunkMap::raycast(const glm::vec3 & rayStart, const glm::vec3 & rayEnd)
{
	std::cout << "RayCasting" << std::endl;
	std::cout << "rayStart = " << rayStart.x << ", " << rayStart.y << ", " << rayStart.z << ")" << std::endl;
	std::cout << "rayEnd = " << rayEnd.x << ", " << rayEnd.y << ", " << rayEnd.z << ")" << std::endl;
	// From rayStart to rayEnd, visit all blocks.
	// Then find the closest block that hits
	auto start = rayStart;
	start.x = Utility::Math::fastFloor(start.x);
	start.y = Utility::Math::fastFloor(start.y);
	start.z = Utility::Math::fastFloor(start.z);

	auto end = rayEnd;
	end.x = Utility::Math::fastFloor(end.x);
	end.y = Utility::Math::fastFloor(end.y);
	end.z = Utility::Math::fastFloor(end.z);

	std::cout << "start = " << start.x << ", " << start.y << ", " << start.z << ")" << std::endl;
	std::cout << "end = " << end.x << ", " << end.y << ", " << end.z << ")" << std::endl;

	float stepValue = 0.1f;

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

	int stepLimit = 500;

	glm::ivec3 startBlockPos = glm::ivec3(rayStart);
	glm::ivec3 curBlockPos = startBlockPos;

	std::cout << "start block (" << startBlockPos.x << ", " << startBlockPos.y << ", " << startBlockPos.z << ")" << std::endl;

	do
	{
		std::cout << "g (" << gx << ", " << gy << ", " << gz << ")" << std::endl;

		auto visitingBlockPos = glm::ivec3(gx, gy, gz);

		if (visitingBlockPos != curBlockPos)
		{
			curBlockPos = visitingBlockPos;

			std::cout << "cur block (" << curBlockPos.x << ", " << curBlockPos.y << ", " << curBlockPos.z << ")" << std::endl;
			Block* curBlock = getBlockAtWorldXYZ(curBlockPos.x, curBlockPos.y, curBlockPos.z);

			if (curBlock->isEmpty() == false)
			{
				// raycasted block not empty. 
				if (curBlockPos != startBlockPos)
				{
					std::cout << "First block (" << curBlockPos.x << ", " << curBlockPos.y << ", " << curBlockPos.z << ")" << std::endl;
					break;
				}
			}
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

Block* Voxel::ChunkMap::raycastBlock(const glm::vec3& playerPosition, const glm::vec3& playerDirection, const float playerRange)
{
	//std::cout << "RayCasting" << std::endl;

	//std::cout << "player pos = " << playerPosition.x << ", " << playerPosition.y << ", " << playerPosition.z << ")" << std::endl;
	//std::cout << "player dir = " << playerDirection.x << ", " << playerDirection.y << ", " << playerDirection.z << ")" << std::endl;

	auto rayStart = playerPosition;
	auto rayEnd = playerPosition + (playerDirection * playerRange);

	//std::cout << "rayStart = " << rayStart.x << ", " << rayStart.y << ", " << rayStart.z << ")" << std::endl;
	//std::cout << "rayEnd = " << rayEnd.x << ", " << rayEnd.y << ", " << rayEnd.z << ")" << std::endl;

	glm::vec3 dirVec = rayEnd - rayStart;

	float div = 200.0f;
	glm::vec3 step = dirVec / div;

	//std::cout << "step = " << step.x << ", " << step.y << ", " << step.z << ")" << std::endl;

	int threshold = 200;

	glm::vec3 curRayPoint = rayStart;

	glm::ivec3 startBlockPos = glm::ivec3(Utility::Math::fastFloor(rayStart.x), Utility::Math::fastFloor(rayStart.y), Utility::Math::fastFloor(rayStart.z));
	glm::ivec3 curBlockPos = startBlockPos;

	//std::cout << "start block (" << startBlockPos.x << ", " << startBlockPos.y << ", " << startBlockPos.z << ")" << std::endl;

	while (threshold >= 0 || rayEnd != rayStart)
	{
		curRayPoint += step;

		if (glm::distance(curRayPoint, rayStart) >playerRange)
		{
			return nullptr;
		}
		//std::cout << "visiting (" << curRayPoint.x << ", " << curRayPoint.y << ", " << curRayPoint.z << ")" << std::endl;

		auto visitingBlockPos = glm::ivec3(Utility::Math::fastFloor(curRayPoint.x), Utility::Math::fastFloor(curRayPoint.y), Utility::Math::fastFloor(curRayPoint.z));

		if (visitingBlockPos != curBlockPos)
		{
			curBlockPos = visitingBlockPos;

			//std::cout << "cur block (" << curBlockPos.x << ", " << curBlockPos.y << ", " << curBlockPos.z << ")" << std::endl;
			Block* curBlock = getBlockAtWorldXYZ(curBlockPos.x, curBlockPos.y, curBlockPos.z);

			if (curBlock)
			{
				if (curBlock->isEmpty() == false)
				{
					// raycasted block not empty. 
					if (curBlockPos != startBlockPos)
					{
						//std::cout << "Block hit (" << curBlockPos.x << ", " << curBlockPos.y << ", " << curBlockPos.z << ")" << std::endl;
						return curBlock;
					}
				}
			}
			else
			{
				return nullptr;
			}
		}


		threshold--;
	}

	return nullptr;
}