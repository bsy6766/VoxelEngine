#include "Cube.h"

using namespace Voxel;

/*
	Cube indices numbers

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
const std::vector<std::vector<float>> Cube::vertices = {
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

const std::vector<std::vector<float>> Cube::allVertices = {
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

const std::vector<float>  Cube::FrontVertices = {
	// x, y, z
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
};

const std::vector<float> Cube::BackVertices = {
	// x, y, z
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f
};

const std::vector<float> Cube::LeftVertices = {
	// x, y, z
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f
};

const std::vector<float> Cube::RightVertices = {
	// x, y, z
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
};

const std::vector<float> Cube::TopVertices = {
	// x, y, z
	-0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, 0.5f
};

const std::vector<float> Cube::BottomVertices = {
	// x, y, z
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, 0.5f,
};

const std::vector<unsigned int> Cube::faceIndices = {
	0, 1, 2, 1, 2, 3
};

const std::vector<unsigned int> Cube::indices = {
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

std::vector<float> Voxel::Cube::getVertices()
{
	// build all
	std::vector<float> vertecies;

	for (auto vertex : allVertices)
	{
		for (auto point : vertex)
		{
			vertecies.push_back(point * 10.0f);
		}
	}

	return vertecies;
}

std::vector<float> Voxel::Cube::getVertices(Face face, const glm::vec3& translation)
{
	if (face == Cube::Face::NONE)
	{
		return std::vector<float>();
	}
	else if (face == Face::ALL)
	{
		return getVertices();
	}
	else
	{
		std::vector<float> faceVertices;

		if (face & Cube::Face::FRONT)
		{
			for (unsigned int i = 0; i<FrontVertices.size(); i+=3)
			{
				faceVertices.push_back(FrontVertices.at(i) + translation.x);
				faceVertices.push_back(FrontVertices.at(i + 1) + translation.y);
				faceVertices.push_back(FrontVertices.at(i + 2) + translation.z);
			}
		}

		if (face & Cube::Face::LEFT)
		{
			for (unsigned int i = 0; i<LeftVertices.size(); i += 3)
			{
				faceVertices.push_back(LeftVertices.at(i) + translation.x);
				faceVertices.push_back(LeftVertices.at(i + 1) + translation.y);
				faceVertices.push_back(LeftVertices.at(i + 2) + translation.z);
			}
		}

		if (face & Cube::Face::BACK)
		{
			for (unsigned int i = 0; i<BackVertices.size(); i += 3)
			{
				faceVertices.push_back(BackVertices.at(i) + translation.x);
				faceVertices.push_back(BackVertices.at(i + 1) + translation.y);
				faceVertices.push_back(BackVertices.at(i + 2) + translation.z);
			}
		}

		if (face & Cube::Face::RIGHT)
		{
			for (unsigned int i = 0; i<RightVertices.size(); i += 3)
			{
				faceVertices.push_back(RightVertices.at(i) + translation.x);
				faceVertices.push_back(RightVertices.at(i + 1) + translation.y);
				faceVertices.push_back(RightVertices.at(i + 2) + translation.z);
			}
		}

		if (face & Cube::Face::TOP)
		{
			for (unsigned int i = 0; i<TopVertices.size(); i += 3)
			{
				faceVertices.push_back(TopVertices.at(i) + translation.x);
				faceVertices.push_back(TopVertices.at(i + 1) + translation.y);
				faceVertices.push_back(TopVertices.at(i + 2) + translation.z);
			}
		}

		if (face & Cube::Face::BOTTOM)
		{
			for (unsigned int i = 0; i<BottomVertices.size(); i += 3)
			{
				faceVertices.push_back(BottomVertices.at(i) + translation.x);
				faceVertices.push_back(BottomVertices.at(i + 1) + translation.y);
				faceVertices.push_back(BottomVertices.at(i + 2) + translation.z);
			}
		}

		return faceVertices;
	}
}

std::vector<float> Voxel::Cube::getColors3(const Face face, glm::vec3 color)
{
	if (face == Cube::Face::NONE)
	{
		return std::vector<float>();
	}
	else
	{
		std::vector<float> colors;

		if (face & Cube::Face::FRONT)
		{
			glm::vec3 sideColor = color * 0.8f;
			for (int i = 0; i < 4; i++)
			{
				colors.push_back(sideColor.r);
				colors.push_back(sideColor.g);
				colors.push_back(sideColor.b);
			}
		}

		if (face & Cube::Face::LEFT)
		{
			glm::vec3 sideColor = color * 0.8f;
			for (int i = 0; i < 4; i++)
			{
				colors.push_back(sideColor.r);
				colors.push_back(sideColor.g);
				colors.push_back(sideColor.b);
			}
		}

		if (face & Cube::Face::BACK)
		{
			glm::vec3 sideColor = color * 0.8f;
			for (int i = 0; i < 4; i++)
			{
				colors.push_back(sideColor.r);
				colors.push_back(sideColor.g);
				colors.push_back(sideColor.b);
			}
		}

		if (face & Cube::Face::RIGHT)
		{
			glm::vec3 sideColor = color * 0.8f;
			for (int i = 0; i < 4; i++)
			{
				colors.push_back(sideColor.r);
				colors.push_back(sideColor.g);
				colors.push_back(sideColor.b);
			}
		}

		if (face & Cube::Face::TOP)
		{
			for (int i = 0; i < 4; i++)
			{
				colors.push_back(color.r);
				colors.push_back(color.g);
				colors.push_back(color.b);
			}
		}

		if (face & Cube::Face::BOTTOM)
		{
			glm::vec3 bottomColor = color * 0.4f;
			for (int i = 0; i < 4; i++)
			{
				colors.push_back(bottomColor.r);
				colors.push_back(bottomColor.g);
				colors.push_back(bottomColor.b);
			}
		}

		return colors;
	}
}

std::vector<float> Voxel::Cube::getColors4(const Face face, glm::vec4 color)
{
	if (face == Cube::Face::NONE)
	{
		return std::vector<float>();
	}
	else
	{
		std::vector<float> colors;

		if (face & Cube::Face::FRONT)
		{
			glm::vec3 sideColor = color * 0.8f;
			for (int i = 0; i < 4; i++)
			{
				colors.push_back(sideColor.r);
				colors.push_back(sideColor.g);
				colors.push_back(sideColor.b);
				colors.push_back(color.a);
			}
		}

		if (face & Cube::Face::LEFT)
		{
			glm::vec3 sideColor = color * 0.8f;
			for (int i = 0; i < 4; i++)
			{
				colors.push_back(sideColor.r);
				colors.push_back(sideColor.g);
				colors.push_back(sideColor.b);
				colors.push_back(color.a);
			}
		}

		if (face & Cube::Face::BACK)
		{
			glm::vec3 sideColor = color * 0.8f;
			for (int i = 0; i < 4; i++)
			{
				colors.push_back(sideColor.r);
				colors.push_back(sideColor.g);
				colors.push_back(sideColor.b);
				colors.push_back(color.a);
			}
		}

		if (face & Cube::Face::RIGHT)
		{
			glm::vec3 sideColor = color * 0.8f;
			for (int i = 0; i < 4; i++)
			{
				colors.push_back(sideColor.r);
				colors.push_back(sideColor.g);
				colors.push_back(sideColor.b);
				colors.push_back(color.a);
			}
		}

		if (face & Cube::Face::TOP)
		{
			for (int i = 0; i < 4; i++)
			{
				colors.push_back(color.r);
				colors.push_back(color.g);
				colors.push_back(color.b);
				colors.push_back(color.a);
			}
		}

		if (face & Cube::Face::BOTTOM)
		{
			glm::vec3 bottomColor = color * 0.4f;
			for (int i = 0; i < 4; i++)
			{
				colors.push_back(bottomColor.r);
				colors.push_back(bottomColor.g);
				colors.push_back(bottomColor.b);
				colors.push_back(color.a);
			}
		}

		return colors;
	}
}

std::vector<unsigned int> Voxel::Cube::getIndices(Face face, const int cubeOffset)
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
			return indices;
		}
		else
		{
			std::vector<unsigned int> newIndices;

			int num = countFaceBit(face);

			int offset = 0 + cubeOffset;

			for (int i = 0; i < num; i++)
			{
				for (int i = 0; i < 6; i++)
				{
					newIndices.push_back(faceIndices.at(i) + offset);
				}
				offset += 4;
			}

			return newIndices;
		}
	}
}