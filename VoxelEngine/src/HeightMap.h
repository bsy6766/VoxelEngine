#ifndef HEIGHT_MAP_H
#define HEIGHT_MAP_H

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

		float redistribution;
		float terrace;

		NoisePreset();
		NoisePreset(const float freq, const float octave1, const float octave2, const float octave3, const float octave4, const float octave5, const float octave6, const float octave1Mul, const float octave2Mul, const float octave3Mul, const float octave4Mul, const float octave5Mul, const float octave6Mul, const float redistribution, const float terrace);
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
			PLAIN,
			DESERT,
			OCEAN,
			DEBUG
		};
	private:
		HeightMap() = delete;
		~HeightMap() = delete;
	public:
		// Plain. Mostly flat 
		static const NoisePreset PlainPreset;
		// Desert. A bit flat with some low hills
		static const NoisePreset DesertPreset;
		// Ocean. Almost flat but with very few islands. 
		static const NoisePreset OceanPreset;
		// Custom for debug
		static const NoisePreset DebugPreset;

		static float getNoise2D(const float x, const float z, const PRESET preset);
	};
}

#endif