// pch
#include "PreCompiled.h"

#include "Chunk.h"

// voxel
#include "ChunkSection.h"
#include "ChunkMesh.h"
#include "Utility.h"
#include "HeightMap.h"
#include "ProgramManager.h"
#include "Program.h"

using namespace Voxel;

Chunk::Chunk()
	: position(0)
	, worldPosition(0.0f)
	, modelMat(1.0f)
	, chunkMesh(nullptr)
	, boundingBox(glm::vec3(0.0f), glm::vec3(0.0f))
	, active(false)
	, visible(false)
	, timestamp(0)
{
	chunkMesh = new ChunkMesh();

	// Just in case, init chunksections to nullptr
	for (unsigned int i = 0; i < chunkSections.size(); ++i)
	{
		chunkSections[i] = nullptr;
	}

	generated.store(false);
	smoothed.store(false);
	structureAdded.store(false);
	preGenerated.store(false);
	needNewMesh.store(false);
}

bool Voxel::Chunk::canGenerate()
{
	if (generated.load() == true)
	{
		std::cout << "Chunk trying to generate again\n";
		return false;
	}

	/*
	if (chunkSections.empty() == false)
	{
		std::cout << "Chunk already has chunksections\n";
		return false;
	}
	*/

	return true;
}

Chunk::~Chunk()
{
	for (auto chunkSection : chunkSections)
	{
		if (chunkSection)
		{
			delete chunkSection;
		}
	}
	
	if (chunkMesh)
	{
		delete chunkMesh;
	}

	//std::cout << "[Chunk] Deleting chunk at (" << position.x << ", " << position.z << ")...\n";
}

Chunk * Voxel::Chunk::createEmpty(const int x, const int z)
{
	Chunk* newChunk = new Chunk();
	if (newChunk->init(x, z))
	{
		//std::cout << "[Chunk] Done.\n\n";
		//std::cout << "[Chunk] Creating new empty chunk at (" << x << ", " << z << ")...\n";
		return newChunk;
	}

	// Failed
	delete newChunk;
	return nullptr;
}

void Voxel::Chunk::initRandomEngine(const std::string & worldSeed)
{
	randomEngine.seed(std::hash<std::string>{}(worldSeed + std::to_string(position.x) + std::to_string(position.z)));
}

void Voxel::Chunk::updateModelMat(const glm::vec3 & playerPosition)
{
	auto chunkWP = glm::vec3(worldPosition.x - Constant::CHUNK_BORDER_SIZE_HALF, 0.0f, worldPosition.z - Constant::CHUNK_BORDER_SIZE_HALF);

	modelMat = glm::translate(glm::mat4(1.0f), chunkWP - playerPosition);
}

void Voxel::Chunk::unload()
{
	if (chunkMesh)
	{
		delete chunkMesh;
		chunkMesh = nullptr;
	}
}

bool Chunk::init(const int x, const int z)
{
	//auto start = Utility::Time::now();
	position = glm::ivec3(x, 0, z);

	// calculate position. Size of each block is 1.0f. There are total 16 x 16 (256) blocks in XZ space.
	worldPosition.x = Constant::CHUNK_BORDER_SIZE * (static_cast<float>(x) + 0.5f);
	worldPosition.y = 0;
	worldPosition.z = Constant::CHUNK_BORDER_SIZE * (static_cast<float>(z) + 0.5f);
	
	//std::cout << "[Chunk] position: (" << x << ", 0, " << z << "), world position: (" << worldPosition.x << ", " << worldPosition.y << ", " << worldPosition.z << ")\n";

	// init border. worldPosition works as center position of border
	float borderDistance = (Constant::CHUNK_BORDER_SIZE * 0.5f);
	
	auto min = glm::vec3(worldPosition.x - borderDistance, 0, worldPosition.z - borderDistance);
	auto max = glm::vec3(worldPosition.x + borderDistance, Constant::TOTAL_CHUNK_SECTION_PER_CHUNK * Constant::CHUNK_SECTION_HEIGHT, worldPosition.z + borderDistance);

	auto size = max - min;

	boundingBox.center = min + (size * 0.5f);
	boundingBox.size = size;
	
	//std::cout << "[Chunk] BorderXZ: min(" << border.min.x << ", " << border.min.z << "), max(" << border.max.x << ", " << border.max.z << ")\n";

	//auto end = Utility::Time::now();
	//std::cout << "Chunk generation elapsed time: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
	return true;
}

bool Voxel::Chunk::generate()
{
	assert(canGenerate());

	// For terrain color variation. Repeating all same color for large amount of area gives bad visual. so use this noise to smoothly mix color
	std::vector<std::vector<float>> colorMap;
	HeightMap::getHeightMapForColor(position, colorMap);
	
	for (auto chunkSection : chunkSections)
	{
		if (chunkSection != nullptr)
		{
			chunkSection->init(heightMap, colorMap);
		}
	}

	//regionMap.clear();
	//heightMap.clear();

	generated.store(true);

	return true;
}

void Voxel::Chunk::generateChunkSections(const int minY, const int maxY)
{
	// Note: Switched chunkSections to std::array because chunkSections will be always have fixed size.
	/*

	// std::vector version. std::array can't be empty

	if (chunkSections.empty())
	{
		// Chunk sections are empty. Fill new.
		for (int i = 0; i < Constant::TOTAL_CHUNK_SECTION_PER_CHUNK; i++)
		{
			// Temp. All blocks above chunk section y 3 will be air.
			if (i > maxY || i < minY)
			{
				//std::cout << "+ chunksection: " << i << std::endl;
				chunkSections.push_back(nullptr);
			}
			else
			{
				//std::cout << "++ chunksection: " << i << std::endl;
				auto newChucnkSection = ChunkSection::createEmpty(position.x, i, position.z, worldPosition);
				if (newChucnkSection)
				{
					// Add new chunk section
					chunkSections.push_back(newChucnkSection);
				}
				else
				{
					throw std::runtime_error("Failed to create chunk section at (" + std::to_string(position.x) + ", " + std::to_string(i) + ", " + std::to_string(position.z) + ")");
				}
			}
		}
	}
	*/

	/*
	// std::vector version. std::array have fixed size
	// chunk section was generated before. Check size.
	if (chunkSections.size() == Constant::TOTAL_CHUNK_SECTION_PER_CHUNK)
	{
	}
	else
	{
		throw std::runtime_error("Invalid size of chunksection: " + std::to_string(chunkSections.size()));
	}
	*/

	// Iterate through chunk sections and initailize.
	for (int i = 0; i < Constant::TOTAL_CHUNK_SECTION_PER_CHUNK; i++)
	{
		if (i > maxY || i < minY)
		{
			// out of bound. skip.
			continue;
		}
		else
		{
			//std::cout << "++ chunksection: " << i << std::endl;
			// Check if chunk section is empty
			if (chunkSections.at(i) == nullptr)
			{
				auto newChucnkSection = ChunkSection::createEmpty(position.x, i, position.z, worldPosition);
				if (newChucnkSection)
				{
					// add new.
					chunkSections.at(i) = newChucnkSection;
				}
				else
				{
					throw std::runtime_error("Failed to create chunk section at (" + std::to_string(position.x) + ", " + std::to_string(i) + ", " + std::to_string(position.z) + ")");
				}
			}
			// else, chunk section exists. skip
		}
	}
}

glm::ivec3 Chunk::getPosition()
{
	return position;
}

glm::ivec2 Voxel::Chunk::getCoordinate()
{
	return glm::ivec2(position.x, position.z);
}

glm::vec3 Chunk::getWorldPosition()
{
	return worldPosition;
}

ChunkSection * Voxel::Chunk::getChunkSectionAtY(const int y)
{
	if (y >= 0 && y < static_cast<int>(chunkSections.size()))
	{
		if (chunkSections.at(y) == nullptr)
		{
			return nullptr;
		}
		else
		{
			return chunkSections.at(y);
		}
	}
	else
	{
		return nullptr;
	}
}

void Voxel::Chunk::createChunkSectionAtY(const int y)
{
	if (y < 0)
	{
		return;
	}
	else
	{
		if (y >= 0 && y < static_cast<int>(chunkSections.size()))
		{
			if (chunkSections.at(y) == nullptr)
			{
				auto newChucnkSection = ChunkSection::createEmpty(position.x, y, position.z, worldPosition);
				if (newChucnkSection)
				{
					chunkSections.at(y) = newChucnkSection;
				}
				else
				{
					throw std::runtime_error("Failed to add chunk section at (" + std::to_string(position.x) + ", " + std::to_string(y) + ", " + std::to_string(position.z) + ")");
				}
			}
		}
		else
		{
			return;
		}
	}
}

void Voxel::Chunk::deleteChunkSectionAtY(const int y)
{
	if (y >= 0 && y < static_cast<int>(chunkSections.size()))
	{
		if (chunkSections.at(y))
		{
			delete chunkSections.at(y);
			chunkSections.at(y) = nullptr;
		}
	}
	else
	{
		return;
	}
}

int Voxel::Chunk::findMaxY()
{
	int max = 0;
	for (auto& row : heightMap)
	{
		for (auto& val : row)
		{
			if (val > max)
			{
				max = val;
			}
		}
	}

	return max;
}

void Voxel::Chunk::render(const glm::vec3& playerPosition)
{
	auto program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::BLOCK_SHADER);

	if (chunkMesh)
	{
		if (chunkMesh->isRenderable())
		{
			// Ready to render
			bool result = chunkMesh->bind();
			if (result)
			{
				updateModelMat(playerPosition);
				program->setUniformMat4("modelMat", modelMat);
				chunkMesh->render();
			}
			else
			{
				assert(false);
			}
		}
		else
		{
			// mesh is not renderable. See if it can load
			if (chunkMesh->isBufferLoadable())
			{
				chunkMesh->loadBuffer(program);

				// Now ready to render
				bool result = chunkMesh->bind();
				if (result)
				{
					updateModelMat(playerPosition);
					program->setUniformMat4("modelMat", modelMat);
					chunkMesh->render();
				}
				else
				{
					assert(false);
				}
			}
			else
			{
				// Not renderable, buffer not ready.
				return;
			}
		}
	}
	// Doens't have chunk mesh. should be error
}

void Voxel::Chunk::setActive(const bool state)
{
	active = state;
}

bool Voxel::Chunk::isActive()
{
	return active;
}

void Voxel::Chunk::setVisibility(const bool visibility)
{
	visible = visibility;
}

bool Voxel::Chunk::isVisible()
{
	return visible;
}

void Voxel::Chunk::releaseMesh()
{
	if (chunkMesh)
	{
		// delete mesh
		chunkMesh->releaseVAO();
	}
}

ChunkMesh * Voxel::Chunk::getMesh()
{
	return chunkMesh;
}

Shape::AABB Voxel::Chunk::getBoundingBox()
{
	return boundingBox;
}

void Voxel::Chunk::setRegionMap(const unsigned int regionID)
{
	regionMap.clear();
	regionMap.push_back(regionID);
}

void Voxel::Chunk::setRegionMap(const std::vector<unsigned int>& regionIDs)
{
	regionMap.clear();
	regionMap = regionIDs;
}

void Voxel::Chunk::mergeHeightMap(std::vector<std::vector<int>>& plainHeightMap)
{
	const int rowSize = heightMap.size();
	const int colSize = heightMap.front().size();

	assert(rowSize == plainHeightMap.size());
	assert(colSize == plainHeightMap.front().size());

	for (int row = 0; row < rowSize; row++)
	{
		for (int col = 0; col < colSize; col++)
		{
			auto pH = plainHeightMap.at(row).at(col);
			auto h = heightMap.at(row).at(col);
			if (h < pH)
			{
				heightMap.at(row).at(col) = ((h + pH) / 2);
			}
		}
	}
}

bool Voxel::Chunk::isGenerated()
{
	return generated.load();
}

bool Voxel::Chunk::hasSingleRegion()
{
	return regionMap.size() == 1;
}

bool Voxel::Chunk::hasMultipleRegion()
{
	return regionMap.size() > 1;
}

unsigned int Voxel::Chunk::getFirstRegion()
{
	return regionMap.front();
}

int Voxel::Chunk::getQ11()
{
	return heightMap.front().front();
}

int Voxel::Chunk::getQ12()
{
	return heightMap.front().back();
}

int Voxel::Chunk::getQ21()
{
	return heightMap.back().front();
}

int Voxel::Chunk::getQ22()
{
	return heightMap.back().back();
}

bool Voxel::Chunk::isSmoothed()
{
	return smoothed.load();
}

int Voxel::Chunk::getTopY(const int localX, const int localZ)
{
	for (unsigned int i = chunkSections.size() - 1; i >= 0; --i)
	{
		if (chunkSections.at(i))
		{
			auto topLocalY = chunkSections.at(i)->getLocalTopY(localX, localZ);
			if (topLocalY == -1)
			{
				continue;
			}
			else
			{
				return topLocalY + (i * Constant::CHUNK_SECTION_HEIGHT);
			}
		}
		else
		{
			continue;
		}
	}

	return -1;
}

void Voxel::Chunk::updateTimestamp(const double timestamp)
{
	this->timestamp = timestamp;
}

void Voxel::Chunk::print()
{
	std::cout << "Chunk (" << position.x << ", " << position.z << ") info.\n";
	std::cout << "World position = " << Utility::Log::vec3ToStr(worldPosition) << std::endl;
	std::cout << "Active: " << (active ? "True" : "False") << std::endl;
	std::cout << "Visible: " << (visible ? "True" : "False") << std::endl;
	std::cout << "Generated: " << (generated.load() ? "True" : "False") << std::endl;

	if (regionMap.size() == 1)
	{
		std::cout << "Region: " << regionMap.front() << std::endl;
	}
	else
	{
		int c = 0;
		for (auto i : regionMap)
		{
			std::cout << i << " ";
			c++;
			if (c >= 16)
			{
				std::cout << std::endl;
				c = 0;
			}
		}
	}

	std::cout << "ChunkSections: " << chunkSections.size() << "\n";
}
