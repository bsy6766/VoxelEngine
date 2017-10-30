#include "ChunkMap.h"
#include <glm/gtx/transform.hpp>
#include <Chunk.h>
#include <ChunkSection.h>
#include <iostream>
#include <Utility.h>
#include <Geometry.h>
#include <ChunkWorkManager.h>
#include <Voronoi.h>
#include <Camera.h>
#include <Frustum.h>
#include <Region.h>
#include <ProgramManager.h>
#include <Program.h>
#include <ChunkUtil.h>
#include <ChunkMesh.h>
#include <Application.h>

using namespace Voxel;

Voxel::ChunkMap::ChunkMap()
	: currentChunkPos(0)
	, chunkBorderVao(0)
	, chunkBorderLineSize(0)
	, blockOutlineVao(0)
	, chunkBorderModelMat(1.0f)
	, renderChunkBorderMode(false)
	, renderChunksMode(true)
	, renderBlockOutlineMode(true)
	, updateChunksMode(true)
	, minXZ(0)
	, maxXZ(0)
{
}

Voxel::ChunkMap::~ChunkMap()
{
	clear();

	if (chunkBorderVao)
	{
		glDeleteVertexArrays(1, &chunkBorderVao);
	}

	if (blockOutlineVao)
	{
		glDeleteVertexArrays(1, &blockOutlineVao);
	}
}

std::vector<glm::vec2> Voxel::ChunkMap::initChunkNearPlayer(const glm::vec3 & playerPosition, const int renderDistance)
{
	std::cout << "[ChunkMap] Player is at (" << playerPosition.x << ", " << playerPosition.y << ", " << playerPosition.z << ")\n";

	// Only need player x and z to find which chunk that player is in. This is world position
	int chunkX = static_cast<int>(playerPosition.x) / Constant::CHUNK_SECTION_WIDTH;
	int chunkZ = static_cast<int>(playerPosition.z) / Constant::CHUNK_SECTION_LENGTH;

	if (playerPosition.x < 0) chunkX -= 1;
	if (playerPosition.z < 0) chunkZ -= 1;

	std::cout << "[ChunkMap] Player is in chunk (" << chunkX << ", " << chunkZ << ")\n";

	currentChunkPos.x = chunkX;
	currentChunkPos.y = chunkZ;

	// Add extra 2 rows and 2 cols to min max of active chunk.
	// The reason why I did this is to avoid bad block placement during the map generation.
	// Mid size structure like tree and boulders can exceed size of 16 which is larger than single chunk
	// That is why we add 2 extra row and col to give extra 32 blocks space to place mid size structure.
	// These extra row and col won't be active and stay inactive.
	//
	// This will consume more memory due to number of extra chunks to add.
	// i.e. Render distance 16 (256 chunks) will have 68 extra chunks to load, resulting total 324 chunks.
	// The number of extra chunk dramatically increases as render distance increase.
	// With 32 bits application, memory is limitted. Probably 20 is maximum render distance.
	// This is why I also want to switch the build to 64 bits since majority of steam user uses 64 bits hardware (based on hardware survay).

	int rd = renderDistance + 2;

	int minX = chunkX - rd;
	int maxX = chunkX + rd;
	int minZ = chunkZ - rd;
	int maxZ = chunkZ + rd;

	// Extra layer is applied when active chunks get initailized
	minXZ.x = minX;
	minXZ.y = minZ;
	maxXZ.x = maxX;
	maxXZ.y = maxZ;

	// chunk coodinates that needs to be genreated
	std::vector<glm::vec2> chunkCoordinates;
	chunkCoordinates.push_back(glm::vec2(currentChunkPos));

	for (int x = minX; x <= maxX; x++)
	{
		for (int z = minZ; z <= maxZ; z++)
		{
			auto coordinate = glm::ivec2(x, z);
			if (chunkLUT.find(coordinate) == chunkLUT.end())
			{
				// new chunk
				//std::cout << "[ChunkMap] Adding (" << x << ", " << z << ") chunk.\n";
				Chunk* newChunk = Chunk::createEmpty(x, z);

				map.emplace(coordinate, std::shared_ptr<Chunk>(newChunk));

				// Add to LUt
				chunkLUT.emplace(coordinate);

				// Don't add current chunk position to vector, because it's already added in front
				if (x == currentChunkPos.x && z == currentChunkPos.y)
				{
					continue;
				}

				chunkCoordinates.push_back(glm::vec2(newChunk->getCoordinate()));
			}
			else
			{
				// Chunk is already loaded on map
				continue;
			}
		}
	}

	std::cout << "Chunk map size = " << map.size() << std::endl;

	// Returns chunks coordinates that need to be processed (gen, build mesh, etc). Actually all chunks that is generated here.
	return chunkCoordinates;
}

void Voxel::ChunkMap::initActiveChunks()
{
	const int extraLayer = 2;

	// Activate chunks that is only in render distance. 
	const int minX = minXZ.x + extraLayer;
	const int maxX = maxXZ.x - extraLayer;

	const int minZ = minXZ.y + extraLayer;
	const int maxZ = maxXZ.y - extraLayer;

	int count = 0;

	// Initailize 2D list in render distance. Don't count the extra rows and cols. Mark chunks as active.
	for (int x = minX; x <= maxX; x++)
	{
		activeChunks.push_back(std::list<glm::ivec2>());

		for (int z = minZ; z <= maxZ; z++)
		{
			if (hasChunkAtXZ(x, z))
			{
				activeChunks.back().push_back(glm::ivec2(x, z));

				// Safe to get raw pointer because this is initialization and guaranteed to have chunk on intializing.
				Chunk* chunk = getChunkAtXZ(x, z).get();

				// Todo: handle invalid chunk. 

				// set active. that's all for now.
				chunk->setActive(true);
			}
		}
	}

	std::cout << "Active chunk size = " << activeChunks.size() * activeChunks.front().size() << std::endl;
}

void Voxel::ChunkMap::initChunkBorderDebug(Program* program)
{
	// Generate vertex array object
	glGenVertexArrays(1, &chunkBorderVao);
	// Bind it
	glBindVertexArray(chunkBorderVao);

	// Generate buffer object
	GLuint vbo;
	glGenBuffers(1, &vbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	float size = Constant::CHUNK_BORDER_SIZE;
	float yMin = 0;
	float yMax = 256.0f;

	std::vector<float> lines =
	{
		// vertical blue chunk lines
		0, yMin, 0,
		0, yMax, 0,
		size, yMin, 0,
		size, yMax, 0,
		0, yMin, size,
		0, yMax, size,
		size, yMin, size,
		size, yMax, size,
	};

	std::vector<float> colors =
	{
		// vertical blue chunk lines
		0, 0, 1.0f, 1.0f,
		0, 0, 1.0f, 1.0f,
		0, 0, 1.0f, 1.0f,
		0, 0, 1.0f, 1.0f,
		0, 0, 1.0f, 1.0f,
		0, 0, 1.0f, 1.0f,
		0, 0, 1.0f, 1.0f,
		0, 0, 1.0f, 1.0f,
	};

	float subY = 0.0f;
	for (int i = 0; i < 17; i++)
	{
		lines.push_back(0); lines.push_back(subY); lines.push_back(0);
		lines.push_back(0); lines.push_back(subY); lines.push_back(size);
		lines.push_back(0); lines.push_back(subY); lines.push_back(size);
		lines.push_back(size); lines.push_back(subY); lines.push_back(size);
		lines.push_back(size); lines.push_back(subY); lines.push_back(size);
		lines.push_back(size); lines.push_back(subY); lines.push_back(0);
		lines.push_back(size); lines.push_back(subY); lines.push_back(0);
		lines.push_back(0); lines.push_back(subY); lines.push_back(0);

		colors.push_back(0); colors.push_back(0); colors.push_back(1.0f); colors.push_back(1.0f);
		colors.push_back(0); colors.push_back(0); colors.push_back(1.0f); colors.push_back(1.0f);
		colors.push_back(0); colors.push_back(0); colors.push_back(1.0f); colors.push_back(1.0f);
		colors.push_back(0); colors.push_back(0); colors.push_back(1.0f); colors.push_back(1.0f);
		colors.push_back(0); colors.push_back(0); colors.push_back(1.0f); colors.push_back(1.0f);
		colors.push_back(0); colors.push_back(0); colors.push_back(1.0f); colors.push_back(1.0f);
		colors.push_back(0); colors.push_back(0); colors.push_back(1.0f); colors.push_back(1.0f);
		colors.push_back(0); colors.push_back(0); colors.push_back(1.0f); colors.push_back(1.0f);

		subY += Constant::CHUNK_BORDER_SIZE;
	}

	float nx = -Constant::CHUNK_BORDER_SIZE;
	float nz = -Constant::CHUNK_BORDER_SIZE;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i == 1 && j == 1) continue;

			lines.push_back(nx); lines.push_back(yMin); lines.push_back(nz);
			lines.push_back(nx); lines.push_back(yMax); lines.push_back(nz);

			//red
			colors.push_back(1.0f); colors.push_back(0); colors.push_back(0); colors.push_back(1.0f);
			colors.push_back(1.0f); colors.push_back(0); colors.push_back(0); colors.push_back(1.0f);

			nz += Constant::CHUNK_BORDER_SIZE;
		}

		nx += Constant::CHUNK_BORDER_SIZE;
		nz = -Constant::CHUNK_BORDER_SIZE;
	}

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * lines.size(), &lines.front(), GL_STATIC_DRAW);

	// Enable vertices attrib
	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Generate buffer object
	GLuint cbo;
	glGenBuffers(1, &cbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, cbo);

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * colors.size(), &colors.front(), GL_STATIC_DRAW);
	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	// unbind buffer

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);

	chunkBorderLineSize = static_cast<int>(lines.size());
}

void Voxel::ChunkMap::initBlockOutline(Program* program)
{
	// Create debug chunk box
	// Generate vertex array object
	glGenVertexArrays(1, &blockOutlineVao);
	// Bind it
	glBindVertexArray(blockOutlineVao);

	// Enable vertices attrib
	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");

	// Generate buffer object
	GLuint vbo;
	glGenBuffers(1, &vbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLfloat cube[] = {
		// x, y, z
		0.502f, -0.502f, -0.502f,
		-0.502f, -0.502f, -0.502f,
		-0.502f, -0.502f, 0.502f,
		0.502f, -0.502f, 0.502f,
		0.502f, 0.502f, -0.502f,
		-0.502f, 0.502f, -0.502f,
		-0.502f, 0.502f, 0.502f,
		0.502f, 0.502f, 0.502f,
	};

	GLfloat color[] = {
		// x, y, z
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	unsigned int indices[] = {
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
	};

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Generate buffer object
	GLuint cbo;
	glGenBuffers(1, &cbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, cbo);

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	// unbind buffer
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint ibo;
	// Generate indices object
	glGenBuffers(1, &ibo);
	// Bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	// Load indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// unbind buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
	glDeleteBuffers(1, &ibo);
}

void ChunkMap::clear()
{
	map.clear();
	chunkLUT.clear();
	currentChunkPos = glm::ivec2(0);
	activeChunks.clear();
	regionTerrainsMap.clear();

	minXZ = glm::ivec2(0);
	maxXZ = glm::ivec2(0);
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

std::vector<std::vector<std::shared_ptr<Chunk>>> Voxel::ChunkMap::getNearByChunks(const glm::ivec2 & chunkXZ)
{
	std::vector<std::vector<std::shared_ptr<Chunk>>> nearBy;

	for (int i = 0; i < 3; i++)
	{
		nearBy.push_back(std::vector<std::shared_ptr<Chunk>>());
		for (int j = 0; j < 3; j++)
		{
			nearBy.back().push_back(nullptr);
		}
	}

	// south east
	auto SEChunk = getChunkAtXZ(chunkXZ.x + 1, chunkXZ.y + 1);
	if (SEChunk)
	{
		nearBy.at(0).at(0) = SEChunk;
	}

	// south 
	auto SChunk = getChunkAtXZ(chunkXZ.x, chunkXZ.y + 1);
	if (SChunk)
	{
		nearBy.at(0).at(1) = SChunk;
	}

	// south west
	auto SWChunk = getChunkAtXZ(chunkXZ.x - 1, chunkXZ.y + 1);
	if (SWChunk)
	{
		nearBy.at(0).at(2) = SWChunk;
	}

	// east
	auto EChunk = getChunkAtXZ(chunkXZ.x + 1, chunkXZ.y);
	if (EChunk)
	{
		nearBy.at(1).at(0) = EChunk;
	}

	// center
	// Skip
	//nearBy.at(1).at(1) = nullptr;

	// west
	auto WChunk = getChunkAtXZ(chunkXZ.x - 1, chunkXZ.y);
	if (WChunk)
	{
		nearBy.at(1).at(2) = WChunk;
	}

	// north east
	auto NEChunk = getChunkAtXZ(chunkXZ.x + 1, chunkXZ.y - 1);
	if (NEChunk)
	{
		nearBy.at(2).at(0) = NEChunk;
	}

	// north
	auto NChunk = getChunkAtXZ(chunkXZ.x, chunkXZ.y - 1);
	if (NChunk)
	{
		nearBy.at(2).at(1) = NChunk;
	}

	// north west
	auto NWChunk = getChunkAtXZ(chunkXZ.x - 1, chunkXZ.y - 1);
	if (NWChunk)
	{
		nearBy.at(2).at(2) = NWChunk;
	}


	return nearBy;
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
	return static_cast<unsigned int>(map.size());
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

Block * Voxel::ChunkMap::getBlockAtWorldXYZ(const glm::vec3 & worldPosition)
{
	return getBlockAtWorldXYZ(static_cast<int>(worldPosition.x), static_cast<int>(worldPosition.y), static_cast<int>(worldPosition.z));
}

int Voxel::ChunkMap::isBlockAtWorldXYZOpaque(const int x, const int y, const int z)
{
	// Retruns 0 if block exists and transparent. 
	// Returns 1 if block exists and opaque
	// Retruns 2 if chunk section doesn't exists
	// Retruns 3 if chunk doesn't exsits.
	// Returns 4 if chunk is inactive

	//auto start = Utility::Time::now();

	glm::ivec3 blockLocalPos;
	glm::ivec3 chunkSectionPos;

	blockWorldCoordinateToLocalAndChunkSectionCoordinate(glm::ivec3(x, y, z), blockLocalPos, chunkSectionPos);

	int result = -1;
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
						result = 0;
					}
					else
					{
						result = 1;
					}
				}
				else
				{
					// block is air == nullptr
					result = 0;
				}
			}
			// There is no block in this chunk section = nullptr
			else
			{
				result = 2;
			}
		}
		// Can't access block that is in inactive chunk
		else
		{
			result = 4;
		}
	}
	else
	{
		result = 3;
	}

	//auto end = Utility::Time::now();
	//std::cout << "block query t: " << Utility::Time::toMicroSecondString(start, end) << std::endl;
	// block quert takes 0 micro seconds usually, but sometime spikes to 15 ish.

	return result;
}

void Voxel::ChunkMap::placeBlockFromFace(const glm::ivec3 & blockWorldCoordinate, const Block::BLOCK_ID blockID, const Cube::Face & faceDir, ChunkWorkManager* workManager)
{
	glm::ivec3 targetPos = blockWorldCoordinate;

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

	placeBlockAt(targetPos, blockID, workManager);
}

void Voxel::ChunkMap::placeBlockAt(const glm::ivec3 & blockWorldCoordinate, const Block::BLOCK_ID blockID, ChunkWorkManager * wm, const bool overwrite)
{
	glm::ivec3 blockLocalPos;
	glm::ivec3 chunkSectionPos;

	blockWorldCoordinateToLocalAndChunkSectionCoordinate(blockWorldCoordinate, blockLocalPos, chunkSectionPos);

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

				chunkSection->setBlockAt(blockLocalPos, blockID, overwrite);

				if (wm)
				{
					std::vector<glm::ivec2> refreshList = getChunksNearByBlock(blockLocalPos, chunkSectionPos);
					wm->addBuildMeshWorks(refreshList, true);
				}
			}
		}
		// Else, chunk is nullptr.
	}
	else
	{
		// Player is impossible to place block where chunk doesn't exists.
		std::cout << "[ChunkMap] Error. Tried to place block where chunk doesn't exists\n";
		return;
	}
}

void Voxel::ChunkMap::placeBlockAt(const glm::ivec3 & blockWorldCoordinate, const Block::BLOCK_ID blockID, const glm::uvec3 & color, ChunkWorkManager * wm, const bool overwrite)
{
	glm::ivec3 blockLocalPos;
	glm::ivec3 chunkSectionPos;

	blockWorldCoordinateToLocalAndChunkSectionCoordinate(blockWorldCoordinate, blockLocalPos, chunkSectionPos);

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

				chunkSection->setBlockAt(blockLocalPos, blockID, color, overwrite);

				if (wm)
				{
					std::vector<glm::ivec2> refreshList = getChunksNearByBlock(blockLocalPos, chunkSectionPos);
					wm->addBuildMeshWorks(refreshList, true);
				}
			}
		}
		// Else, chunk is nullptr.
	}
	else
	{
		// Player is impossible to place block where chunk doesn't exists.
		std::cout << "[ChunkMap] Error. Tried to place block where chunk doesn't exists\n";
		return;
	}
}

void Voxel::ChunkMap::placeBlockAt(const glm::ivec3 & blockWorldCoordinate, const Block::BLOCK_ID blockID, const glm::vec3 & color, ChunkWorkManager * wm, const bool overwrite)
{
	glm::ivec3 blockLocalPos;
	glm::ivec3 chunkSectionPos;

	blockWorldCoordinateToLocalAndChunkSectionCoordinate(blockWorldCoordinate, blockLocalPos, chunkSectionPos);

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

				chunkSection->setBlockAt(blockLocalPos, blockID, color, overwrite);

				if (wm)
				{
					std::vector<glm::ivec2> refreshList = getChunksNearByBlock(blockLocalPos, chunkSectionPos);
					wm->addBuildMeshWorks(refreshList, true);
				}
			}
		}
		// Else, chunk is nullptr.
	}
	else
	{
		// Player is impossible to place block where chunk doesn't exists.
		std::cout << "[ChunkMap] Error. Tried to place block where chunk doesn't exists\n";
		return;
	}
}

void Voxel::ChunkMap::removeBlockAt(const glm::ivec3 & blockWorldCoordinate, ChunkWorkManager* workManager)
{
	glm::ivec3 blockLocalPos;
	glm::ivec3 chunkSectionPos;

	blockWorldCoordinateToLocalAndChunkSectionCoordinate(blockWorldCoordinate, blockLocalPos, chunkSectionPos);

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

					workManager->addBuildMeshWorks(refreshList, true);
				}
				else
				{
					std::cout << "[ChunkMap] Error. Tried to break block where chunk section doesn't exists\n";
					return;
				}
			}
		}
		// Else, chunk is nullptr.
	}
	else
	{
		// Player is impossible to place block where chunk doesn't exists.
		std::cout << "[ChunkMap] Error. Tried to break block where chunk doesn't exists\n";
		return;
	}
}

RayResult Voxel::ChunkMap::raycastBlock(const glm::vec3& playerEyePosition, const glm::vec3& playerDirection, const float playerRange)
{
	//std::cout << "RayCasting\n";

	//std::cout << "player pos = " << playerPosition.x << ", " << playerPosition.y << ", " << playerPosition.z << ")\n";
	//std::cout << "player dir = " << playerDirection.x << ", " << playerDirection.y << ", " << playerDirection.z << ")\n";

	auto rayStart = playerEyePosition;
	auto rayEnd = playerEyePosition + (playerDirection * playerRange);

	//std::cout << "rayStart = " << rayStart.x << ", " << rayStart.y << ", " << rayStart.z << ")\n";
	//std::cout << "rayEnd = " << rayEnd.x << ", " << rayEnd.y << ", " << rayEnd.z << ")\n";

	glm::vec3 dirVec = rayEnd - rayStart;

	float div = 1500.0f;
	glm::vec3 step = dirVec / div;

	//std::cout << "step = " << step.x << ", " << step.y << ", " << step.z << ")\n";

	int threshold = 1500;

	glm::vec3 curRayPoint = rayStart;

	glm::ivec3 startBlockPos = glm::ivec3(Utility::Math::fastFloor(rayStart.x), Utility::Math::fastFloor(rayStart.y), Utility::Math::fastFloor(rayStart.z));
	glm::ivec3 curBlockPos = startBlockPos;

	//std::cout << "start block (" << startBlockPos.x << ", " << startBlockPos.y << ", " << startBlockPos.z << ")\n";

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
		//std::cout << "visiting (" << curRayPoint.x << ", " << curRayPoint.y << ", " << curRayPoint.z << ")\n";

		auto visitingBlockPos = glm::ivec3(Utility::Math::fastFloor(curRayPoint.x), Utility::Math::fastFloor(curRayPoint.y), Utility::Math::fastFloor(curRayPoint.z));

		if (visitingBlockPos != curBlockPos)
		{
			curBlockPos = visitingBlockPos;

			//std::cout << "cur block (" << curBlockPos.x << ", " << curBlockPos.y << ", " << curBlockPos.z << ")\n";
			Block* curBlock = getBlockAtWorldXYZ(curBlockPos.x, curBlockPos.y, curBlockPos.z);

			if (curBlock)
			{
				if (curBlock->isEmpty() == false)
				{
					// raycasted block not empty. 
					if (curBlockPos != startBlockPos)
					{
						//std::cout << "Block hit (" << curBlockPos.x << ", " << curBlockPos.y << ", " << curBlockPos.z << ")\n";
						result.block = curBlock;

						// Check which face did ray hit on cube.
						result.face = raycastFace(rayStart, curRayPoint, curBlock->getBoundingBox());

						return result;
					}
				}
			}
		}


		threshold--;
	}

	return result;
}

Cube::Face Voxel::ChunkMap::raycastFace(const glm::vec3 & rayStart, const glm::vec3 & rayEnd, const Geometry::AABB & blockAABB)
{
	// Check if ray hits each triangle of cube. 

	Cube::Face result = Cube::Face::NONE;
	auto triangles = blockAABB.toTriangles();

	float minDist = std::numeric_limits<float>::max();
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
		//std::cout << "FRONT\n";
		break;
	case 2:
	case 3:
		result = Cube::Face::LEFT;
		//std::cout << "LEFT\n";
		break;
	case 4:
	case 5:
		result = Cube::Face::BACK;
		//std::cout << "BACK\n";
		break;
	case 6:
	case 7:
		result = Cube::Face::RIGHT;
		//std::cout << "RIGHT\n";
		break;
	case 8:
	case 9:
		result = Cube::Face::TOP;
		//std::cout << "TOP\n";
		break;
	case 10:
	case 11:
		result = Cube::Face::BOTTOM;
		//std::cout << "BOTTOM\n";
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
int Voxel::ChunkMap::raycastTriangle(const glm::vec3 & rayStart, const glm::vec3 & rayEnd, const Geometry::Triangle & tri, glm::vec3 & intersectingPoint)
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

			//std::cout << "Removing chunk (" << coordinate.x << ", " << coordinate.y << ")\n";
		}
	}
}

int Voxel::ChunkMap::getActiveChunksCount()
{
	return static_cast<int>(activeChunks.size() * activeChunks.front().size());
}

glm::ivec2 Voxel::ChunkMap::checkPlayerChunkPos(const glm::vec3 & playerPosition)
{
	if (!updateChunksMode) return currentChunkPos;

	int chunkX = static_cast<int>(playerPosition.x) / Constant::CHUNK_SECTION_WIDTH;
	int chunkZ = static_cast<int>(playerPosition.z) / Constant::CHUNK_SECTION_LENGTH;

	// Shift by 1 if player is in negative position in XZ axis.
	if (playerPosition.x < 0) chunkX -= 1;
	if (playerPosition.z < 0) chunkZ -= 1;

	auto newChunkXZ = glm::ivec2(chunkX, chunkZ);

	if (renderChunkBorderMode)
	{
		glm::vec3 translate = glm::vec3(newChunkXZ.x, 0, newChunkXZ.y);
		translate *= Constant::CHUNK_BORDER_SIZE;
		chunkBorderModelMat = glm::translate(glm::mat4(1.0f), translate);
	}

	if (newChunkXZ != currentChunkPos)
	{
		//std::cout << "Player pos (" << playerPosition.x << ", " << playerPosition.z << ")\n";
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
		if (!isNearby)
		{
			return newChunkXZ;
		}
	}

	return currentChunkPos;
}

bool Voxel::ChunkMap::update(const glm::ivec2& newChunkXZ, ChunkWorkManager * workManager, const double time)
{
	// Anyway, first we get how far player moved. In chunk distance.
	// Then find which row and col need to be added based on direction player moved.
	// also find which row and col to pop aswell.
	glm::ivec2 d = newChunkXZ - currentChunkPos;
	//std::cout << "Player moved to new chunk (" << chunkX << ", " << chunkZ << ") from chunk (" << currentChunkPos.x << ", " << currentChunkPos.y << ")\n";
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

	//auto start = Utility::Time::now();

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
				moveWest(workManager);
			}
			else
			{
				// moved to east
				std::cout << "Player moved to east. d.x = " << d.x << std::endl;
				moveEast(workManager);
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
				moveNorth(workManager);
			}
			else
			{
				// Moved to sourth
				std::cout << "Player moved to south. d.y = " << d.y << std::endl;
				moveSouth(workManager);
			}
		}
	}
	// Else, didn't move in Z axis

	workManager->notify();

	//auto end = Utility::Time::now();
	//std::cout << "Chunk loader update took: " << Utility::Time::toMilliSecondString(start, end) << std::endl;

	return true;
}

void Voxel::ChunkMap::moveWest(ChunkWorkManager* wm)
{
	// Remove the last row (East. because moving west)
	removeRowEast(wm);

	// Before add new list on front row (west), rebuild mesh for current front. This will be processed after all chunk has been generated.
	std::vector<ivec2> refreshList;
	for (auto& chunkXZ : activeChunks.front())
	{
		refreshList.push_back(chunkXZ);
	}

	// Then, add row on front (west)
	addRowWest(wm);

	// now add build mesh work
	for (auto& chunkXZ : refreshList)
	{
		wm->addBuildMeshWork(chunkXZ);
	}

	minXZ.x -= 1;
	maxXZ.x -= 1;
}

void Voxel::ChunkMap::moveEast(ChunkWorkManager* wm)
{
	// Remove the first row (West. because moving east)
	removeRowWest(wm);

	// Before add new list on back row (east), rebuild mesh for current back. This will be processed after all chunk has been generated
	std::vector<ivec2> refreshList;
	for (auto& chunkXZ : activeChunks.back())
	{
		refreshList.push_back(chunkXZ);
	}

	// Then, add row on back (east)
	addRowEast(wm);

	// now add build mesh work
	for (auto& chunkXZ : refreshList)
	{
		wm->addBuildMeshWork(chunkXZ);
	}

	minXZ.x += 1;
	maxXZ.x += 1;
}

void Voxel::ChunkMap::moveSouth(ChunkWorkManager* wm)
{
	// Remove the first element on each row (north. because moving south)
	removeColNorth(wm);

	// Before we add new element on back each row (south), rebuild mesh for current back element of each row. This will processed after all chunk has been generated
	std::vector<ivec2> refreshList;
	for (auto& row : activeChunks)
	{
		refreshList.push_back(row.back());
	}

	// Then, add new element on back of each row
	addColSouth(wm);

	// now add build mesh work
	for (auto& chunkXZ : refreshList)
	{
		wm->addBuildMeshWork(chunkXZ);
	}

	minXZ.y += 1;
	maxXZ.y += 1;
}

void Voxel::ChunkMap::moveNorth(ChunkWorkManager* wm)
{
	// Remove the last element on each row (south. because moving north)
	removeColSouth(wm);

	// Before we add new element on front of each row, rebuild mesh for current front element of each row. This will be processed after all chunk has been generated
	std::vector<ivec2> refreshList;
	for (auto& row : activeChunks)
	{
		refreshList.push_back(row.front());
	}

	// Then, add new element on front of each row
	addColNorth(wm);

	// now add build mesh work
	for (auto& chunkXZ : refreshList)
	{
		wm->addBuildMeshWork(chunkXZ);
	}

	minXZ.y -= 1;
	maxXZ.y -= 1;
}

void Voxel::ChunkMap::addRowWest(ChunkWorkManager* wm)
{
	// Add row west (negative X)

	//get x. - 1 because we are adding new row on west (negative x)
	int x = minXZ.x - 1;

	// Same for z. Use min max to add entire row on chunk map. Don't use active chunks
	int zStart = minXZ.y;
	int zEnd = maxXZ.y;

	// Generate empty chunk and add to pre load list
	for (int z = zStart; z <= zEnd; z++)
	{
		if (!hasChunkAtXZ(x, z))
		{
			generateEmptyChunk(x, z);

			wm->addPreGenerateWork(glm::ivec2(x, z));
		}
	}

	// Add new list on front with empty chunks
	activeChunks.push_front(std::list<glm::ivec2>());

	// Recalculate x and z. Use 2 because we have extra 2 rows and cols. Only activate within render distance, not entire row
	zStart += 2;
	zEnd -= 2;
	x += 2;

	for (int z = zStart; z <= zEnd; z++)
	{
		auto newChunk = getChunkAtXZ(x, z);

		// activate the chunk
		newChunk->setActive(true);

		auto pos = glm::ivec2(x, z);
		// add active chunk position to grid
		activeChunks.front().push_back(pos);

		// Also add to preGenerate queue. chunk might be already generated, smoothed, etc and worker thread will handle this.
		wm->addPreGenerateWork(pos);
	}
}

void Voxel::ChunkMap::addRowEast(ChunkWorkManager * wm)
{
	// Add row east (positive x)

	// get x. + 1 because we ad adding new row on east (positive x)
	int x = maxXZ.x + 1;

	// Same for z. Use min max to add entire row on chunk map. Don't use active chunks
	int zStart = minXZ.y;
	int zEnd = maxXZ.y;

	// Generate empty chunk. Add to preGenerate queue so it generates chunks even it's inactive chunk
	for (int z = zStart; z <= zEnd; z++)
	{
		if (!hasChunkAtXZ(x, z))
		{
			generateEmptyChunk(x, z);

			wm->addPreGenerateWork(glm::ivec2(x, z));
		}
	}

	// Add new list on back with empty chunks
	activeChunks.push_back(std::list<glm::ivec2>());

	// Recalculate x and z. Use 2 because we have extra 2 rows and cols. Only activate within render distance, not entire row
	zStart += 2;
	zEnd -= 2;
	x -= 2;

	for (int z = zStart; z <= zEnd; z++)
	{
		auto newChunk = getChunkAtXZ(x, z);

		// activate the chunk
		newChunk->setActive(true);

		auto pos = glm::ivec2(x, z);
		// add active chunk position to grid
		activeChunks.back().push_back(pos);

		// Also add to preGenerate queue. chunk might be already generated, smoothed, etc and worker thread will handle this.
		wm->addPreGenerateWork(pos);
	}
}

void Voxel::ChunkMap::addColSouth(ChunkWorkManager * wm)
{
	// Add element on the back of the row (positive z)

	// get z. + 1 because we are adding new element on each row toward souht (positive z)
	int z = maxXZ.y + 1;

	// Same for x. Use min max to add entire row on chunk map. Don't use active chunks
	int xStart = minXZ.x;
	int xEnd = maxXZ.x;

	// Generate empty chunk and add to pre load list
	for (int x = xStart; x <= xEnd; x++)
	{
		if (!hasChunkAtXZ(x, z))
		{
			generateEmptyChunk(x, z);

			wm->addPreGenerateWork(glm::ivec2(x, z));
		}
	}
	
	// Unlike east and west, no need to add row. 

	// Re calculate z.
	z -= 2;

	for (auto& row : activeChunks)
	{
		// get position. Use same x as the row. z remains the same
		auto pos = glm::ivec2(row.front().x, z);

		auto newChunk = getChunkAtXZ(pos.x, pos.y);

		// activate the chunk
		newChunk->setActive(true);

		// add active chunk position to grid
		row.push_back(pos);

		// Also add to preGenerate queue. chunk might be already generated, smoothed, etc and worker thread will handle this.
		wm->addPreGenerateWork(pos);
	}
}

void Voxel::ChunkMap::addColNorth(ChunkWorkManager * wm)
{
	// get z. - 1 because we are adding new element on each row toward north (negative z)
	int z = minXZ.y - 1;

	// iterate all x
	int xStart = minXZ.x;
	int xEnd = maxXZ.x;

	// Generate empty chunk and add to pre load list
	for (int x = xStart; x <= xEnd; x++)
	{
		if (!hasChunkAtXZ(x, z))
		{
			generateEmptyChunk(x, z);

			wm->addPreGenerateWork(glm::ivec2(x, z));
		}
	}

	// Unlike east and west, no need to add row. 

	// Re calculate z.
	z += 2;

	for (auto& row : activeChunks)
	{
		// get position. Use same x as the row. z remains the same
		auto pos = glm::ivec2(row.front().x, z);

		auto newChunk = getChunkAtXZ(pos.x, pos.y);
		
		// activate the chunk
		newChunk->setActive(true);

		// add active chunk position to grid
		row.push_front(pos);

		// Also add to preGenerate queue. chunk might be already generated, smoothed, etc and worker thread will handle this.
		wm->addPreGenerateWork(pos);
	}
}

void Voxel::ChunkMap::removeRowWest(ChunkWorkManager* wm)
{
	// Remove row west (negative X)

	//get x. Use min, not active chunk
	int x = minXZ.x;

	// Same for z. use min max to remove entire row from map, not on active chunks
	int zStart = minXZ.y;
	int zEnd = maxXZ.y;

	// Called by main thread. Iterate through entire row, clear buffer. These chunks will get removed from map. So add to finished queue.
	for (int z = zStart; z <= zEnd; z++)
	{
		std::shared_ptr<Chunk> chunk = getChunkAtXZ(x, z);

		// Make sure deactivates.
		chunk->setActive(false);

		auto mesh = chunk->getMesh();
		if (mesh)
		{
			// Clear mesh. This doesn't releases vao. 
			mesh->clearBuffers();
		}

		// Add to queue and let main thread to release it
		wm->addFinishedQueue(glm::ivec2(x, z));
	}

	// Before chunk map add new row on postive x (east), deactivate front row (west) from active chunk
	// We don't release mesh, there is no need to. Leave as generated, smoothed, structure added.
	for (auto& chunkXZ : activeChunks.front())
	{
		std::shared_ptr<Chunk> chunk = getChunkAtXZ(chunkXZ.x, chunkXZ.y);

		chunk->setActive(false);
		/*
		chunk->smoothed = false;

		auto mesh = chunk->getMesh();
		if (mesh)
		{
			mesh->clearBuffers();
		}

		*/
		// instead of removing, we keep it because now these chunks become edge.
	}

	// After deactivting front row, pop it from grid.
	activeChunks.pop_front();
}

void Voxel::ChunkMap::removeRowEast(ChunkWorkManager* wm)
{
	// Remove row east (positive x)

	//get x. Use max, not active chunk.
	int x = maxXZ.x;

	// Same for z. use min max to remove entire row from map, not on active chunks
	int zStart = minXZ.y;
	int zEnd = maxXZ.y;

	// Called by main thread. Iterate through entire row, clear buffer. These chunks will get removed from map. So add to finished queue.
	for (int z = zStart; z <= zEnd; z++)
	{
		std::shared_ptr<Chunk> chunk = getChunkAtXZ(x, z);

		// Make sure deactivates.
		chunk->setActive(false);

		auto mesh = chunk->getMesh();
		if (mesh)
		{
			// Clear mesh. This doesn't releases vao. 
			mesh->clearBuffers();
		}

		// Add to queue and let main thread to release it
		wm->addFinishedQueue(glm::ivec2(x, z));
	}

	// Before chunk map add new row on negative x (west), deactivate back row (east) from active chunk
	// We don't release mesh, there is no need to. Leave as generated, smoothed, structure added.
	for (auto& chunkXZ : activeChunks.back())
	{
		std::shared_ptr<Chunk> chunk = getChunkAtXZ(chunkXZ.x, chunkXZ.y);

		chunk->setActive(false);
		/*
		chunk->smoothed = false;

		auto mesh = chunk->getMesh();
		if (mesh)
		{
			mesh->clearBuffers();
		}
		*/

		// instead of removing, we keep it because now these chunks become edge.
	}

	// After deactivting back row, pop it from grid.
	activeChunks.pop_back();
}

void Voxel::ChunkMap::removeColSouth(ChunkWorkManager* wm)
{
	// Remove last element on each row (psotive z)

	// get x. Use min max because we are removing entire col
	int xStart = minXZ.x;
	int xEnd = maxXZ.x;

	// Get z. Use max z.
	int z = maxXZ.y;

	// Called by main thread. Iterate through entire col, clear buffer. These chunks will get removed from map. So add to finished queue.
	for (int x = xStart; x <= xEnd; x++)
	{
		std::shared_ptr<Chunk> chunk = getChunkAtXZ(x, z);

		// Make sure deactivates.
		chunk->setActive(false);

		auto mesh = chunk->getMesh();
		if (mesh)
		{
			// Clear mesh. This doesn't releases vao. 
			mesh->clearBuffers();
		}

		// Add to queue and let main thread to release it
		wm->addFinishedQueue(glm::ivec2(x, z));
	}

	// Before chunk map add new col on negative z (north), deactivate back col (south) from active chunk
	// We don't release mesh, there is no need to. Leave as generated, smoothed, structure added.
	for (auto& row : activeChunks)
	{
		// get back (south)
		auto chunkXZ = row.back();
		std::shared_ptr<Chunk> chunk = getChunkAtXZ(chunkXZ.x, chunkXZ.y);

		chunk->setActive(false);
		/*
		chunk->smoothed = false;

		auto mesh = chunk->getMesh();
		if (mesh)
		{
			mesh->clearBuffers();
		}
		*/

		// instead of removing, we keep it because now these chunks become edge.
	}

	// Pop last element on each row (south)
	for (auto& row : activeChunks)
	{
		row.pop_back();
	}
}

void Voxel::ChunkMap::removeColNorth(ChunkWorkManager * wm)
{
	// Remove first element on each row (negative z)

	// get x. Use min max because we are removing entire col
	int xStart = minXZ.x;
	int xEnd = maxXZ.x;

	// Get z. Use min z.
	int z = minXZ.y;

	// Called by main thread. Iterate through entire col, clear buffer. These chunks will get removed from map. So add to finished queue.
	for (int x = xStart; x <= xEnd; x++)
	{
		std::shared_ptr<Chunk> chunk = getChunkAtXZ(x, z);

		// Make sure deactivates.
		chunk->setActive(false);

		auto mesh = chunk->getMesh();
		if (mesh)
		{
			// Clear mesh. This doesn't releases vao. 
			mesh->clearBuffers();
		}

		// Add to queue and let main thread to release it
		wm->addFinishedQueue(glm::ivec2(x, z));
	}

	// Before chunk map add new col on postiive z (south), deactivate front col (north) from active chunk
	// We don't release mesh, there is no need to. Leave as generated, smoothed, structure added.
	for (auto& row : activeChunks)
	{
		// get front (north)
		auto chunkXZ = row.front();
		std::shared_ptr<Chunk> chunk = getChunkAtXZ(chunkXZ.x, chunkXZ.y);

		chunk->setActive(false);
		/*
		chunk->smoothed = false;

		auto mesh = chunk->getMesh();
		if (mesh)
		{
			mesh->clearBuffers();
		}
		*/

		// instead of removing, we keep it because now these chunks become edge.
	}

	// Pop first element on each row (north)
	for (auto& row : activeChunks)
	{
		row.pop_front();
	}
}

bool Voxel::ChunkMap::isChunkOnEdge(const glm::ivec2 & chunkXZ)
{
	// Check if chunk is on edge. Edge means end of render distance. Doesn't incldues extra rows and cols in active chunk.
	return chunkXZ.x == minXZ.x || chunkXZ.y == minXZ.y || chunkXZ.x == maxXZ.x || chunkXZ.y == maxXZ.y;
}

int Voxel::ChunkMap::findVisibleChunk()
{
	// Count number of visible chunk for debug
	int count = 0;

	// iterate chunk map
	for (auto& e : map)
	{
		auto chunk = e.second;
		if (chunk != nullptr)
		{
			auto chunkXZ = chunk->getCoordinate();
			if (isChunkOnEdge(chunkXZ))
			{
				// Don't render chunks that is out of render distance.
				continue;
			}
			else
			{
				if (chunk->isGenerated())
				{
					bool visible = Camera::mainCamera->getFrustum()->isChunkBorderInFrustum(chunk.get());
					chunk->setVisibility(visible);

					if (visible)
					{
						auto mesh = chunk->getMesh();

						if (mesh->isRenderable())
						{
							count++;
						}
					}
				}
			}
		}
	}

	return count;
}

int Voxel::ChunkMap::findVisibleChunk(std::vector<glm::ivec2>& visibleChunks)
{
	int count = 0;

	for (auto& e : map)
	{
		auto chunk = e.second;
		if (chunk != nullptr)
		{
			auto chunkXZ = chunk->getCoordinate();
			if (isChunkOnEdge(chunkXZ))
			{
				continue;
			}
			else
			{
				if (chunk->isGenerated())
				{
					bool visible = Camera::mainCamera->getFrustum()->isChunkBorderInFrustum(chunk.get());
					chunk->setVisibility(visible);

					if (visible)
					{
						count++;
						auto pos = chunk->getPosition();
						visibleChunks.push_back(glm::ivec2(pos.x, pos.z));
					}
				}
			}
		}
	}

	return count;
}

int Voxel::ChunkMap::findVisibleChunk(std::unordered_set<glm::ivec2, KeyFuncs, KeyFuncs>& visibleChunks)
{
	int count = 0;

	for (auto& e : map)
	{
		auto chunk = e.second;
		if (chunk != nullptr)
		{
			auto chunkXZ = chunk->getCoordinate();
			if (isChunkOnEdge(chunkXZ))
			{
				continue;
			}
			else
			{
				if (chunk->isGenerated())
				{
					bool visible = Camera::mainCamera->getFrustum()->isChunkBorderInFrustum(chunk.get());
					chunk->setVisibility(visible);

					if (visible)
					{
						count++;
						auto pos = chunk->getPosition();
						visibleChunks.emplace(glm::ivec2(pos.x, pos.z));
					}
				}
			}
		}
	}

	return count;
}

void Voxel::ChunkMap::setRenderChunkBorderMode(const bool mode)
{
	renderChunkBorderMode = mode;
}

void Voxel::ChunkMap::setRenderChunksMode(const bool mode)
{
	renderChunksMode = mode;
}

void Voxel::ChunkMap::setRenderBlockOutlineMode(const bool mode)
{
	renderBlockOutlineMode = mode;
}

void Voxel::ChunkMap::setUpdateChunkMapMode(const bool mode)
{
	updateChunksMode = mode;
}

void Voxel::ChunkMap::setRegionTerrainType(const unsigned int regionID, const Terrain & terrainType)
{
	if (regionTerrainsMap.find(regionID) == regionTerrainsMap.end())
	{
		regionTerrainsMap.emplace(regionID, terrainType);
	}
}

std::unordered_map<unsigned int, Terrain>& Voxel::ChunkMap::getRegionTerrainsMap()
{
	return regionTerrainsMap;
}

glm::ivec2 Voxel::ChunkMap::getCurrentChunkXZ()
{
	return currentChunkPos;
}

void Voxel::ChunkMap::getCollidableBlockNearPlayer(const glm::vec3 & playerPosition, std::vector<Block*>& collidableBlocks)
{
	auto standingBlockWorldPos = glm::ivec3(0);
	standingBlockWorldPos.x = static_cast<int>((playerPosition.x >= 0) ? playerPosition.x : glm::floor(playerPosition.x));
	standingBlockWorldPos.y = static_cast<int>((playerPosition.y >= 0) ? playerPosition.y : 0);
	standingBlockWorldPos.z = static_cast<int>((playerPosition.z >= 0) ? playerPosition.z : glm::floor(playerPosition.z));

	int startX = standingBlockWorldPos.x - 1;
	int startY = standingBlockWorldPos.y - 1;
	int startZ = standingBlockWorldPos.z - 1;
	int endX = standingBlockWorldPos.x + 1;
	int endY = standingBlockWorldPos.y + 3;
	int endZ = standingBlockWorldPos.z + 1;

	// max 54 blocks

	for (int x = startX; x <= endX; x++)
	{
		for (int z = startZ; z <= endZ; z++)
		{
			for (int y = startY; y <= endY; y++)
			{
				Block* block = getBlockAtWorldXYZ(x, y, z);
				if (block)
				{
					if (block->isCollidable())
					{
						collidableBlocks.push_back(block);
					}
				}
			}
		}
	}
}

void Voxel::ChunkMap::queryNearByCollidableBlocksInXZ(const glm::vec3 & playerPosition, std::vector<Block*>& collidableBlocks)
{
	auto standingBlockWorldPos = glm::ivec3(0);
	standingBlockWorldPos.x = static_cast<int>((playerPosition.x >= 0) ? playerPosition.x : glm::floor(playerPosition.x));
	standingBlockWorldPos.y = static_cast<int>((playerPosition.y >= 0) ? playerPosition.y : 0);
	standingBlockWorldPos.z = static_cast<int>((playerPosition.z >= 0) ? playerPosition.z : glm::floor(playerPosition.z));

	// Query only near by in XZ axis. Bottom of over top blocks aren't counted

	int startX = standingBlockWorldPos.x - 1;
	int startY = standingBlockWorldPos.y;
	int startZ = standingBlockWorldPos.z - 1;
	int endX = standingBlockWorldPos.x + 1;
	int endY = standingBlockWorldPos.y + 3;
	int endZ = standingBlockWorldPos.z + 1;

	for (int x = startX; x <= endX; x++)
	{
		for (int z = startZ; z <= endZ; z++)
		{
			for (int y = startY; y <= endY; y++)
			{
				Block* block = getBlockAtWorldXYZ(x, y, z);
				if (block)
				{
					if (block->isCollidable())
					{
						collidableBlocks.push_back(block);
					}
				}
			}
		}
	}
}

void Voxel::ChunkMap::queryBottomCollidableBlocksInY(const glm::vec3 & playerPosition, std::vector<Block*>& collidableBlocks)
{
	auto standingBlockWorldPos = glm::ivec3(0);
	standingBlockWorldPos.x = static_cast<int>((playerPosition.x >= 0) ? playerPosition.x : glm::floor(playerPosition.x));
	standingBlockWorldPos.y = static_cast<int>((playerPosition.y >= 0) ? playerPosition.y : 0);
	standingBlockWorldPos.z = static_cast<int>((playerPosition.z >= 0) ? playerPosition.z : glm::floor(playerPosition.z));

	// Query only near by in XZ axis. Bottom of over top blocks aren't counted

	int startX = standingBlockWorldPos.x - 1;
	int startY = standingBlockWorldPos.y - 1;
	int startZ = standingBlockWorldPos.z - 1;
	int endX = standingBlockWorldPos.x + 1;
	int endY = standingBlockWorldPos.y;
	int endZ = standingBlockWorldPos.z + 1;

	for (int x = startX; x <= endX; x++)
	{
		for (int z = startZ; z <= endZ; z++)
		{
			for (int y = startY; y <= endY; y++)
			{
				Block* block = getBlockAtWorldXYZ(x, y, z);
				if (block)
				{
					if (block->isCollidable())
					{
						collidableBlocks.push_back(block);
					}
				}
			}
		}
	}
}

void Voxel::ChunkMap::queryTopCollidableBlocksInY(const glm::vec3 & playerPosition, std::vector<Block*>& collidableBlocks)
{
	auto standingBlockWorldPos = glm::ivec3(0);
	standingBlockWorldPos.x = static_cast<int>((playerPosition.x >= 0) ? playerPosition.x : glm::floor(playerPosition.x));
	standingBlockWorldPos.y = static_cast<int>((playerPosition.y >= 0) ? playerPosition.y : 0);
	standingBlockWorldPos.z = static_cast<int>((playerPosition.z >= 0) ? playerPosition.z : glm::floor(playerPosition.z));

	// Query only near by in XZ axis. Bottom of over top blocks aren't counted

	int startX = standingBlockWorldPos.x - 1;
	int startY = standingBlockWorldPos.y + 2;
	int startZ = standingBlockWorldPos.z - 1;
	int endX = standingBlockWorldPos.x + 1;
	int endY = standingBlockWorldPos.y + 3;
	int endZ = standingBlockWorldPos.z + 1;

	for (int x = startX; x <= endX; x++)
	{
		for (int z = startZ; z <= endZ; z++)
		{
			for (int y = startY; y <= endY; y++)
			{
				Block* block = getBlockAtWorldXYZ(x, y, z);
				if (block)
				{
					if (block->isCollidable())
					{
						collidableBlocks.push_back(block);
					}
				}
			}
		}
	}
}

int Voxel::ChunkMap::getTopYAt(const glm::vec2 & position)
{
	glm::ivec3 blockLocalPos;
	glm::ivec3 chunkSectionPos;

	blockWorldCoordinateToLocalAndChunkSectionCoordinate(glm::ivec3(position.x, 0, position.y), blockLocalPos, chunkSectionPos);

	auto chunk = getChunkAtXZ(chunkSectionPos.x, chunkSectionPos.z);
	if (chunk)
	{
		return chunk->getTopY(blockLocalPos.x, blockLocalPos.z);
	}
	else
	{
		return -1;
	}
}

void Voxel::ChunkMap::render()
{
	if (renderChunksMode)
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
}

void Voxel::ChunkMap::renderChunkBorder(Program * program)
{
	if (renderChunkBorderMode)
	{
		glBindVertexArray(chunkBorderVao);

		program->setUniformMat4("modelMat", chunkBorderModelMat);

		glDrawArrays(GL_LINES, 0, chunkBorderLineSize);

		// For debug
		auto glView = Application::getInstance().getGLView();

		if (glView->doesCountDrawCalls())
		{
			glView->incrementDrawCall();
		}

		if (glView->doesCountVerticesSize())
		{
			glView->addVerticesSize(chunkBorderLineSize / 2);
		}
	}
}

void Voxel::ChunkMap::renderBlockOutline(Program * lineProgram, const glm::vec3& blockPosition)
{
	if (renderBlockOutlineMode)
	{
		glBindVertexArray(blockOutlineVao);

		glm::mat4 cubeMat = glm::translate(glm::mat4(1.0f), blockPosition);
		lineProgram->setUniformMat4("modelMat", cubeMat);

		glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

		// For debug
		auto glView = Application::getInstance().getGLView();

		if (glView->doesCountDrawCalls())
		{
			glView->incrementDrawCall();
		}

		if (glView->doesCountVerticesSize())
		{
			glView->addVerticesSize(12);
		}
	}
}

void Voxel::ChunkMap::printCurrentChunk()
{
	auto chunk = getChunkAtXZ(currentChunkPos.x, currentChunkPos.y);
	chunk->print();
}

void Voxel::ChunkMap::printChunkMap()
{
	for (int x = minXZ.x; x <= maxXZ.x; ++x)
	{
		for (int z = minXZ.y; z<= maxXZ.y; ++z)
		{
			auto chunk = map.find(glm::ivec2(x, z))->second;

			std::cout << "(" << x << ", " << z << ")" << ((chunk->isActive()) ? std::string("A") : std::string(" ")) << "\t";
		}
		std::cout << "\n";
	}

	std::cout << "\n";
}

void Voxel::ChunkMap::printActiveChunks()
{
	for (auto& row : activeChunks)
	{
		for (auto& chunkXZ : row)
		{
			std::cout << "(" << chunkXZ.x << ", " << chunkXZ.y << ")\t";
		}

		std::cout << "\n";
	}
}
