#ifndef CUBE_H
#define CUBE_H

#include <vector>

namespace Voxel
{
	/**
	*	@class Cube
	*	@brief Static class that provides cube verticies
	*/
	class Cube
	{
	public:
		// Faces bit enum.
		// |= sets bit, &= ~ resets bit
		enum Face : unsigned int
		{
			NONE = 0,			// 0 means no faces at all
			FRONT = 1 << 0,		// 0000 0001
			LEFT = 1 << 1,		// 0000 0010
			BACK = 1 << 2,		// 0000 0100
			RIGHT = 1 << 3,		// 0000 1000
			TOP = 1 << 4,		// 0001 0000
			BOTTOM = 1 << 5,	// 0010 0000
			ALL = 1 << 6,		// 0100 0000
		};

	private:
		// This is static class. 
		Cube() = delete;
		~Cube() = delete;

		static int countFaceBit(Face face);
	public:
		// Cube size of 1 oriented zero to positive
		//const static std::vector<float> verticies;
		const static std::vector<std::vector<float>> allVerticies;

		const static std::vector<float> FrontVerticies;
		const static std::vector<float> BackVerticies;
		const static std::vector<float> LeftVerticies;
		const static std::vector<float> RightVerticies;
		const static std::vector<float> TopVerticies;
		const static std::vector<float> BottomVerticies;

		const static std::vector<unsigned int> faceIndicies;
		const static std::vector<unsigned int> indicies;

		// Get cube verticies without any color
		static std::vector<float> getVerticies();
		// Get cube verticies on specific face
		static std::vector<float> getVerticies(Face face);
		// Get cube verticies with color
		static std::vector<float> getVerticies(Face face, float r, float g, float b);
		// Get cube indicies
		static std::vector<unsigned int> getIndicies(Face face, const int cubeOffset);
	};
}

#endif