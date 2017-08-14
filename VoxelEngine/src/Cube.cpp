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

const std::vector<std::vector<float>> Cube::allVerticies = {
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

const std::vector<float>  Cube::FrontVerticies = {
	// x, y, z
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
};

const std::vector<float> Cube::BackVerticies = {
	// x, y, z
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f
};

const std::vector<float> Cube::LeftVerticies = {
	// x, y, z
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f
};

const std::vector<float> Cube::RightVerticies = {
	// x, y, z
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
};

const std::vector<float> Cube::TopVerticies = {
	// x, y, z
	-0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, 0.5f
};

const std::vector<float> Cube::BottomVerticies = {
	// x, y, z
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, 0.5f,
};

const std::vector<unsigned int> Cube::faceIndicies = {
	0, 1, 2, 1, 2, 3
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

int Voxel::Cube::countFaceBit(Face face)
{
	int count = 0;
	unsigned int bit = static_cast<unsigned int>(face);

	while (bit != 0)
	{
		if (bit & 1) count++;
		bit >>= 1;
	}

	return count;
}

std::vector<float> Voxel::Cube::getVerticies()
{
	// build all
	std::vector<float> vertecies;

	for (auto vertex : allVerticies)
	{
		for (auto point : vertex)
		{
			vertecies.push_back(point * 10.0f);
		}
	}

	return vertecies;
}

std::vector<float> Voxel::Cube::getVerticies(Face face)
{
	if (face == Cube::Face::NONE)
	{
		return std::vector<float>();
	}
	else if (face == Face::ALL)
	{
		return getVerticies();
	}
	else
	{
		std::vector<float> faceVerticies;

		if (face & Cube::Face::FRONT)
		{
			for (auto vertex : FrontVerticies)
			{
				faceVerticies.push_back(vertex * 1.0f);
			}
		}

		if (face & Cube::Face::LEFT)
		{
			for (auto vertex : LeftVerticies)
			{
				faceVerticies.push_back(vertex* 1.0f);
			}
		}

		if (face & Cube::Face::BACK)
		{
			for (auto vertex : BackVerticies)
			{
				faceVerticies.push_back(vertex* 1.0f);
			}
		}

		if (face & Cube::Face::RIGHT)
		{
			for (auto vertex : RightVerticies)
			{
				faceVerticies.push_back(vertex* 1.0f);
			}
		}

		if (face & Cube::Face::TOP)
		{
			for (auto vertex : TopVerticies)
			{
				faceVerticies.push_back(vertex* 1.0f);
			}
		}

		if (face & Cube::Face::BOTTOM)
		{
			for (auto vertex : BottomVerticies)
			{
				faceVerticies.push_back(vertex* 1.0f);
			}
		}

		return faceVerticies;
	}
}

std::vector<float> Voxel::Cube::getVerticies(Face face, float r, float g, float b)
{
	std::vector<float> verticies;

	int counter = 0;

	for (auto vertex : allVerticies)
	{
		bool y = false;
		if (vertex.at(1) == 0) y = true;
		for (auto point : vertex)
		{
			verticies.push_back(point * 5.0f);
		}

		if (counter == 0)
		{
			verticies.push_back(1);
			verticies.push_back(0);
			verticies.push_back(0);
		}
		else if (counter == 1)
		{
			verticies.push_back(1);
			verticies.push_back(1);
			verticies.push_back(1);
		}
		else if (counter == 2)
		{
			verticies.push_back(0);
			verticies.push_back(1);
			verticies.push_back(0);
		}
		else if (counter == 3)
		{
			verticies.push_back(0.0f);
			verticies.push_back(0.0f);
			verticies.push_back(1.0f);
		}
		else
		{
			verticies.push_back(0.0f);
			verticies.push_back(1.0f);
			verticies.push_back(0.0f);
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

	return verticies;
}

std::vector<unsigned int> Voxel::Cube::getIndicies(Face face, const int cubeOffset)
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

			int num = countFaceBit(face);

			int offset = 0 + cubeOffset;

			for (int i = 0; i < num; i++)
			{
				for (int i = 0; i < 6; i++)
				{
					newIndicies.push_back(faceIndicies.at(i) + offset);
				}
				offset += 4;
			}

			return newIndicies;
		}
	}
}
