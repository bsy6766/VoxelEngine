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

std::vector<float> Voxel::Quad::getVertices(const glm::vec2 & size, const glm::vec2 & position)
{
	if (size.x <= 0 || size.y <= 0)
	{
		return std::vector<float>();
	}
	else
	{
		auto vertices = std::vector<float>();

		float widthHalf = size.x * 0.5f + position.x;
		float heightHalf = size.y * 0.5f + position.y;

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
