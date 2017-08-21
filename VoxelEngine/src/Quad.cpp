#include "Quad.h"

using namespace Voxel;

/*

				  y+
		1------------------3
		|				   |
		|				   |
		|				   |
  x+ 	|				   |  x- 
		|				   |
		|				   |
		0------------------2
				 y-
*/

const std::vector<unsigned int> Quad::indices =
{
	0, 1, 2, 1, 2, 3
};

const std::vector<float> Quad::defaultColors =
{
	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 1, 1, 1,
};

/*

		Vertex								UV coordinate
					(width, height)						(1,1)
		*------------*						*------------*
		|		     |						|			 |
		|			 |						|			 |
		|	 		 |						|			 |
		|			 |					  v	|			 |
		*------------*						*------------*
	(0,0)								(0,0)   u

*/

const std::vector<float> Quad::uv = 
{
	0, 0,
	0, 1,
	1, 0,
	1, 1
};

std::vector<float> Voxel::Quad::getVertices(const glm::vec2 & size)
{
	if (size.x <= 0 || size.y <= 0)
	{
		return std::vector<float>();
	}
	else
	{
		auto vertices = std::vector<float>();

		float widthHalf = size.x * 0.5f;
		float heightHalf = size.y * 0.5f;
		//float z = -514.13328242716122077782265459946f;

		// Add vertices from 0 to 4
		// 0
		vertices.push_back(widthHalf);
		vertices.push_back(-heightHalf);
		vertices.push_back(0);

		//1
		vertices.push_back(widthHalf);
		vertices.push_back(heightHalf);
		vertices.push_back(0);

		//2
		vertices.push_back(-widthHalf);
		vertices.push_back(-heightHalf);
		vertices.push_back(0);

		//3
		vertices.push_back(-widthHalf);
		vertices.push_back(heightHalf);
		vertices.push_back(0);

		return vertices;
	}
}

std::vector<float> Voxel::Quad::getColors(const glm::vec4 & color)
{
	auto colors = std::vector<float>();

	for (int i = 0; i < 4; i++)
	{
		colors.push_back(color.r);
		colors.push_back(color.g);
		colors.push_back(color.b);
		colors.push_back(color.a);
	}

	return colors;
}
