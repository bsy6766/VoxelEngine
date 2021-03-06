#ifndef HEIGHT_MAP_H
#define HEIGHT_MAP_H

// cpp
#include <unordered_map>

// voxel
#include "SimplexNoise.h"
#include "Terrain.h"

namespace Voxel
{
	struct NoisePreset
	{
		float freq;	// Frequency

		// 6 layers of octave
		float octave1;
		float octave2;
		float octave3;
		float octave4;
		float octave5;
		float octave6;

		float octave1Mul;
		float octave2Mul;
		float octave3Mul;
		float octave4Mul;
		float octave5Mul;
		float octave6Mul;

		float shift;
		float amplify;
		float redistribution;
		float terrace;

		bool applyRedist;
		bool applyTerrace;

		NoisePreset();
		NoisePreset(const float freq, const float octave1, const float octave2, const float octave3, const float octave4, const float octave5, const float octave6, const float octave1Mul, const float octave2Mul, const float octave3Mul, const float octave4Mul, const float octave5Mul, const float octave6Mul, const float shift, const float amplify, const float redistribution, const float terrace, const bool applyRedist, const bool applyTerrace);
	};

	/**
	*	@class HeightMap
	*	@brief A static class that generates height map using simplex noise
	*/
	class HeightMap
	{
	public:
		enum class PRESET
		{
			NONE = 0,
			PLAIN,			// Flat terrain. No hills and mountains
			HILLS,			// Few hills in flat terrain
			MOUNTAINS,		// 
			BORDER,
			TREE
		};
	private:
		HeightMap() = delete;
		~HeightMap() = delete;

		// Returns value in range 0.0f ~ 2.0f
		static float getNoise(const NoisePreset* np, Noise::SimplexNoise* noisePtr, const float x, const float z, const bool normalize = false);
	public:
		static const float freqScale;
		// Terrain presets
		static const NoisePreset PlainPreset;
		static const NoisePreset HillsPreset;
		static const NoisePreset MountainsPreset;

		// Tree preset. We use high frequency with high amplitude. 
		static const NoisePreset TreePositionPreset;
		
		// Temperature preset
		static const NoisePreset TemperaturePreset;
		// Moisture preset
		static const NoisePreset MoisturePreset;
		// color rpeset
		static const NoisePreset ColorPreset;

		// Get noise 2d. Return noise value between 0 and 2.
		static float getNoise2D(const float x, const float z, const PRESET preset, const bool normalize = false);
		static float getNoise2D(const float x, const float z, const Terrain& terrain, const bool normalize = false);
		static float getTemperatureNoise2D(const float x, const float z);
		static float getMoistureNosie2D(const float x, const float z);
		static float getColorNoise2D(const float x, const float z);

		static int getYFromHeightValue(const float value, const Voxel::TerrainType type);

		static int smoothHelper(std::vector<std::vector<int>>& heightMap, const unsigned int xStart, const unsigned int zStart, const unsigned int xEnd, const unsigned int zEnd);
		static void smoothHeightMap(std::vector<std::vector<int>>& heightMap, int& highestY);

		static void smoothHelper(std::vector<std::vector<int>>& heightMap, const int q11, const int q12, const int q21, const int q22, const unsigned int xStart, const unsigned int zStart, const unsigned int xEnd, const unsigned int zEnd);
		static void smoothHeightMap(std::vector<std::vector<int>>& heightMap, const int q11, const int q12, const int q21, const int q22, const int xLen, const int zLen);

		static void generateHeightMapForChunk(const glm::vec3& chunkPosition, std::vector<std::vector<int>>& heightMap, const std::vector<unsigned int>& regionMap, const std::unordered_map<unsigned int, Terrain>& regionTerrains);
		static void generatePlainHeightMapForChunk(const glm::vec3& chunkPosition, std::vector<std::vector<int>>& heightMap);
		static void getHeightMapForColor(const glm::vec3& chunkPosition, std::vector<std::vector<float>>& colorMap);

		static glm::ivec2 getTreePosition(const glm::vec3& chunkPosition);
	};
}

#endif