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
		// Faces bit enum
		enum Face : unsigned int
		{
			NONE = 0,			// 0 means no faces at all
			FRONT = 1 << 0,
			LEFT = 1 << 1,
			BACK = 1 << 2,
			RIGHT = 1 << 3,
			TOP = 1 << 4,
			BOTTOM = 1 << 5,
			ALL = 1 << 6,
		};

	private:

	public:
		Cube() = delete;
		~Cube() = delete;
		// Cube size of 1 oriented zero to positive
		//const static std::vector<float> verticies;
		const static std::vector<std::vector<float>> verticies;
		const static std::vector<unsigned int> indicies;

		// Get cube verticies without any color
		static std::vector<float> getVerticies();
		// Get cube verticies on specific face
		static std::vector<float> getVerticies(Face face);
		// Get cube verticies with color
		static std::vector<float> getVerticies(Face face, float r, float g, float b);
		// Get cube indicies
		static std::vector<unsigned int> getIndicies(Face face);
	};
}

#endif