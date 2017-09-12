#include "ChunkMap.h"
#include <Chunk.h>
#include <ChunkSection.h>
#include <iostream>
#include <Utility.h>
#include <Physics.h>
#include <ChunkWorkManager.h>
#include <Voronoi.h>
#include <Camera.h>
#include <Frustum.h>
#include <Region.h>

using namespace Voxel;

Voxel::ChunkMap::ChunkMap()
	: currentChunkPos(0)
{
}

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
	std::cout << "[ChunkMap] Player is at (" << playerPosition.x << ", " << playerPosition.y << ", " << playerPosition.z << ")" << std::endl;

	// Only need player x and z to find which chunk that player is in. This is world position
	int chunkX = static_cast<int>(playerPosition.x) / Constant::CHUNK_SECTION_WIDTH;
	int chunkZ = static_cast<int>(playerPosition.z) / Constant::CHUNK_SECTION_LENGTH;

	if (playerPosition.x < 0) chunkX -= 1;
	if (playerPosition.z < 0) chunkZ -= 1;

	std::cout << "[ChunkMap] Player is in chunk (" << chunkX << ", " << chunkZ << ")" << std::endl;

	currentChunkPos.x = chunkX;
	currentChunkPos.y = chunkZ;

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
				map.emplace(coordinate, std::shared_ptr<Chunk>(newChunk));

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

std::vector<glm::vec2> Voxel::ChunkMap::initActiveChunks(const int renderDistance)
{
	int minX = currentChunkPos.x - renderDistance;
	int maxX = currentChunkPos.x + renderDistance;
	int minZ = currentChunkPos.y - renderDistance;
	int maxZ = currentChunkPos.y + renderDistance;

	// chunk coodinates that need mesh
	std::vector<glm::vec2> chunkCoordinates;

	chunkCoordinates.push_back(glm::vec2(currentChunkPos));

	for (int x = minX; x <= maxX; x++)
	{
		activeChunks.push_back(std::list<glm::ivec2>());

		for (int z = minZ; z <= maxZ; z++)
		{
			if (hasChunkAtXZ(x, z))
			{
				activeChunks.back().push_back(glm::ivec2(x, z));
				// Safe to get raw pointer because this is initialization
				Chunk* chunk = getChunkAtXZ(x, z).get();
				chunk->setActive(true);
				chunk->setVisibility(true);
				//chunk->updateTimestamp(time);

				// Don't add current chunk position to vector, because it's already added in front
				if (x == currentChunkPos.x && z == currentChunkPos.y)
				{
					continue;
				}

				chunkCoordinates.push_back(glm::vec2(chunk->getCoordinate()));
			}
		}
	}

	return chunkCoordinates;
}

void ChunkMap::clear()
{
	/*
	for (auto e : map)
	{
		if (e.second)
		{
			delete e.second;
		}
	}
	*/

	map.clear();
}

bool Voxel::ChunkMap::hasChunkAtXZ(int x, int z)
{
	std::unique_lock<std::mutex> lock(mapMutex);
	auto v2 = glm::ivec2(x, z);
	return chunkLUT.find(v2) != chunkLUT.end();
}

std::shared_ptr<Chunk> Voxel::ChunkMap::getChunkAtXZ(int x, int z)
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

std::vector<glm::ivec2> Voxel::ChunkMap::getChunksNearByBlock(const glm::ivec3 & blockLocalPos, const glm::ivec3& blockChunkPos)
{
	std::vector<glm::ivec2> list;

	auto edgeX = Constant::CHUNK_SECTION_WIDTH - 1;
	auto edgeZ = Constant::CHUNK_SECTION_LENGTH - 1;

	// add left, right, front, back chunks

	if (blockLocalPos.x == 0)
	{
		list.push_back(glm::ivec2(blockChunkPos.x - 1, blockChunkPos.z));
	}

	if (blockLocalPos.z == 0)
	{
		list.push_back(glm::ivec2(blockChunkPos.x, blockChunkPos.z - 1));
	}

	if (blockLocalPos.x == edgeX)
	{
		list.push_back(glm::ivec2(blockChunkPos.x + 1, blockChunkPos.z));
	}

	if (blockLocalPos.z == edgeZ)
	{
		list.push_back(glm::ivec2(blockChunkPos.x, blockChunkPos.z + 1));
	}

	// Add diagonal chunks

	if (blockLocalPos.x == 0 && blockLocalPos.z == 0)
	{
		list.push_back(glm::ivec2(blockChunkPos.x - 1, blockChunkPos.z - 1));
	}

	if (blockLocalPos.x == edgeX && blockLocalPos.z == edgeZ)
	{
		list.push_back(glm::ivec2(blockChunkPos.x + 1, blockChunkPos.z + 1));
	}

	if (blockLocalPos.x == 0 && blockLocalPos.z == edgeZ)
	{
		list.push_back(glm::ivec2(blockChunkPos.x - 1, blockChunkPos.z + 1));
	}

	if (blockLocalPos.x == edgeX && blockLocalPos.z == 0)
	{
		list.push_back(glm::ivec2(blockChunkPos.x + 1, blockChunkPos.z - 1));
	}

	list.push_back(glm::ivec2(blockChunkPos.x, blockChunkPos.z));

	return list;
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
				auto chunkSection = chunk->getChunkSectionAtY(chunkSectionPos.y);
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
				auto chunkSection = chunk->getChunkSectionAtY(chunkSectionPos.y);
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

void Voxel::ChunkMap::placeBlockAt(const glm::ivec3 & blockPos, const Cube::Face & faceDir, ChunkWorkManager* workManager)
{
	glm::ivec3 targetPos = blockPos;
	
	switch (faceDir)
	{
	case Cube::Face::FRONT:
		targetPos.z -= 1;
		break;
	case Cube::Face::BACK:
		targetPos.z += 1;
		break;
	case Cube::Face::LEFT:
		targetPos.x -= 1;
		break;
	case Cube::Face::RIGHT:
		targetPos.x += 1;
		break;
	case Cube::Face::TOP:
		targetPos.y += 1;
		break;
	case Cube::Face::BOTTOM:
		targetPos.y -= 1;
		break;
	default:
		return;
		break;
	}

	if (targetPos.y < 0 || targetPos.y > 256)
	{
		return;
	}

	glm::ivec3 blockLocalPos;
	glm::ivec3 chunkSectionPos;

	blockWorldCoordinateToLocalAndChunkSectionCoordinate(glm::ivec3(targetPos.x, targetPos.y, targetPos.z), blockLocalPos, chunkSectionPos);

	bool hasChunk = this->hasChunkAtXZ(chunkSectionPos.x, chunkSectionPos.z);

	if (hasChunk)
	{
		// target chunk
		auto chunk = this->getChunkAtXZ(chunkSectionPos.x, chunkSectionPos.z);
		if (chunk)
		{
			if (chunk->isActive())
			{
				// chunk is active. Only can place block at active chunk
				// target chunk section
				auto chunkSection = chunk->getChunkSectionAtY(chunkSectionPos.y);
				if (chunkSection == nullptr)
				{
					chunk->createChunkSectionAtY(chunkSectionPos.y);
				}

				chunkSection = chunk->getChunkSectionAtY(chunkSectionPos.y);

				assert(chunkSection != nullptr);

				chunkSection->setBlockAt(blockLocalPos, Block::BLOCK_ID::GRASS);

				//meshGenerator->generateSingleChunkMesh(chunk, this);

				//std::cout << "Placing block at chunk: " << Utility::Log::vec3ToStr(chunkSectionPos) << std::endl;
				//std::cout << "Block pos = " << Utility::Log::vec3ToStr(targetPos) << std::endl;
				//std::cout << "Block local pos = " << Utility::Log::vec3ToStr(blockLocalPos) << std::endl;
			
				std::vector<glm::ivec2> refreshList = getChunksNearByBlock(blockLocalPos, chunkSectionPos);

				workManager->addLoad(refreshList, true);
			}
		}
		// Else, chunk is nullptr.
	}
	else
	{
		// Player is impossible to place block where chunk doesn't exists.
		std::cout << "[ChunkMap] Error. Tried to place block where chunk doesn't exists" << std::endl;
		return;
	}
}

void Voxel::ChunkMap::removeBlockAt(const glm::ivec3 & blockPos, ChunkWorkManager* workManager)
{
	glm::ivec3 blockLocalPos;
	glm::ivec3 chunkSectionPos;

	blockWorldCoordinateToLocalAndChunkSectionCoordinate(blockPos, blockLocalPos, chunkSectionPos);

	bool hasChunk = this->hasChunkAtXZ(chunkSectionPos.x, chunkSectionPos.z);

	if (hasChunk)
	{
		// target chunk
		auto chunk = this->getChunkAtXZ(chunkSectionPos.x, chunkSectionPos.z);
		if (chunk)
		{
			if (chunk->isActive())
			{
				// chunk is active. Only can place block at active chunk
				// target chunk section
				auto chunkSection = chunk->getChunkSectionAtY(chunkSectionPos.y);
				if (chunkSection)
				{
					chunkSection->setBlockAt(blockLocalPos, Block::BLOCK_ID::AIR);

					if (chunkSection->getTotalNonAirBlockSize() == 0)
					{
						chunk->deleteChunkSectionAtY(chunkSectionPos.y);
					}

					std::vector<glm::ivec2> refreshList = getChunksNearByBlock(blockLocalPos, chunkSectionPos);

					workManager->addLoad(refreshList, true);
				}
				else
				{
					std::cout << "[ChunkMap] Error. Tried to break block where chunk section doesn't exists" << std::endl;
					return;
				}
			}
		}
		// Else, chunk is nullptr.
	}
	else
	{
		// Player is impossible to place block where chunk doesn't exists.
		std::cout << "[ChunkMap] Error. Tried to break block where chunk doesn't exists" << std::endl;
		return;
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
			//delete chunk;

			map.erase(coordinate);
			chunkLUT.erase(coordinate);

			//std::cout << "Removing chunk (" << coordinate.x << ", " << coordinate.y << ")" << std::endl;
		}
	}
}

int Voxel::ChunkMap::getActiveChunksCount()
{
	int count = 0;

	for (auto& e : map)
	{
		if ((e.second)->isActive())
		{
			count++;
		}
	}

	return count;
}

bool Voxel::ChunkMap::update(const glm::vec3 & playerPosition, ChunkWorkManager * workManager, const double time)
{
	int chunkX = static_cast<int>(playerPosition.x) / Constant::CHUNK_SECTION_WIDTH;
	int chunkZ = static_cast<int>(playerPosition.z) / Constant::CHUNK_SECTION_LENGTH;

	// Shift by 1 if player is in negative position in XZ axis.
	if (playerPosition.x < 0) chunkX -= 1;
	if (playerPosition.z < 0) chunkZ -= 1;

	auto newChunkXZ = glm::ivec2(chunkX, chunkZ);

	if (newChunkXZ != currentChunkPos)
	{
		//std::cout << "Player pos (" << playerPosition.x << ", " << playerPosition.z << ")" << std::endl;
		// Player moved to new chunk.
		// normally, player should move more than 1 chunk at a time. Moving more than 1 chunk (16 blocks = 16 meter in scale)
		// means player is cheating or in god mode or whatever. 

		// Check if it's in boundary
		//auto newChunk = map->getChunkAtXZ(chunkX, chunkZ);
		//assert(newChunk != nullptr);
		auto curChunkWorldPos = Voxel::Math::chunkXZToWorldPosition(currentChunkPos);

		// check if player is out of range. If so, proceed.
		float dist = glm::distance(curChunkWorldPos, glm::vec3(playerPosition.x, 0, playerPosition.z));
		//std::cout << "d = " << dist << std::endl;
		bool isNearby = glm::abs(dist) <= Constant::CHUNK_RANGE;

		//bool inBorder = newChunk->isPointInBorder(playerPosition);
		if (isNearby)
		{
			return false;
		}

		// Anyway, first we get how far player moved. In chunk distance.
		// Then find which row and col need to be added based on direction player moved.
		// also find which row and col to pop aswell.
		glm::ivec2 d = newChunkXZ - currentChunkPos;
		std::cout << "Player moved to new chunk (" << chunkX << ", " << chunkZ << ") from chunk (" << currentChunkPos.x << ", " << currentChunkPos.y << ")" << std::endl;
		currentChunkPos = newChunkXZ;

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

		auto start = Utility::Time::now();

		std::vector<glm::ivec2> chunksToUnload;		// Chunks to unload and release
		std::vector<glm::ivec2> chunksToLoad;		// Chunks to load (map gen and mesh build)
		std::vector<glm::ivec2> chunksToReload;		// Chunks to reload (rebuild mesh)

		if (d.x != 0)
		{
			// Run move function as much as chunk distance
			int dist = glm::abs(d.x);

			// Moved in x axis
			for (int i = 0; i < dist; i++)
			{
				if (d.x < 0)
				{
					// Moved to west
					std::cout << "Player moved to west. d.x = " << d.x << std::endl;
					moveWest(chunksToUnload, chunksToLoad, chunksToReload, time);
				}
				else
				{
					// moved to east
					std::cout << "Player moved to east. d.x = " << d.x << std::endl;
					moveEast(chunksToUnload, chunksToLoad, chunksToReload, time);
				}
			}
		}
		// Else, didn't move in X axis

		if (d.y/*z*/ != 0)
		{
			// Run move function as much as chunk distance
			int dist = glm::abs(d.y);
			
			// Moved in z axis
			for (int i = 0; i < dist; i++)
			{
				if (d.y < 0)
				{
					// Move to north
					std::cout << "Player moved to north. d.y = " << d.y << std::endl;
					moveNorth(chunksToUnload, chunksToLoad, chunksToReload, time);
				}
				else
				{
					// Moved to sourth
					std::cout << "Player moved to south. d.y = " << d.y << std::endl;
					moveSouth(chunksToUnload, chunksToLoad, chunksToReload, time);
				}
			}
		}
		// Else, didn't move in Z axis

		bool notify = false;
		// Unload has highest priority
		if (!chunksToUnload.empty())
		{
			workManager->addUnload(chunksToUnload);
			notify = true;
		}

		// Load has second priority
		if (!chunksToLoad.empty())
		{
			auto p = glm::vec2(newChunkXZ);
			std::sort(chunksToLoad.begin(), chunksToLoad.end(), [p](const glm::vec2& lhs, const glm::vec2& rhs) { return glm::abs(glm::distance(p, lhs)) < glm::abs(glm::distance(p, rhs)); });
			workManager->addLoad(chunksToLoad);
			notify = true;
		}

		// Rebuilding mesh isn't important unless player is build mode. 
		// but we are rebuilding mesh that is far away from, so it has lowest priority
		if (!chunksToReload.empty())
		{
			workManager->addLoad(chunksToReload);
			notify = true;
		}

		if (notify)
		{
			workManager->notify();
		}

		auto end = Utility::Time::now();
		std::cout << "Chunk loader update took: " << Utility::Time::toMilliSecondString(start, end) << std::endl;

		return true;
	}

	return false;
}

void Voxel::ChunkMap::moveWest(std::vector<glm::ivec2>& chunksToUnload, std::vector<glm::ivec2>& chunksToLoad, std::vector<glm::ivec2>& chunksToReload, const double curTime)
{
	// Unload all the chunk in the back of the active chunk list
	for (auto& chunkXZ : activeChunks.back())
	{
		std::shared_ptr<Chunk> chunk = getChunkAtXZ(chunkXZ.x, chunkXZ.y);
		
		chunk->setActive(false);
		chunk->setVisibility(false);

		chunksToUnload.push_back(glm::ivec2(chunkXZ.x, chunkXZ.y));
	}

	// Pop
	activeChunks.pop_back();

	// Get x from first list and -1 for new list to west (negative x)
	int x = activeChunks.front().front().x - 1;
	// Also get first z in list.
	int zStart = activeChunks.back().front().y;

	// Before add new list on front, rebuild mesh for current front
	for (auto& chunkXZ : activeChunks.front())
	{
		std::shared_ptr<Chunk> chunk = getChunkAtXZ(chunkXZ.x, chunkXZ.y);

		auto pos = chunk->getPosition();
		chunksToReload.push_back(glm::ivec2(pos.x, pos.z));
	}

	// Add new list on front with empty chunks
	activeChunks.push_front(std::list<glm::ivec2>());

	// Add empty chunks
	int lastZ = activeChunks.back().size() + zStart;
	for (int z = zStart; z < lastZ; z++)
	{
		if (!hasChunkAtXZ(x, z))
		{
			generateEmptyChunk(x, z);
		}
	}

	for (int z = zStart; z < lastZ; z++)
	{
		auto newChunk = getChunkAtXZ(x, z);

		newChunk->setActive(true);
		newChunk->setVisibility(true);
		newChunk->updateTimestamp(curTime);

		auto pos = glm::ivec2(x, z);
		activeChunks.front().push_back(pos);

		chunksToLoad.push_back(pos);
	}
}

void Voxel::ChunkMap::moveEast(std::vector<glm::ivec2>& chunksToUnload, std::vector<glm::ivec2>& chunksToLoad, std::vector<glm::ivec2>& chunksToReload, const double curTime)
{
	// Unload all the chunk in the front of chunk list
	for (auto& chunkXZ : activeChunks.front())
	{
		std::shared_ptr<Chunk> chunk = getChunkAtXZ(chunkXZ.x, chunkXZ.y);

		chunk->setActive(false);
		chunk->setVisibility(false);

		chunksToUnload.push_back(glm::ivec2(chunkXZ.x, chunkXZ.y));
	}

	// Pop
	activeChunks.pop_front();

	// Get x from last list and +1 for new list to east (positive x)
	int x = activeChunks.back().front().x + 1;
	// Also get first z in list.
	int zStart = activeChunks.back().front().y;

	// Before add new list on back, rebuild mesh for current back
	for (auto& chunkXZ : activeChunks.back())
	{
		std::shared_ptr<Chunk> chunk = getChunkAtXZ(chunkXZ.x, chunkXZ.y);

		auto pos = chunk->getPosition();
		chunksToReload.push_back(glm::ivec2(pos.x, pos.z));
	}

	// Add new list on front with empty chunks
	activeChunks.push_back(std::list<glm::ivec2>());

	// Add empty chunks
	int lastZ = activeChunks.front().size() + zStart;
	for (int z = zStart; z < lastZ; z++)
	{
		if (!hasChunkAtXZ(x, z))
		{
			generateEmptyChunk(x, z);
		}
	}

	for (int z = zStart; z < lastZ; z++)
	{
		auto newChunk = getChunkAtXZ(x, z);

		newChunk->setActive(true);
		newChunk->setVisibility(true);
		newChunk->updateTimestamp(curTime);

		auto pos = glm::ivec2(x, z);
		activeChunks.back().push_back(pos);

		chunksToLoad.push_back(pos);
	}
}

void Voxel::ChunkMap::moveSouth(std::vector<glm::ivec2>& chunksToUnload, std::vector<glm::ivec2>& chunksToLoad, std::vector<glm::ivec2>& chunksToReload, const double curTime)
{
	// Get z. This is the new z coordinate that we will add to active chunk list
	int z = activeChunks.front().back().y + 1;
	//std::cout << "new z = " << z << std::endl;

	// Iterate through active chunks and unload all the chunks in front of sub list
	for (auto& chunksZ : activeChunks)
	{
		std::shared_ptr<Chunk> chunk = getChunkAtXZ(chunksZ.front().x, chunksZ.front().y);

		chunk->setActive(false);
		chunk->setVisibility(false);

		auto pos = chunk->getPosition();
		//std::cout << "Deactivating chunk at (" << pos.x << ", " << pos.z << ")" << std::endl;
		chunksToUnload.push_back(glm::ivec2(pos.x, pos.z));
		chunksZ.pop_front();
	}

	// Iterate through active chunks and add generate new chunk.
	for (auto& chunksZ : activeChunks)
	{
		int curX = chunksZ.front().x;

		if (!hasChunkAtXZ(curX, z))
		{
			// map doesn't have chunk. create empty
			generateEmptyChunk(curX, z);
		}
		// We need to generate chunk first because thread will be able to access new chunk while building a mesh, which results weired wall of mesh 
	}

	for (auto& chunksZ : activeChunks)
	{
		// Before we add new chunk on back, we need to refresh current one
		auto curPos = chunksZ.back();
		chunksToReload.push_back(glm::ivec2(curPos.x, curPos.y));

		// get chunk
		auto newChunk = getChunkAtXZ(curPos.x, z);
		newChunk->setActive(true);
		newChunk->setVisibility(true);
		newChunk->updateTimestamp(curTime);

		auto pos = glm::ivec2(curPos.x, z);
		chunksZ.push_back(pos);

		//std::cout << "Activating chunk at (" << pos.x << ", " << pos.z << ")" << std::endl;
		chunksToLoad.push_back(pos);
	}
}

void Voxel::ChunkMap::moveNorth(std::vector<glm::ivec2>& chunksToUnload, std::vector<glm::ivec2>& chunksToLoad, std::vector<glm::ivec2>& chunksToReload, const double curTime)
{
	// Get z
	int z = activeChunks.front().front().y - 1;

	// Iterate through active chunks and unload all the chunks in front of sub list
	for (auto& chunksZ : activeChunks)
	{
		std::shared_ptr<Chunk> chunk = getChunkAtXZ(chunksZ.back().x, chunksZ.back().y);

		chunk->setActive(false);
		chunk->setVisibility(false);

		auto pos = chunk->getPosition();
		//std::cout << "Deactivating chunk at (" << pos.x << ", " << pos.z << ")" << std::endl;
		chunksToUnload.push_back(glm::ivec2(pos.x, pos.z));
		chunksZ.pop_back();
	}

	// Iterate through active chunks and add generate new chunk
	for (auto& chunksZ : activeChunks)
	{
		int curX = chunksZ.front().x;

		if (!hasChunkAtXZ(curX, z))
		{
			// map doesn't have chunk. create empty
			generateEmptyChunk(curX, z);
		}
		// We need to generate chunk first because thread will be able to access new chunk while building a mesh, which results weired wall of mesh 
	}

	for (auto& chunksZ : activeChunks)
	{
		// Before we add new chunk on front, we need to refresh current one
		auto curPos = chunksZ.front();
		chunksToReload.push_back(glm::ivec2(curPos.x, curPos.y));

		// get chunk
		auto newChunk = getChunkAtXZ(curPos.x, z);
		newChunk->setActive(true);
		newChunk->setVisibility(true);
		newChunk->updateTimestamp(curTime);

		auto pos = glm::ivec2(curPos.x, z);
		chunksZ.push_front(pos);

		//std::cout << "Activating chunk at (" << pos.x << ", " << pos.z << ")" << std::endl;
		chunksToLoad.push_back(pos);
	}
}

int Voxel::ChunkMap::findVisibleChunk()
{
	int count = 0;

	for (auto& e : map)
	{
		auto chunk = e.second;
		if (chunk != nullptr)
		{
			bool visible = Camera::mainCamera->getFrustum()->isChunkBorderInFrustum(chunk.get());
			chunk->setVisibility(visible);

			if (visible) 
				count++;
		}
	}

	return count;
}

void Voxel::ChunkMap::render()
{
	for (auto& e : map)
	{
		// Get chunk raw pointer. 
		auto chunk = (e.second).get();
		if (chunk != nullptr)
		{
			if (chunk->isActive())
			{
				if (chunk->isGenerated())
				{
					if (chunk->isVisible())
					{
						chunk->render();
					}
				}
			}
		}
	}
}