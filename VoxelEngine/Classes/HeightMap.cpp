// pch
#include "PreCompiled.h"

#include "HeightMap.h"

// voxel
#include "Utility.h"
#include "ChunkUtil.h"
#include "Terrain.h"

using namespace Voxel;

NoisePreset::NoisePreset()
	: freq(1.0f)
	, octave1(1.0f)
	, octave2(0.5f)
	, octave3(0.2f)
	, octave4(0.0f)
	, octave5(0.0f)
	, octave6(0.0f)
	, octave1Mul(1.0f)
	, octave2Mul(2.0f)
	, octave3Mul(4.0f)
	, octave4Mul(8.0f)
	, octave5Mul(16.0f)
	, octave6Mul(32.0f)
	, shift(0)
	, amplify(1.0f)
	, redistribution(1.0f)
	, terrace(1.0f)
	, applyRedist(true)
	, applyTerrace(false)
{}

NoisePreset::NoisePreset(const float freq, const float octave1, const float octave2, const float octave3, const float octave4, const float octave5, const float octave6, const float octave1Mul, const float octave2Mul, const float octave3Mul, const float octave4Mul, const float octave5Mul, const float octave6Mul, const float shift, const float amplify, const float redistribution, const float terrace, const bool applyRedist, const bool applyTerrace)
	: freq(freq)
	, octave1(octave1)
	, octave2(octave2)
	, octave3(octave3)
	, octave4(octave4)
	, octave5(octave5)
	, octave6(octave6)
	, octave1Mul(octave1Mul)
	, octave2Mul(octave2Mul)
	, octave3Mul(octave3Mul)
	, octave4Mul(octave4Mul)
	, octave5Mul(octave5Mul)
	, octave6Mul(octave6Mul)
	, shift(shift)
	, amplify(amplify)
	, redistribution(redistribution)
	, terrace(terrace)
	, applyRedist(applyRedist)
	, applyTerrace(applyTerrace)
{}


const float HeightMap::freqScale = Constant::CHUNK_BORDER_SIZE / 16.0f;

// Frequency = Zoom in & out. Lower the value, it streches (zoom in) the height map. Higher the value, it shrinks (zoom out) the height map. 
// Redistribution = Lower value pushes middle elevation towards up. Higher values push middle elevation towards down.
// Amplify = Boost the height map value by multiply.
// Shift = Shift the height map value by addition. This is applied after amplification.

const NoisePreset HeightMap::PlainPreset =			NoisePreset(0.019f * HeightMap::freqScale, 1.0f, 0.5f, 0.4f, 0.3f, 0.2f, 0.0f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 33.0f, 30.0f, 0.5f, 31.0f, true, false);		// perfect plain terrain.

const NoisePreset HeightMap::HillsPreset =			NoisePreset(0.08f * HeightMap::freqScale, 1.0f, 0.8f, 0.7f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 33.0f, 30.0f, 0.7f, 31.0f, true, false);		// Some small hills

//const NoisePreset HeightMap::MountainsPreset =		NoisePreset(0.1f, 1.0f, 0.6f, 0.5f, 0.2f, 0.0f, 0.0f, 0.2f, 0.5f, 4.0f, 8.0f, 16.0f, 32.0f, 33.0f, 30.0f, 1.5f, 31.0f, true, false);	// small many mountains next to each other
const NoisePreset HeightMap::MountainsPreset =		NoisePreset(0.15f * HeightMap::freqScale, 1.0f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 50.0f, 30.0f, 0.9f, 31.0f, true, true);		// some small mountains

const NoisePreset HeightMap::TreePositionPreset =	NoisePreset(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 0.0f, 200.0f, 1.0f, 4.0f, false, false);

// for biome
const NoisePreset HeightMap::TemperaturePreset =	NoisePreset(0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 0.0f, 1.0f, 1.0f, 0.0f, false, false);
const NoisePreset HeightMap::MoisturePreset =		NoisePreset(0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 0.0f, 1.0f, 1.0f, 0.0f, false, false);

// for color mixing & blending
const NoisePreset HeightMap::ColorPreset =			NoisePreset(0.15f * HeightMap::freqScale, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 0.0f, 1.0f, 1.0f, 0.0f, false, false);

float Voxel::HeightMap::getNoise(const NoisePreset* np, Noise::SimplexNoise* noisePtr, const float x, const float z, const bool normalize)
{
	float val = 0;

	val = np->octave1 * noisePtr->noise(glm::vec2(((np->octave1Mul * x) + 0.5f) * np->freq, ((np->octave1Mul * z) + 0.5f) * np->freq))
		+ np->octave2 * noisePtr->noise(glm::vec2(((np->octave2Mul * x) + 0.5f) * np->freq, ((np->octave2Mul * z) + 0.5f) * np->freq))
		+ np->octave3 * noisePtr->noise(glm::vec2(((np->octave3Mul * x) + 0.5f) * np->freq, ((np->octave3Mul * z) + 0.5f) * np->freq))
		+ np->octave4 * noisePtr->noise(glm::vec2(((np->octave4Mul * x) + 0.5f) * np->freq, ((np->octave4Mul * z) + 0.5f) * np->freq))
		+ np->octave5 * noisePtr->noise(glm::vec2(((np->octave5Mul * x) + 0.5f) * np->freq, ((np->octave5Mul * z) + 0.5f) * np->freq))
		+ np->octave6 * noisePtr->noise(glm::vec2(((np->octave6Mul * x) + 0.5f) * np->freq, ((np->octave6Mul * z) + 0.5f) * np->freq));

	// So we devide by sum of np->octaves
	val /= (np->octave1 + np->octave2 + np->octave3 + np->octave4 + np->octave5 + np->octave6);

	// Now val should be in range of -1 ~ 1. Add 1 to make it in range of 0 ~ 2
	val += 1.0f;

	if (normalize)
	{
		// Fake normalize. Multiply 0.5f to make range of 0 ~ 1
		val *= 0.5f;
	}

	// Just in case if val is still 0, make it 0
	if (val < 0)
	{
		std::cout << "Val = " << val << std::endl;
		val = 0;
	}

	// Redistribution
	if (np->applyRedist)
	{
		val = powf(val, np->redistribution);
	}

	// terrace
	if (np->applyTerrace)
	{
		val = glm::floor(val * np->terrace) / np->terrace;
	}

	//val = glm::clamp(val, 0.0f, 2.0f);

	// Amplified
	val *= np->amplify;

	// Shifted
	val += np->shift;

	assert(val >= 0);

	return val;
}

float Voxel::HeightMap::getNoise2D(const float x, const float z, const PRESET preset, const bool normalize)
{
	const NoisePreset * np = nullptr;

	Noise::SimplexNoise* worldNoise = Noise::Manager::getWorldNoise();

	switch (preset)
	{
	case PRESET::PLAIN:
		np = &HeightMap::PlainPreset;
		break;
	case PRESET::HILLS:
		np = &HeightMap::HillsPreset;
		break;
	case PRESET::MOUNTAINS:
		np = &HeightMap::MountainsPreset;
		break;
	case PRESET::TREE:
		np = &HeightMap::TreePositionPreset;
		break;
	case PRESET::NONE:
	default:
		return 0;
		break;
	}

	float val = getNoise(np, worldNoise, x, z);

	return val;
}

float Voxel::HeightMap::getNoise2D(const float x, const float z, const Terrain & terrain, const bool normalize)
{
	const NoisePreset * np = nullptr;

	Noise::SimplexNoise* worldNoise = Noise::Manager::getWorldNoise();

	switch (terrain.getType())
	{
	case Voxel::TerrainType::PLAIN:
		np = &HeightMap::PlainPreset;
		break;
	case Voxel::TerrainType::HILLS:
		np = &HeightMap::HillsPreset;
		break;
	case Voxel::TerrainType::MOUNTAINS:
		np = &HeightMap::MountainsPreset;
		break;
	case Voxel::TerrainType::NONE:
	default:
		return 0;
		break;
	}

	float val = getNoise(np, worldNoise, x, z);

	return val;
}

float Voxel::HeightMap::getTemperatureNoise2D(const float x, const float z)
{
	Noise::SimplexNoise* tNoise = Noise::Manager::getTemperatureNoise();

	return getNoise(&TemperaturePreset, tNoise, x, z);
}

float Voxel::HeightMap::getMoistureNosie2D(const float x, const float z)
{
	Noise::SimplexNoise* mNoise = Noise::Manager::getMoistureNoise();

	return getNoise(&MoisturePreset, mNoise, x, z);
}

float Voxel::HeightMap::getColorNoise2D(const float x, const float z)
{
	Noise::SimplexNoise* cNoise = Noise::Manager::getColorNoise();

	// 0 ~ 2.0f
	float val = getNoise(&ColorPreset, cNoise, x, z);

	// 0 ~ 1.0f
	val *= 0.5f;

	float min = 0.2f;
	float max = 0.8f;

	float shiftedMin = min - min;
	float shiftedMax = max - min;

	float newVal = shiftedMax * val;

	return newVal + min;
}

int Voxel::HeightMap::getYFromHeightValue(const float value, const Voxel::TerrainType type)
{
	int y = 0;

	switch (type)
	{
	case Voxel::TerrainType::PLAIN:
	case Voxel::TerrainType::HILLS:
		y = static_cast<int>(value * 30.0f) + 33;	//33 = lowest sea level
		break;
	case Voxel::TerrainType::MOUNTAINS:
		y = static_cast<int>(value * 35.0f) + 33;
		break;
	default:
		break;
	}

	return y;
}

int Voxel::HeightMap::smoothHelper(std::vector<std::vector<int>>& heightMap, const unsigned int xStart, const unsigned int zStart, const unsigned int xEnd, const unsigned int zEnd)
{
	// https://en.wikipedia.org/wiki/Bilinear_interpolation#Algorithm

	float q11 = static_cast<float>(heightMap.at(xStart).at(zStart));
	float q12 = static_cast<float>(heightMap.at(xStart).at(zEnd - 1));
	float q21 = static_cast<float>(heightMap.at(xEnd - 1).at(zStart));
	float q22 = static_cast<float>(heightMap.at(xEnd - 1).at(zEnd - 1));

	float x1 = static_cast<float>(xStart);
	float x2 = static_cast<float>(xEnd);
	float z1 = static_cast<float>(zStart);
	float z2 = static_cast<float>(zEnd);

	float x21 = x2 - x1;

	int maxY = 0;

	for (unsigned int x = xStart; x < xEnd; ++x)
	{
		float xf = static_cast<float>(x);

		for (unsigned int z = zStart; z < zEnd; ++z)
		{
			if (x == xStart && z == zStart) continue;
			if (x == xStart && z == zEnd - 1) continue;
			if (x == xEnd - 1 && z == zStart) continue;
			if (x == xEnd - 1 && z == zEnd - 1) continue;

			float zf = static_cast<float>(z);

			float fxy1 = ((x2 - xf) / (x2 - x1) * q11) + ((xf - x1) / (x2 - x1) * q21);
			float fxy2 = ((x2 - xf) / (x2 - x1) * q12) + ((xf - x1) / (x2 - x1) * q22);

			float fxy = ((z2 - zf) / (z2 - z1) * fxy1) + ((zf - z1) / (z2 - z1) * fxy2);

			int val = static_cast<int>(glm::round(fxy));

			heightMap.at(x).at(z) = val;

			if (val > maxY)
			{
				maxY = val;
			}
		}
	}

	return maxY;
}

void Voxel::HeightMap::smoothHeightMap(std::vector<std::vector<int>>& heightMap, int& highestY)
{
	/*
	for (auto x : heightMap)
	{
		for (auto z : x)
		{
			std::cout << z << " ";
		}
		std::cout << std::endl;
	}
	*/

	//smoothHelper(heightMap, 0, 0, 8, 8);
	//smoothHelper(heightMap, 0, 8, 8, 16);
	//smoothHelper(heightMap, 8, 0, 16, 8);
	//smoothHelper(heightMap, 8, 8, 16, 16);

	//smoothHelper(heightMap, 4, 4, 12, 12);
	//smoothHelper(heightMap, 0, 0, 4, 4);
	//smoothHelper(heightMap, 0, 12, 4, 16);
	//smoothHelper(heightMap, 12, 0, 16, 4);
	//smoothHelper(heightMap, 12, 12, 16, 16);

	highestY = smoothHelper(heightMap, 0, 0, 16, 16);

	/*
	for (auto x : heightMap)
	{
		for (auto z : x)
		{
			std::cout << z << " ";
		}
		std::cout << std::endl;
	}
	*/
}

void Voxel::HeightMap::smoothHelper(std::vector<std::vector<int>>& heightMap, const int q11, const int q12, const int q21, const int q22, const unsigned int xStart, const unsigned int zStart, const unsigned int xEnd, const unsigned int zEnd)
{
	// https://en.wikipedia.org/wiki/Bilinear_interpolation#Algorithm

	float x1 = static_cast<float>(xStart);
	float x2 = static_cast<float>(xEnd);
	float z1 = static_cast<float>(zStart);
	float z2 = static_cast<float>(zEnd);

	const float x2_1 = x2 - x1;
	const float z2_1 = z2 - z1;

	//heighestY = 0;
	
	for (unsigned int x = xStart; x < xEnd; ++x)
	{
		float xf = static_cast<float>(x);

		for (unsigned int z = zStart; z < zEnd; ++z)
		{
			float zf = static_cast<float>(z);

			float fxy1 = ((x2 - xf) / (x2_1)* q11) + ((xf - x1) / (x2_1)* q21);
			float fxy2 = ((x2 - xf) / (x2_1)* q12) + ((xf - x1) / (x2_1)* q22);

			float fxy = ((z2 - zf) / (z2_1)* fxy1) + ((zf - z1) / (z2_1)* fxy2);

			int val = static_cast<int>(glm::round(fxy));

			heightMap.at(x).at(z) = val;

			/*
			if (val > heighestY)
			{
				heighestY = val;
			}
			*/
		}
	}
}

void Voxel::HeightMap::smoothHeightMap(std::vector<std::vector<int>>& heightMap, const int q11, const int q12, const int q21, const int q22, const int xLen, const int zLen)
{
	float x1 = 0;
	float x2 = static_cast<float>(xLen);
	float z1 = 0;
	float z2 = static_cast<float>(zLen);

	const float x2_1 = x2 - x1;
	const float z2_1 = z2 - z1;

	auto sizeX = heightMap.size();
	auto sizeZ = heightMap.front().size();

	for (unsigned int x = 0; x < sizeX; ++x)
	{
		float xf = static_cast<float>(x);

		for (unsigned int z = 0; z < sizeZ; ++z)
		{
			/*
			if (x == xStart && z == zStart) continue;
			if (x == xStart && z == zEnd - 1) continue;
			if (x == xEnd - 1 && z == zStart) continue;
			if (x == xEnd - 1 && z == zEnd - 1) continue;
			*/

			float zf = static_cast<float>(z);

			float fxy1 = ((x2 - xf) / (x2_1)* q11) + ((xf - x1) / (x2_1)* q21);
			float fxy2 = ((x2 - xf) / (x2_1)* q12) + ((xf - x1) / (x2_1)* q22);

			float fxy = ((z2 - zf) / (z2_1)* fxy1) + ((zf - z1) / (z2_1)* fxy2);

			heightMap.at(x).at(z) = static_cast<int>(glm::round(fxy));
		}
	}
}

void Voxel::HeightMap::generateHeightMapForChunk(const glm::vec3 & chunkPosition, std::vector<std::vector<int>>& heightMap, const std::vector<unsigned int>& regionMap, const std::unordered_map<unsigned int, Terrain>& regionTerrains)
{
	//int maxY = 0;
	//int minY = 10000;

	int xEnd = Constant::CHUNK_SECTION_WIDTH;
	int zEnd = Constant::CHUNK_SECTION_LENGTH;

	float nx = chunkPosition.x;
	float nz = chunkPosition.z;
	const float step = 1.0f / Constant::CHUNK_BORDER_SIZE;

	heightMap.clear();
	
	for (int x = 0; x < xEnd; x++)
	{
		heightMap.push_back(std::vector<int>());

		for (int z = 0; z < zEnd; z++)
		{
			auto index = static_cast<int>(x + (Constant::CHUNK_SECTION_WIDTH * z));

			auto regionID = regionMap.at(index);

			auto find_it = regionTerrains.find(regionID);
			if (find_it == regionTerrains.end())
			{
				throw std::runtime_error("Can't generate height map because region map and region terrains doesn't match. RegionID: " + std::to_string(regionID));
			}

			auto& terrain = find_it->second;

			// Get height 
			float val = HeightMap::getNoise2D(nx, nz, terrain);

			val = glm::round(val);

			// The lowest block level is 30. The range of terrain in y axis is 120 (30 
			//int y = HeightMap::getYFromHeightValue(val, terrain.getType());
			int y = static_cast<int>(val);
			heightMap.back().push_back(y);

			/*
			if (y > maxY)
			{
				maxY = y;
			}
			else if (y < minY)
			{
				minY = y;
			}
			*/

			nz += step;
		}

		nx += step;
		nz = chunkPosition.z;
	}

	//maxChunkSectionY = (maxY / Constant::CHUNK_SECTION_HEIGHT);

	/*
	//minY -= 16;

	if (minY < 0)
	{
		//minY = 0;
		minChunkSectionY = 0;
	}
	else
	{
		minChunkSectionY = (minY / Constant::CHUNK_SECTION_HEIGHT);
	}

	if (minChunkSectionY < 0)
	{
		minChunkSectionY = 0;
	}
	*/
}

void Voxel::HeightMap::generatePlainHeightMapForChunk(const glm::vec3 & chunkPosition, std::vector<std::vector<int>>& heightMap)
{
	int xEnd = Constant::CHUNK_SECTION_WIDTH;
	int zEnd = Constant::CHUNK_SECTION_LENGTH;

	float nx = static_cast<float>(chunkPosition.x);
	float nz = static_cast<float>(chunkPosition.z);
	const float step = 1.0f / Constant::CHUNK_BORDER_SIZE;

	heightMap.clear();

	for (int x = 0; x < xEnd; x++)
	{
		heightMap.push_back(std::vector<int>());

		for (int z = 0; z < zEnd; z++)
		{
			// Get height 
			float val = HeightMap::getNoise2D(nx, nz, PRESET::PLAIN);

			val = glm::round(val);

			heightMap.back().push_back(static_cast<int>(val));

			nz += step;
		}

		nx += step;
		nz = static_cast<float>(chunkPosition.z);
	}
}

void Voxel::HeightMap::getHeightMapForColor(const glm::vec3 & chunkPosition, std::vector<std::vector<float>>& colorMap)
{

	int xStart = static_cast<int>(chunkPosition.x);
	int zStart = static_cast<int>(chunkPosition.z);
	int xEnd = xStart + Constant::CHUNK_SECTION_WIDTH;
	int zEnd = zStart + Constant::CHUNK_SECTION_LENGTH;

	float nx = chunkPosition.x;
	float nz = chunkPosition.z;
	const float step = 1.0f / Constant::CHUNK_BORDER_SIZE;

	colorMap.clear();

	for (int x = xStart; x < xEnd; x++)
	{
		colorMap.push_back(std::vector<float>());

		for (int z = zStart; z < zEnd; z++)
		{
			// Get height 
			float val = HeightMap::getColorNoise2D(nx, nz);

			colorMap.back().push_back(val);

			nz += step;
		}
		nx += step;
		nz = chunkPosition.z;
	}
}

glm::ivec2 Voxel::HeightMap::getTreePosition(const glm::vec3 & chunkPosition)
{
	float nx = chunkPosition.x;
	float nz = chunkPosition.z;
	const float step = 1.0f / Constant::CHUNK_BORDER_SIZE;

	float max = 0;
	glm::ivec2 localPos;

	const int xEnd = Constant::CHUNK_SECTION_WIDTH;
	const int zEnd = Constant::CHUNK_SECTION_LENGTH;

	for (int x = 0; x < xEnd; x++)
	{
		for (int z = 0; z < zEnd; z++)
		{
			float val = glm::round(HeightMap::getNoise2D(nx, nz, HeightMap::PRESET::TREE));

			if (val > max)
			{
				max = val;
				localPos = glm::ivec2(x, z);
			}

			nz += step;
		}

		nx += step;
		nz = chunkPosition.z;
	}

	return localPos;
}
