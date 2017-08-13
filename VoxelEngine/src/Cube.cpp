#include "Cube.h"

using namespace Voxel;

/*
const std::vector<float> Cube::verticies = {
    //  X     Y     Z       U     V
    // bottom
    -1.0f,-1.0f,-1.0f,   0.0f, 0.0f,
     1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
    -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
     1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
     1.0f,-1.0f, 1.0f,   1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,

    // top
    -1.0f, 1.0f,-1.0f,   0.0f, 0.0f,
    -1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
     1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
     1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
    -1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
     1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

    // front
    -1.0f,-1.0f, 1.0f,   1.0f, 0.0f,
     1.0f,-1.0f, 1.0f,   0.0f, 0.0f,
    -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,   0.0f, 0.0f,
     1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

    // back
    -1.0f,-1.0f,-1.0f,   0.0f, 0.0f,
    -1.0f, 1.0f,-1.0f,   0.0f, 1.0f,
     1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
     1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
    -1.0f, 1.0f,-1.0f,   0.0f, 1.0f,
     1.0f, 1.0f,-1.0f,   1.0f, 1.0f,

    // left
    -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
    -1.0f,-1.0f,-1.0f,   0.0f, 0.0f,
    -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,   1.0f, 0.0f,

    // right
     1.0f,-1.0f, 1.0f,   1.0f, 1.0f,
     1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
     1.0f, 1.0f,-1.0f,   0.0f, 0.0f,
     1.0f,-1.0f, 1.0f,   1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,   0.0f, 0.0f,
     1.0f, 1.0f, 1.0f,   0.0f, 1.0f
};

/*
	Quad indicies example

	  1 *--------* 3
		|\		 |
		|  \	 |
		|    \	 |
		|      \ |
	  0 *--------* 2

	  0 -> 1 -> 2 -> 1 -> 2 -> 3
*/

/*
	Cube indicies numbers

					Top
							Back
			   7---------5
			  /|        /|
			 / |       / |
			1--|------3  |
	Right	|  |      |  |  Left
			|  |      |  |
			|  6------|--4
			| /       | /
			0---------2
		Front
			    Bottom
*/

/*
	X, Y, Z axis
			x+	y+	z+
			 \  |  /
			  \ | /
			   \|/
				*
			   /|\
			  / | \
			 /  |  \
			z-  y- x-
*/


/*
const std::vector<std::vector<float>> Cube::verticies = {
	// x, y, z
	{ 0.0f, -1.0f, 0.0f, },
	{ 0.0, 0.0, 0.0f, },
	{ 1.0f, -1.0f, 0.0f, },
	{ 1.0f, 0.0f, 0.0f, },
	{ 1.0f, -1.0f, 1.0f, },
	{ 1.0f, 0.0f, 1.0f, },
	{ 0.0f, -1.0f, 1.0f, },
	{ 0.0f, 0.0f, 1.0f },
};
*/

const std::vector<std::vector<float>> Cube::verticies = {
	// x, y, z
	{ -0.5f, -0.5f, -0.5f, },
	{ -0.5f, 0.5f, -0.5f, },
	{ 0.5f, -0.5f, -0.5f, },
	{ 0.5f, 0.5f, -0.5f, },
	{ 0.5f, -0.5f, 0.5f, },
	{ 0.5f, 0.5f, 0.5f, },
	{ -0.5f, -0.5f, 0.5f, },
	{ -0.5f, 0.5f, 0.5f },
};

const std::vector<unsigned int> Cube::indicies = {
	// Front
	0, 1, 2, 1, 2, 3,
	// Left
	2, 3, 4, 3, 4, 5,
	// Back
	4, 5, 6, 5, 6, 7,
	// Right
	6, 7, 0, 7, 0, 1,
	// Top
	1, 7, 3, 7, 3, 5,
	// Bot
	0, 6, 2, 6, 2, 4
};

std::vector<float> Voxel::Cube::getVerticies()
{
	// build all
	std::vector<float> allVerticies;

	for (auto vertex : verticies)
	{
		for (auto point : vertex)
		{
			allVerticies.push_back(point * 50.0f);
		}
	}

	return allVerticies;
}

std::vector<float> Voxel::Cube::getVerticies(Face face, float r, float g, float b)
{
	std::vector<float> allVerticies;

	int counter = 0;

	for (auto vertex : verticies)
	{
		bool y = false;
		if (vertex.at(1) == 0) y = true;
		for (auto point : vertex)
		{
			allVerticies.push_back(point * 25.0f);
		}

		if (counter == 0)
		{
			allVerticies.push_back(1);
			allVerticies.push_back(0);
			allVerticies.push_back(0);
		}
		else if (counter == 1)
		{
			allVerticies.push_back(1);
			allVerticies.push_back(1);
			allVerticies.push_back(1);
		}
		else if (counter == 2)
		{
			allVerticies.push_back(0);
			allVerticies.push_back(1);
			allVerticies.push_back(0);
		}
		else if (counter == 3)
		{
			allVerticies.push_back(0.0f);
			allVerticies.push_back(0.0f);
			allVerticies.push_back(1.0f);
		}
		else
		{
			allVerticies.push_back(0.0f);
			allVerticies.push_back(1.0f);
			allVerticies.push_back(0.0f);
		}

		/*
		if (counter < 4)
		{
			allVerticies.push_back(1);
			allVerticies.push_back(y ? 1 : 0);
			allVerticies.push_back(0);
		}
		else
		{
			allVerticies.push_back(0.0f);
			allVerticies.push_back(y ? 1 : 0);
			allVerticies.push_back(1.0f);
		}
		*/

		counter++;
	}

	return allVerticies;
}

std::vector<unsigned int> Voxel::Cube::getIndicies(Face face)
{
	if (face == Cube::Face::NONE)
	{
		return std::vector<unsigned int>();
	}
	else
	{
		if (face == Cube::Face::ALL)
		{
			// Get all 
			return indicies;
		}
		else
		{
			std::vector<unsigned int> newIndicies;

			if (face & Cube::Face::FRONT)
			{
				for (int i = 0; i < 6; i++)
				{
					newIndicies.push_back(indicies.at(i));
				}
			}

			if (face & Cube::Face::LEFT)
			{
				for (int i = 6; i < 12; i++)
				{
					newIndicies.push_back(indicies.at(i));
				}
			}

			if (face & Cube::Face::BACK)
			{
				for (int i = 12; i < 18; i++)
				{
					newIndicies.push_back(indicies.at(i));
				}
			}

			if (face & Cube::Face::RIGHT)
			{
				for (int i = 18; i < 24; i++)
				{
					newIndicies.push_back(indicies.at(i));
				}
			}

			if (face & Cube::Face::TOP)
			{
				for (int i = 24; i < 30; i++)
				{
					newIndicies.push_back(indicies.at(i));
				}
			}

			if (face & Cube::Face::BOTTOM)
			{
				for (int i = 30; i < 36; i++)
				{
					newIndicies.push_back(indicies.at(i));
				}
			}

			return newIndicies;
		}
	}
}
