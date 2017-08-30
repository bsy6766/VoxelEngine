#include "ChunkMap.h"
#include <Chunk.h>
#include <ChunkSection.h>
#include <iostream>
#include <Utility.h>
#include <Physics.h>

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

	if (playerPosition.x < 0) chunkX -= 1;
	if (playerPosition.z < 0) chunkZ -= 1;

	//std::cout << "[ChunkMap] Initializing chunk near player at (" << chunkX << ", " << chunkZ << ")" << std::endl;

	// excluding the chunk where player stands
	//auto rdFromCenter = renderDistance - 1;

	int minX = chunkX - renderDistance;
	int maxX = chunkX + renderDistance;
	int minZ = chunkZ - renderDistance;
	int maxZ = chunkZ + renderDistance;

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

bool Voxel::ChunkMap::hasChunkAtXZ(int x, int z)
{
	std::unique_lock<std::mutex> lock(mapMutex);
	auto v2 = glm::ivec2(x, z);
	return chunkLUT.find(v2) != chunkLUT.end();
}

Chunk * Voxel::ChunkMap::getChunkAtXZ(int x, int z)
{
	std::unique_lock<std::mutex> lock(mapMutex);
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

void Voxel::ChunkMap::generateRegion(const glm::ivec2& regionCoordinate)
{
	int chunkX = regionCoordinate.x * Constant::REGION_WIDTH;
	int chunkZ = regionCoordinate.y * Constant::REGION_LENGTH;

	int chunkXLen = chunkX + Constant::REGION_WIDTH;
	int chunkZLen = chunkZ + Constant::REGION_LENGTH;

	for (int x = chunkX; x < chunkXLen; x++)
	{
		for (int z = chunkZ; z < chunkZLen; z++)
		{
			generateChunk(x, z);
			//generateEmptyChunk(x, z);
		}
	}
}

void Voxel::ChunkMap::generateChunk(const int x, const int z)
{
	// for sake, just check one more time
	if (!hasChunkAtXZ(x, z))
	{
		std::unique_lock<std::mutex> lock(mapMutex);
		Chunk* newChunk = Chunk::create(x, z);
		map.emplace(glm::ivec2(x, z), newChunk);

		// Add to LUt
		chunkLUT.emplace(glm::ivec2(x, z));
	}
}

void Voxel::ChunkMap::generateEmptyChunk(const int x, const int z)
{
	// for sake, just check one more time
	if (!hasChunkAtXZ(x, z))
	{
		std::unique_lock<std::mutex> lock(mapMutex);
		Chunk* newChunk = Chunk::createEmpty(x, z);
		map.emplace(glm::ivec2(x, z), newChunk);

		// Add to LUt
		chunkLUT.emplace(glm::ivec2(x, z));
	}
}

unsigned int Voxel::ChunkMap::getSize()
{
	return map.size();
}

void Voxel::ChunkMap::blockWorldCoordinateToLocalAndChunkSectionCoordinate(const glm::ivec3& blockWorldCoordinate, glm::ivec3& blockLocalCoordinate, glm::ivec3& chunkSectionCoordinate)
{
	int x = blockWorldCoordinate.x;
	int y = blockWorldCoordinate.y;
	int z = blockWorldCoordinate.z;

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

	blockLocalCoordinate.x = localX;
	blockLocalCoordinate.y = localY;
	blockLocalCoordinate.z = localZ;

	chunkSectionCoordinate.x = chunkX;
	chunkSectionCoordinate.y = chunkY;
	chunkSectionCoordinate.z = chunkZ;
}

Block * Voxel::ChunkMap::getBlockAtWorldXYZ(int x, int y, int z)
{
	glm::ivec3 blockLocalPos;
	glm::ivec3 chunkSectionPos;

	blockWorldCoordinateToLocalAndChunkSectionCoordinate(glm::ivec3(x, y, z), blockLocalPos, chunkSectionPos);

	bool hasChunk = this->hasChunkAtXZ(chunkSectionPos.x, chunkSectionPos.z);

	if (!hasChunk)
	{
		// There is no chunk generated. 
		return nullptr;
	}
	else
	{
		// target chunk
		auto chunk = this->getChunkAtXZ(chunkSectionPos.x, chunkSectionPos.z);
		if (chunk)
		{
			if (chunk->isActive())
			{
				// target chunk section
				auto chunkSection = chunk->getChunkSectionByY(chunkSectionPos.y);
				if (chunkSection)
				{
					// return block
					return chunkSection->getBlockAt(blockLocalPos.x, blockLocalPos.y, blockLocalPos.z);
				}
				// There is no block in this chunk section = nullptr
			}
			// Can't access block that is in inactive chunk
		}
	}

	return nullptr;
}

int Voxel::ChunkMap::isBlockAtWorldXYZOpaque(const int x, const int y, const int z)
{
	// Retruns 0 if block exists and transparent. 
	// Returns 1 if block exists and opaque
	// Retruns 2 if chunk section doesn't exists
	// Retruns 3 if chunk doesn't exsits.

	glm::ivec3 blockLocalPos;
	glm::ivec3 chunkSectionPos;

	blockWorldCoordinateToLocalAndChunkSectionCoordinate(glm::ivec3(x, y, z), blockLocalPos, chunkSectionPos);

	bool hasChunk = this->hasChunkAtXZ(chunkSectionPos.x, chunkSectionPos.z);

	if (!hasChunk)
	{
		// There is no chunk generated. 
		return 3;
	}
	else
	{
		// target chunk
		auto chunk = this->getChunkAtXZ(chunkSectionPos.x, chunkSectionPos.z);
		if (chunk)
		{
			if (chunk->isActive())
			{
				// target chunk section
				auto chunkSection = chunk->getChunkSectionByY(chunkSectionPos.y);
				if (chunkSection)
				{
					// chunk section exists. return block
					Block* block = chunkSection->getBlockAt(blockLocalPos.x, blockLocalPos.y, blockLocalPos.z);
					if (block)
					{
						if (block->isTransparent())
						{
							return 0;
						}
						else
						{
							return 1;
						}
					}
					else
					{
						// block is air == nullptr
						return 0;
					}
				}
				// There is no block in this chunk section = nullptr
				else
				{
					return 2;
				}
			}
			// Can't access block that is in inactive chunk
		}

		return 3;
	}
}

RayResult Voxel::ChunkMap::raycastBlock(const glm::vec3& playerPosition, const glm::vec3& playerDirection, const float playerRange)
{
	//std::cout << "RayCasting" << std::endl;

	//std::cout << "player pos = " << playerPosition.x << ", " << playerPosition.y << ", " << playerPosition.z << ")" << std::endl;
	//std::cout << "player dir = " << playerDirection.x << ", " << playerDirection.y << ", " << playerDirection.z << ")" << std::endl;

	auto rayStart = playerPosition;
	auto rayEnd = playerPosition + (playerDirection * playerRange);

	//std::cout << "rayStart = " << rayStart.x << ", " << rayStart.y << ", " << rayStart.z << ")" << std::endl;
	//std::cout << "rayEnd = " << rayEnd.x << ", " << rayEnd.y << ", " << rayEnd.z << ")" << std::endl;

	glm::vec3 dirVec = rayEnd - rayStart;

	float div = 1500.0f;
	glm::vec3 step = dirVec / div;

	//std::cout << "step = " << step.x << ", " << step.y << ", " << step.z << ")" << std::endl;

	int threshold = 1500;

	glm::vec3 curRayPoint = rayStart;

	glm::ivec3 startBlockPos = glm::ivec3(Utility::Math::fastFloor(rayStart.x), Utility::Math::fastFloor(rayStart.y), Utility::Math::fastFloor(rayStart.z));
	glm::ivec3 curBlockPos = startBlockPos;

	//std::cout << "start block (" << startBlockPos.x << ", " << startBlockPos.y << ", " << startBlockPos.z << ")" << std::endl;

	RayResult result;
	result.block = nullptr;
	result.face = Cube::Face::NONE;

	while (threshold >= 0)
	{
		curRayPoint += step;

		if (glm::distance(curRayPoint, rayStart) > playerRange)
		{
			return result;
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
						result.block = curBlock;

						// Check which face did ray hit on cube.
						result.face = raycastFace(rayStart, curRayPoint, curBlock->getAABB());

						return result;
					}
				}
			}
		}


		threshold--;
	}

	return result;
}

Cube::Face Voxel::ChunkMap::raycastFace(const glm::vec3 & rayStart, const glm::vec3 & rayEnd, const AABB & blockAABB)
{
	// Check if ray hits each triangle of cube. 

	Cube::Face result = Cube::Face::NONE;
	auto triangles = blockAABB.toTriangles();
	
	float minDist = 1000000.0f;
	unsigned int closestTriangle = 0;

	for (unsigned int i = 0; i < triangles.size(); i++)
	{
		glm::vec3 intersectingPoint;
		int rayResult = raycastTriangle(rayStart, rayEnd, triangles.at(i), intersectingPoint);

		if (rayResult == 1)
		{
			//std::cout << "hit: " << i << std::endl;
			//std::cout << "point: " << Utility::Log::vec3ToStr(intersectingPoint)<< std::endl;
			float dist = glm::abs(glm::distance(intersectingPoint, rayStart));
			if (dist < minDist)
			{
				minDist = dist;
				closestTriangle = i;
			}
		}
	}

	switch (closestTriangle)
	{
	case 0:
	case 1:
		result = Cube::Face::FRONT;
		//std::cout << "FRONT" << std::endl;
		break;
	case 2:
	case 3:
		result = Cube::Face::LEFT;
		//std::cout << "LEFT" << std::endl;
		break;
	case 4:
	case 5:
		result = Cube::Face::BACK;
		//std::cout << "BACK" << std::endl;
		break;
	case 6:
	case 7:
		result = Cube::Face::RIGHT;
		//std::cout << "RIGHT" << std::endl;
		break;
	case 8:
	case 9:
		result = Cube::Face::TOP;
		//std::cout << "TOP" << std::endl;
		break;
	case 10:
	case 11:
		result = Cube::Face::BOTTOM;
		//std::cout << "BOTTOM" << std::endl;
		break;
	default:
		break;
	}
	return result;
}

//    Return: -1 = triangle is degenerate (a segment or point)
//             0 =  disjoint (no intersect)
//             1 =  intersect in unique point I1
//             2 =  are in the same plane
int Voxel::ChunkMap::raycastTriangle(const glm::vec3 & rayStart, const glm::vec3 & rayEnd, const Triangle & tri, glm::vec3 & intersectingPoint)
{
	glm::vec3    u, v, n;              // triangle vectors
	glm::vec3    dir, w0, w;           // ray vectors
	float     r, a, b;              // params to calc ray-plane intersect

	// get triangle edge vectors and plane normal
	u = tri.p2 - tri.p1;
	v = tri.p3 - tri.p1;
	n = glm::cross(u, v);              // cross product
	if (n == glm::vec3(0))             // triangle is degenerate
		return -1;                  // do not deal with this case

	dir = rayEnd - rayStart;              // ray direction vector
	w0 = rayStart - tri.p1;
	a = -glm::dot(n, w0);
	b = glm::dot(n, dir);
	if (fabs(b) <  0.00000001f) {     // ray is  parallel to triangle plane
		if (a == 0)                 // ray lies in triangle plane
			return 2;
		else return 0;              // ray disjoint from plane
	}

	// get intersect point of ray with triangle plane
	r = a / b;
	if (r < 0.0)                    // ray goes away from triangle
		return 0;                   // => no intersect
									// for a segment, also test if (r > 1.0) => no intersect

	intersectingPoint = rayStart + (r * dir);  // intersect point of ray and plane

									// is I inside T?
	float    uu, uv, vv, wu, wv, D;
	uu = glm::dot(u, u);
	uv = glm::dot(u, v);
	vv = glm::dot(v, v);
	w = intersectingPoint - tri.p1;
	wu = glm::dot(w, u);
	wv = glm::dot(w, v);
	D = uv * uv - uu * vv;

	// get and test parametric coords
	float s, t;
	s = (uv * wv - vv * wu) / D;
	if (s < 0.0 || s > 1.0)         // I is outside T
		return 0;
	t = (uv * wu - uu * wv) / D;
	if (t < 0.0 || (s + t) > 1.0)  // I is outside T
		return 0;

	return 1;                       // I is in T
}

void Voxel::ChunkMap::releaseChunk(const glm::ivec2 & coordinate)
{
	if (hasChunkAtXZ(coordinate.x, coordinate.y))
	{
		auto chunk = getChunkAtXZ(coordinate.x, coordinate.y);
		if (chunk)
		{
			std::unique_lock<std::mutex> lock(mapMutex);
			chunk->releaseMesh();
			delete chunk;

			map.erase(coordinate);
			chunkLUT.erase(coordinate);

			//std::cout << "Removing chunk (" << coordinate.x << ", " << coordinate.y << ")" << std::endl;
		}
	}
}
