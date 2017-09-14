#include "HeightMap.h"
#include <Utility.h>
#include <ChunkUtil.h>

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
	, redistribution(1.0f)
	, terrace(1.0f)
	, applyRedist(true)
	, applyTerrace(false)
{}

NoisePreset::NoisePreset(const float freq, const float octave1, const float octave2, const float octave3, const float octave4, const float octave5, const float octave6, const float octave1Mul, const float octave2Mul, const float octave3Mul, const float octave4Mul, const float octave5Mul, const float octave6Mul, const float redistribution, const float terrace, const bool applyRedist, const bool applyTerrace)
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
	, redistribution(redistribution)
	, terrace(terrace)
	, applyRedist(applyRedist)
	, applyTerrace(applyTerrace)
{}




const NoisePreset HeightMap::PlainPreset =			NoisePreset(0.1f, 1.0f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.5f, 4.0f, 8.0f, 16.0f, 32.0f, 0.5f, 31.0f, true, false);
const NoisePreset HeightMap::DesertPreset =			NoisePreset();
const NoisePreset HeightMap::OceanPreset =			NoisePreset();
const NoisePreset HeightMap::DebugPreset =			NoisePreset(0.01f, 1.0f, 0.5f, 0.2f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 1.5f, 31.0f, true, false);
const NoisePreset HeightMap::TemperaturePreset =	NoisePreset(0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 1.0f, 0.0f, false, false);
const NoisePreset HeightMap::MoisturePreset =		NoisePreset(0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 1.0f, 0.0f, false, false);
const NoisePreset HeightMap::ColorPreset =			NoisePreset(0.15f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 1.0f, 0.0f, false, false);

float Voxel::HeightMap::getNoise(const NoisePreset* np, Noise::SimplexNoise* noisePtr, const float x, const float z)
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
		val = glm::round(val * np->terrace) / np->terrace;
	}

	val *= 0.5f;
	val += 0.25f;

	val = glm::clamp(val, 0.5f, 1.0f);

	return val;
}

float Voxel::HeightMap::getNoise2D(const float x, const float z, const PRESET preset)
{
	const NoisePreset * np = nullptr;

	Noise::SimplexNoise* worldNoise = Noise::Manager::getWorldNoise();

	switch (preset)
	{
	case PRESET::PLAIN:
		np = &HeightMap::PlainPreset;
		break;
	case PRESET::DEBUG:
		np = &HeightMap::DebugPreset;
		break;
	case PRESET::NONE:
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

	return getNoise(&ColorPreset, cNoise, x, z);
}

void Voxel::HeightMap::getHeightMapForChunk(const glm::vec3 & chunkPosition, int& maxChunkSectionY, int& minChunkSectionY, std::vector<std::vector<float>>& heightMap)
{
	int maxY = 0;
	int minY = 10000;

	int xStart = static_cast<int>(chunkPosition.x);
	int zStart = static_cast<int>(chunkPosition.z);
	int xEnd = xStart + Constant::CHUNK_SECTION_WIDTH;
	int zEnd = zStart + Constant::CHUNK_SECTION_LENGTH;

	float nx = static_cast<float>(chunkPosition.x);
	float nz = static_cast<float>(chunkPosition.z);
	const float step = 1.0f / Constant::CHUNK_BORDER_SIZE;

	heightMap.clear();

	for (int x = xStart; x < xEnd; x++)
	{
		heightMap.push_back(std::vector<float>());

		for (int z = zStart; z < zEnd; z++)
		{
			// Get height 
			float val = HeightMap::getNoise2D(nx, nz, HeightMap::PRESET::PLAIN);

			// The lowest block level is 30. The range of terrain in y axis is 120 (30 
			int y = (static_cast<int>(val * 60.0f) + 30);

			heightMap.back().push_back(val);

			if (y > maxY)
			{
				maxY = y;
			}
			else if (y < minY)
			{
				minY = y;
			}

			// Get temperature
			//temperatureMap.back().push_back(HeightMap::getTemperatureNoise2D(nx, nz));
			//moistureMap.back().push_back(HeightMap::getMoistureNosie2D(nx, nz));

			nz += step;
		}
		nx += step;
		nz = static_cast<float>(chunkPosition.z);
	}

	maxChunkSectionY = (maxY / Constant::CHUNK_SECTION_HEIGHT);

	minChunkSectionY = ((minY - 5) / Constant::CHUNK_SECTION_HEIGHT);

	if (minChunkSectionY < 0)
	{
		minChunkSectionY = 0;
	}
}

void Voxel::HeightMap::getHeightMapForColor(const glm::vec3 & chunkPosition, std::vector<std::vector<float>>& colorMap)
{

	int xStart = static_cast<int>(chunkPosition.x);
	int zStart = static_cast<int>(chunkPosition.z);
	int xEnd = xStart + Constant::CHUNK_SECTION_WIDTH;
	int zEnd = zStart + Constant::CHUNK_SECTION_LENGTH;

	float nx = static_cast<float>(chunkPosition.x);
	float nz = static_cast<float>(chunkPosition.z);
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

			// Get temperature
			//temperatureMap.back().push_back(HeightMap::getTemperatureNoise2D(nx, nz));
			//moistureMap.back().push_back(HeightMap::getMoistureNosie2D(nx, nz));

			nz += step;
		}
		nx += step;
		nz = static_cast<float>(chunkPosition.z);
	}
}
