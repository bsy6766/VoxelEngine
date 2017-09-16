#include "Cube.h"

using namespace Voxel;

/*
	Cube indices numbers

					Top
							Back
			   7---------5
			  /|        /|
			 / |       / |
			3--|------1  |
	Right	|  |      |  |  Left
			|  |      |  |
			|  6------|--4
			| /       | /
			2---------0
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

const std::vector<std::vector<float>> Cube::allVertices = {
	// x, y, z
	{ -0.5f, -0.5f, -0.5f, },	// 0
	{ -0.5f, 0.5f, -0.5f, },	// 1
	{ 0.5f, -0.5f, -0.5f, },	// 2
	{ 0.5f, 0.5f, -0.5f, },		// 3
	{ -0.5f, -0.5f, 0.5f, },	// 4
	{ -0.5f, 0.5f, 0.5f },		// 5
	{ 0.5f, -0.5f, 0.5f, },		// 6
	{ 0.5f, 0.5f, 0.5f, },		// 7
};

const std::vector<glm::vec3> Cube::allVerticesGlmVec3 = {
	// x, y, z
	glm::vec3(-0.5f, -0.5f, -0.5f),	// 0
	glm::vec3(-0.5f, 0.5f, -0.5f),	// 1
	glm::vec3(0.5f, -0.5f, -0.5f),	// 2
	glm::vec3(0.5f, 0.5f, -0.5f),	// 3
	glm::vec3(-0.5f, -0.5f, 0.5f),	// 4
	glm::vec3(-0.5f, 0.5f, 0.5),	// 5
	glm::vec3(0.5f, -0.5f, 0.5f),	// 6
	glm::vec3(0.5f, 0.5f, 0.5f),	// 7
};

const std::vector<float>  Cube::FrontVertices = {
	// x, y, z
	-0.5f, -0.5f, -0.5f,	// 0
	-0.5f, 0.5f, -0.5f,		// 1
	0.5f, -0.5f, -0.5f,		// 2
	0.5f, 0.5f, -0.5f,		// 3
};

const std::vector<float> Cube::BackVertices = {
	// x, y, z
	0.5f, -0.5f, 0.5f,		// 6
	0.5f, 0.5f, 0.5f,		// 7
	-0.5f, -0.5f, 0.5f,		// 4
	-0.5f, 0.5f, 0.5f		// 5
};

const std::vector<float> Cube::LeftVertices = {
	// x, y, z
	-0.5f, -0.5f, 0.5f,		// 4
	-0.5f, 0.5f, 0.5f,		// 5
	-0.5f, -0.5f, -0.5f,	// 0 
	-0.5f, 0.5f, -0.5f,		// 1
};

const std::vector<float> Cube::RightVertices = {
	// x, y, z
	0.5f, -0.5f, -0.5f,		// 2
	0.5f, 0.5f, -0.5f,		// 3
	0.5f, -0.5f, 0.5f,		// 6
	0.5f, 0.5f, 0.5f,		// 7
};

const std::vector<float> Cube::TopVertices = {
	// x, y, z
	-0.5f, 0.5f, -0.5f,		// 1
	-0.5f, 0.5f, 0.5f,		// 5
	0.5f, 0.5f, -0.5f,		// 3
	0.5f, 0.5f, 0.5f		// 7
};

const std::vector<float> Cube::BottomVertices = {
	// x, y, z
	-0.5f, -0.5f, -0.5f,	// 0
	-0.5f, -0.5f, 0.5f,		// 4
	0.5f, -0.5f, -0.5f,		// 2
	0.5f, -0.5f, 0.5f,		// 6
};

const std::vector<float>  Cube::FrontNormals = {
	// x, y, z
	-0.5f, -0.5f, -1.5f,
	-0.5f, 0.5f, -1.5f,
	0.5f, -0.5f, -1.5f,
	0.5f, 0.5f, -1.5f,
};

const std::vector<float> Cube::BackNormals = {
	// x, y, z
	0.5f, -0.5f, 1.5f,
	0.5f, 0.5f, 1.5f,
	-0.5f, -0.5f, 1.5f,
	-0.5f, 0.5f, 1.5f
};

const std::vector<float> Cube::LeftNormals = {
	// x, y, z
	-1.5f, -0.5f, 0.5f,
	-1.5f, 0.5f, 0.5f,
	-1.5f, -0.5f, -0.5f,
	-1.5f, 0.5f, -0.5f,
};

const std::vector<float> Cube::RightNormals = {
	// x, y, z
	1.5f, -0.5f, -0.5f,
	1.5f, 0.5f, -0.5f,
	1.5f, -0.5f, 0.5f,
	1.5f, 0.5f, 0.5f,
};

const std::vector<float> Cube::TopNormals = {
	// x, y, z
	-0.5f, 1.5f, -0.5f,
	-0.5f, 1.5f, 0.5f,
	0.5f, 1.5f, -0.5f,
	0.5f, 1.5f, 0.5f
};

const std::vector<float> Cube::BottomNormals = {
	// x, y, z
	-0.5f, -1.5f, -0.5f,
	-0.5f, -1.5f, 0.5f,
	0.5f, -1.5f, -0.5f,
	0.5f, -1.5f, 0.5f,
};

const std::vector<unsigned int> Cube::faceIndices = {
	0, 1, 2, 1, 2, 3
};

const std::vector<unsigned int> Cube::indices = {
	// Front
	0, 1, 2, 1, 2, 3,
	// Left
	0, 1, 4, 1, 4, 5,
	// Back
	4, 5, 6, 5, 6, 7,
	// Right
	2, 3, 6, 3, 6, 7,
	// Top
	1, 5, 3, 5, 3, 7,
	// Bot
	0, 4, 2, 4, 2, 6
};

const float Cube::TopShadeRatio = 1.0f;
const float Cube::FrontAndBackShadeRatio = 0.7f;
const float Cube::LeftAndRightShadeRatio = 0.55f;
const float Cube::BottomShadeRatio = 0.4f;
const float Cube::ShadePower = 0.05f;






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
			vertecies.push_back(point);
		}
	}

	return vertecies;
}

std::vector<float> Voxel::Cube::getVertices(const float size)
{
	std::vector<float> vertecies;

	for (auto vertex : allVertices)
	{
		for (auto point : vertex)
		{
			vertecies.push_back(point * size);
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

unsigned int Voxel::Cube::getVertices(Face face, const glm::vec3 & translation, std::vector<float>& vertices)
{
	unsigned int count = 0;
	if (face == Cube::Face::NONE)
	{
		return 0;
	}
	else if (face == Face::ALL)
	{
		assert(false);
		//Todo: Implement here.
		return 0;
	}
	else
	{
		if (face & Cube::Face::FRONT)
		{
			for (unsigned int i = 0; i<FrontVertices.size(); i += 3)
			{
				vertices.push_back(FrontVertices.at(i) + translation.x);
				vertices.push_back(FrontVertices.at(i + 1) + translation.y);
				vertices.push_back(FrontVertices.at(i + 2) + translation.z);
				count++;
			}
		}

		if (face & Cube::Face::LEFT)
		{
			for (unsigned int i = 0; i<LeftVertices.size(); i += 3)
			{
				vertices.push_back(LeftVertices.at(i) + translation.x);
				vertices.push_back(LeftVertices.at(i + 1) + translation.y);
				vertices.push_back(LeftVertices.at(i + 2) + translation.z);
				count++;
			}
		}

		if (face & Cube::Face::BACK)
		{
			for (unsigned int i = 0; i<BackVertices.size(); i += 3)
			{
				vertices.push_back(BackVertices.at(i) + translation.x);
				vertices.push_back(BackVertices.at(i + 1) + translation.y);
				vertices.push_back(BackVertices.at(i + 2) + translation.z);
				count++;
			}
		}

		if (face & Cube::Face::RIGHT)
		{
			for (unsigned int i = 0; i<RightVertices.size(); i += 3)
			{
				vertices.push_back(RightVertices.at(i) + translation.x);
				vertices.push_back(RightVertices.at(i + 1) + translation.y);
				vertices.push_back(RightVertices.at(i + 2) + translation.z);
				count++;
			}
		}

		if (face & Cube::Face::TOP)
		{
			for (unsigned int i = 0; i<TopVertices.size(); i += 3)
			{
				vertices.push_back(TopVertices.at(i) + translation.x);
				vertices.push_back(TopVertices.at(i + 1) + translation.y);
				vertices.push_back(TopVertices.at(i + 2) + translation.z);
				count++;
			}
		}

		if (face & Cube::Face::BOTTOM)
		{
			for (unsigned int i = 0; i<BottomVertices.size(); i += 3)
			{
				vertices.push_back(BottomVertices.at(i) + translation.x);
				vertices.push_back(BottomVertices.at(i + 1) + translation.y);
				vertices.push_back(BottomVertices.at(i + 2) + translation.z);
				count++;
			}
		}
	}

	return count;
}

std::vector<float> Voxel::Cube::getNormals(Face face, const glm::vec3 & translation)
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
		std::vector<float> normals;

		if (face & Cube::Face::FRONT)
		{
			for (unsigned int i = 0; i < FrontNormals.size(); i += 3)
			{
				normals.push_back(FrontNormals.at(i) + translation.x);
				normals.push_back(FrontNormals.at(i + 1) + translation.y);
				normals.push_back(FrontNormals.at(i + 2) + translation.z);
			}
		}

		if (face & Cube::Face::LEFT)
		{
			for (unsigned int i = 0; i < LeftNormals.size(); i += 3)
			{
				normals.push_back(LeftNormals.at(i) + translation.x);
				normals.push_back(LeftNormals.at(i + 1) + translation.y);
				normals.push_back(LeftNormals.at(i + 2) + translation.z);
			}
		}

		if (face & Cube::Face::BACK)
		{
			for (unsigned int i = 0; i < BackNormals.size(); i += 3)
			{
				normals.push_back(BackNormals.at(i) + translation.x);
				normals.push_back(BackNormals.at(i + 1) + translation.y);
				normals.push_back(BackNormals.at(i + 2) + translation.z);
			}
		}

		if (face & Cube::Face::RIGHT)
		{
			for (unsigned int i = 0; i<RightNormals.size(); i += 3)
			{
				normals.push_back(RightNormals.at(i) + translation.x);
				normals.push_back(RightNormals.at(i + 1) + translation.y);
				normals.push_back(RightNormals.at(i + 2) + translation.z);
			}
		}

		if (face & Cube::Face::TOP)
		{
			for (unsigned int i = 0; i<TopNormals.size(); i += 3)
			{
				normals.push_back(TopNormals.at(i) + translation.x);
				normals.push_back(TopNormals.at(i + 1) + translation.y);
				normals.push_back(TopNormals.at(i + 2) + translation.z);
			}
		}

		if (face & Cube::Face::BOTTOM)
		{
			for (unsigned int i = 0; i<BottomNormals.size(); i += 3)
			{
				normals.push_back(BottomNormals.at(i) + translation.x);
				normals.push_back(BottomNormals.at(i + 1) + translation.y);
				normals.push_back(BottomNormals.at(i + 2) + translation.z);
			}
		}

		return normals;
	}
}

void Voxel::Cube::getNormals(Face face, const glm::vec3 & translation, std::vector<float>& normals)
{
	if (face == Cube::Face::NONE)
	{
		return;
	}
	else if (face == Face::ALL)
	{
		assert(false);
		// Todo: implement here.
		return;
	}
	else
	{
		if (face & Cube::Face::FRONT)
		{
			for (unsigned int i = 0; i < FrontNormals.size(); i += 3)
			{
				normals.push_back(FrontNormals.at(i) + translation.x);
				normals.push_back(FrontNormals.at(i + 1) + translation.y);
				normals.push_back(FrontNormals.at(i + 2) + translation.z);
			}
		}

		if (face & Cube::Face::LEFT)
		{
			for (unsigned int i = 0; i < LeftNormals.size(); i += 3)
			{
				normals.push_back(LeftNormals.at(i) + translation.x);
				normals.push_back(LeftNormals.at(i + 1) + translation.y);
				normals.push_back(LeftNormals.at(i + 2) + translation.z);
			}
		}

		if (face & Cube::Face::BACK)
		{
			for (unsigned int i = 0; i < BackNormals.size(); i += 3)
			{
				normals.push_back(BackNormals.at(i) + translation.x);
				normals.push_back(BackNormals.at(i + 1) + translation.y);
				normals.push_back(BackNormals.at(i + 2) + translation.z);
			}
		}

		if (face & Cube::Face::RIGHT)
		{
			for (unsigned int i = 0; i<RightNormals.size(); i += 3)
			{
				normals.push_back(RightNormals.at(i) + translation.x);
				normals.push_back(RightNormals.at(i + 1) + translation.y);
				normals.push_back(RightNormals.at(i + 2) + translation.z);
			}
		}

		if (face & Cube::Face::TOP)
		{
			for (unsigned int i = 0; i<TopNormals.size(); i += 3)
			{
				normals.push_back(TopNormals.at(i) + translation.x);
				normals.push_back(TopNormals.at(i + 1) + translation.y);
				normals.push_back(TopNormals.at(i + 2) + translation.z);
			}
		}

		if (face & Cube::Face::BOTTOM)
		{
			for (unsigned int i = 0; i<BottomNormals.size(); i += 3)
			{
				normals.push_back(BottomNormals.at(i) + translation.x);
				normals.push_back(BottomNormals.at(i + 1) + translation.y);
				normals.push_back(BottomNormals.at(i + 2) + translation.z);
			}
		}
	}
}

std::vector<float> Voxel::Cube::getColors3(const Face face, const glm::vec3& color)
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

std::vector<float> Voxel::Cube::getColors4WithoutShade(const Face face, const glm::vec4 & color)
{
	if (face == Cube::Face::NONE)
	{
		return std::vector<float>();
	}
	else
	{
		std::vector<float> colors;

		int totalFaces = countFaceBit(face);

		for (int i = 0; i < totalFaces; i++)
		{
			for (int i = 0; i < 4; i++)
			{
				colors.push_back(color.r);
				colors.push_back(color.g);
				colors.push_back(color.b);
				colors.push_back(color.a);
			}
		}

		return colors;
	}
}

void Voxel::Cube::getColors4WithoutShade(const Face face, const glm::vec4 & color, std::vector<float>& colors)
{
	if (face == Cube::Face::NONE)
	{
		return;
	}
	else
	{
		int totalFaces = countFaceBit(face);

		for (int i = 0; i < totalFaces; i++)
		{
			for (int i = 0; i < 4; i++)
			{
				colors.push_back(color.r);
				colors.push_back(color.g);
				colors.push_back(color.b);
				colors.push_back(color.a);
			}
		}
	}
}

std::vector<float> Voxel::Cube::getColors4WithDefaultShade(const Face & face, const glm::vec4 & color)
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
			glm::vec3 sideColor = color * FrontAndBackShadeRatio;
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
			glm::vec3 sideColor = color * LeftAndRightShadeRatio;
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
			glm::vec3 sideColor = color * FrontAndBackShadeRatio;
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
			glm::vec3 sideColor = color * LeftAndRightShadeRatio;
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
			glm::vec3 bottomColor = color * BottomShadeRatio;
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

void Voxel::Cube::getColors4WithDefaultShade(const Face & face, const glm::vec4 & color, std::vector<float>& colors)
{
	if (face == Cube::Face::NONE)
	{
		return;
	}
	else
	{
		if (face & Cube::Face::FRONT)
		{
			glm::vec3 sideColor = color * FrontAndBackShadeRatio;
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
			glm::vec3 sideColor = color * LeftAndRightShadeRatio;
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
			glm::vec3 sideColor = color * FrontAndBackShadeRatio;
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
			glm::vec3 sideColor = color * LeftAndRightShadeRatio;
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
			glm::vec3 bottomColor = color * BottomShadeRatio;
			for (int i = 0; i < 4; i++)
			{
				colors.push_back(bottomColor.r);
				colors.push_back(bottomColor.g);
				colors.push_back(bottomColor.b);
				colors.push_back(color.a);
			}
		}
	}
}

std::vector<float> Voxel::Cube::getColors4WithShade(const Face face, const glm::vec4& color, const std::vector<unsigned int>& shadowWeight)
{
	if (face == Cube::Face::NONE)
	{
		return std::vector<float>();
	}
	else
	{
		std::vector<float> colors;

		// Generate coloer vertices based on 

		if (face & Cube::Face::FRONT)
		{
			glm::vec3 sideColor = color * FrontAndBackShadeRatio;

			// 0, 1, 2, 3
			// index 0 (3, 4, 5)
			float count0 = static_cast<float>(shadowWeight.at(3) + shadowWeight.at(4) + shadowWeight.at(5));
			float colorMod0 = count0 * ShadePower;

			colors.push_back(sideColor.r - colorMod0);
			colors.push_back(sideColor.g - colorMod0);
			colors.push_back(sideColor.b - colorMod0);
			colors.push_back(color.a);

			// index 1 (11, 12, 13)
			float count1 = static_cast<float>(shadowWeight.at(11) + shadowWeight.at(12) + shadowWeight.at(13));
			float colorMod1 = count1 * ShadePower;

			colors.push_back(sideColor.r - colorMod1);
			colors.push_back(sideColor.g - colorMod1);
			colors.push_back(sideColor.b - colorMod1);
			colors.push_back(color.a);

			// index 2 (1, 2, 3)
			float count2 = static_cast<float>(shadowWeight.at(1) + shadowWeight.at(2) + shadowWeight.at(3));
			float colorMod2 = count2 * ShadePower;

			colors.push_back(sideColor.r - colorMod2);
			colors.push_back(sideColor.g - colorMod2);
			colors.push_back(sideColor.b - colorMod2);
			colors.push_back(color.a);

			// index 3 (9, 10, 11)
			float count3 = static_cast<float>(shadowWeight.at(9) + shadowWeight.at(10) + shadowWeight.at(11));
			float colorMod3 = count3 * ShadePower;

			colors.push_back(sideColor.r - colorMod3);
			colors.push_back(sideColor.g - colorMod3);
			colors.push_back(sideColor.b - colorMod3);
			colors.push_back(color.a);
		}

		if (face & Cube::Face::LEFT)
		{
			glm::vec3 sideColor = color * LeftAndRightShadeRatio;

			// 4, 5, 0, 1
			// index 4 (5, 6, 7)
			float count4 = static_cast<float>(shadowWeight.at(5) + shadowWeight.at(6) + shadowWeight.at(7));
			float colorMod4 = count4 * ShadePower;

			colors.push_back(sideColor.r - colorMod4);
			colors.push_back(sideColor.g - colorMod4);
			colors.push_back(sideColor.b - colorMod4);
			colors.push_back(color.a);

			// index 5 (13, 14, 15)
			float count5 = static_cast<float>(shadowWeight.at(13) + shadowWeight.at(14) + shadowWeight.at(15));
			float colorMod5 = count5 * ShadePower;

			colors.push_back(sideColor.r - colorMod5);
			colors.push_back(sideColor.g - colorMod5);
			colors.push_back(sideColor.b - colorMod5);
			colors.push_back(color.a);

			// index 0 (3, 4, 5)
			float count0 = static_cast<float>(shadowWeight.at(3) + shadowWeight.at(4) + shadowWeight.at(5));
			float colorMod0 = count0 * ShadePower;

			colors.push_back(sideColor.r - colorMod0);
			colors.push_back(sideColor.g - colorMod0);
			colors.push_back(sideColor.b - colorMod0);
			colors.push_back(color.a);

			// index 1 (11, 12, 13)
			float count1 = static_cast<float>(shadowWeight.at(11) + shadowWeight.at(12) + shadowWeight.at(13));
			float colorMod1 = count1 * ShadePower;

			colors.push_back(sideColor.r - colorMod1);
			colors.push_back(sideColor.g - colorMod1);
			colors.push_back(sideColor.b - colorMod1);
			colors.push_back(color.a);
		}

		if (face & Cube::Face::BACK)
		{
			glm::vec3 sideColor = color * FrontAndBackShadeRatio;

			// 6, 7, 4, 5
			// index 6 (1, 0, 7)
			float count6 = static_cast<float>(shadowWeight.at(1) + shadowWeight.at(0) + shadowWeight.at(7));
			float colorMod6 = count6 * ShadePower;

			colors.push_back(sideColor.r - colorMod6);
			colors.push_back(sideColor.g - colorMod6);
			colors.push_back(sideColor.b - colorMod6);
			colors.push_back(color.a);

			// index 7 (9, 8, 15)
			float count7 = static_cast<float>(shadowWeight.at(9) + shadowWeight.at(8) + shadowWeight.at(15));
			float colorMod7 = count7 * ShadePower;

			colors.push_back(sideColor.r - colorMod7);
			colors.push_back(sideColor.g - colorMod7);
			colors.push_back(sideColor.b - colorMod7);
			colors.push_back(color.a);

			// index 4 (5, 6, 7)
			float count4 = static_cast<float>(shadowWeight.at(5) + shadowWeight.at(6) + shadowWeight.at(7));
			float colorMod4 = count4 * ShadePower;

			colors.push_back(sideColor.r - colorMod4);
			colors.push_back(sideColor.g - colorMod4);
			colors.push_back(sideColor.b - colorMod4);
			colors.push_back(color.a);

			// index 5 (13, 14, 15)
			float count5 = static_cast<float>(shadowWeight.at(13) + shadowWeight.at(14) + shadowWeight.at(15));
			float colorMod5 = count5 * ShadePower;

			colors.push_back(sideColor.r - colorMod5);
			colors.push_back(sideColor.g - colorMod5);
			colors.push_back(sideColor.b - colorMod5);
			colors.push_back(color.a);
		}

		if (face & Cube::Face::RIGHT)
		{
			glm::vec3 sideColor = color * LeftAndRightShadeRatio;

			//2, 3, 6, 7
			// index 2 (1, 2, 3)
			float count2 = static_cast<float>(shadowWeight.at(1) + shadowWeight.at(2) + shadowWeight.at(3));
			float colorMod2 = count2 * ShadePower;

			colors.push_back(sideColor.r - colorMod2);
			colors.push_back(sideColor.g - colorMod2);
			colors.push_back(sideColor.b - colorMod2);
			colors.push_back(color.a);

			// index 3 (9, 10, 11)
			float count3 = static_cast<float>(shadowWeight.at(9) + shadowWeight.at(10) + shadowWeight.at(11));
			float colorMod3 = count3 * ShadePower;

			colors.push_back(sideColor.r - colorMod3);
			colors.push_back(sideColor.g - colorMod3);
			colors.push_back(sideColor.b - colorMod3);
			colors.push_back(color.a);

			// index 6 (1, 0, 7)
			float count6 = static_cast<float>(shadowWeight.at(1) + shadowWeight.at(0) + shadowWeight.at(7));
			float colorMod6 = count6 * ShadePower;

			colors.push_back(sideColor.r - colorMod6);
			colors.push_back(sideColor.g - colorMod6);
			colors.push_back(sideColor.b - colorMod6);
			colors.push_back(color.a);

			// index 7 (9, 8, 15)
			float count7 = static_cast<float>(shadowWeight.at(9) + shadowWeight.at(8) + shadowWeight.at(15));
			float colorMod7 = count7 * ShadePower;

			colors.push_back(sideColor.r - colorMod7);
			colors.push_back(sideColor.g - colorMod7);
			colors.push_back(sideColor.b - colorMod7);
			colors.push_back(color.a);
		}

		if (face & Cube::Face::TOP)
		{
			glm::vec3 sideColor = color * TopShadeRatio;

			// 1, 5, 3, 7

			// index 1 (11, 12, 13)
			float count1 = static_cast<float>(shadowWeight.at(11) + shadowWeight.at(12) + shadowWeight.at(13));
			float colorMod1 = count1 * ShadePower;

			colors.push_back(sideColor.r - colorMod1);
			colors.push_back(sideColor.g - colorMod1);
			colors.push_back(sideColor.b - colorMod1);
			colors.push_back(color.a);

			// index 5 (13, 14, 15)
			float count5 = static_cast<float>(shadowWeight.at(13) + shadowWeight.at(14) + shadowWeight.at(15));
			float colorMod5 = count5 * ShadePower;

			colors.push_back(sideColor.r - colorMod5);
			colors.push_back(sideColor.g - colorMod5);
			colors.push_back(sideColor.b - colorMod5);
			colors.push_back(color.a);

			// index 3 (9, 10, 11)
			float count3 = static_cast<float>(shadowWeight.at(9) + shadowWeight.at(10) + shadowWeight.at(11));
			float colorMod3 = count3 * ShadePower;

			colors.push_back(sideColor.r - colorMod3);
			colors.push_back(sideColor.g - colorMod3);
			colors.push_back(sideColor.b - colorMod3);
			colors.push_back(color.a);

			// index 7 (9, 8, 15)
			float count7 = static_cast<float>(shadowWeight.at(9) + shadowWeight.at(8) + shadowWeight.at(15));
			float colorMod7 = count7 * ShadePower;

			colors.push_back(sideColor.r - colorMod7);
			colors.push_back(sideColor.g - colorMod7);
			colors.push_back(sideColor.b - colorMod7);
			colors.push_back(color.a);
		}

		if (face & Cube::Face::BOTTOM)
		{
			glm::vec3 sideColor = color * BottomShadeRatio;

			// 0, 4, 2, 6
			// index 0 (3, 4, 5)
			float count0 = static_cast<float>(shadowWeight.at(3) + shadowWeight.at(4) + shadowWeight.at(5));
			float colorMod0 = count0 * ShadePower;

			colors.push_back(sideColor.r - colorMod0);
			colors.push_back(sideColor.g - colorMod0);
			colors.push_back(sideColor.b - colorMod0);
			colors.push_back(color.a);

			// index 4 (5, 6, 7)
			float count4 = static_cast<float>(shadowWeight.at(5) + shadowWeight.at(6) + shadowWeight.at(7));
			float colorMod4 = count4 * ShadePower;

			colors.push_back(sideColor.r - colorMod4);
			colors.push_back(sideColor.g - colorMod4);
			colors.push_back(sideColor.b - colorMod4);
			colors.push_back(color.a);

			// index 2 (1, 2, 3)
			float count2 = static_cast<float>(shadowWeight.at(1) + shadowWeight.at(2) + shadowWeight.at(3));
			float colorMod2 = count2 * ShadePower;

			colors.push_back(sideColor.r - colorMod2);
			colors.push_back(sideColor.g - colorMod2);
			colors.push_back(sideColor.b - colorMod2);
			colors.push_back(color.a);

			// index 6 (1, 0, 7)
			float count6 = static_cast<float>(shadowWeight.at(1) + shadowWeight.at(0) + shadowWeight.at(7));
			float colorMod6 = count6 * ShadePower;

			colors.push_back(sideColor.r - colorMod6);
			colors.push_back(sideColor.g - colorMod6);
			colors.push_back(sideColor.b - colorMod6);
			colors.push_back(color.a);
		}

		return colors;
	}
}

void Voxel::Cube::getColors4WithShade(const Face face, const glm::vec4 & color, const std::vector<unsigned int>& shadowWeight, std::vector<float>& colors)
{
	if (face == Cube::Face::NONE)
	{
		return;
	}
	else
	{
		// Generate coloer vertices based on 
		if (face & Cube::Face::FRONT)
		{
			glm::vec3 sideColor = color * FrontAndBackShadeRatio;

			// 0, 1, 2, 3
			// index 0 (3, 4, 5)
			float count0 = static_cast<float>(shadowWeight.at(3) + shadowWeight.at(4) + shadowWeight.at(5));
			float colorMod0 = count0 * ShadePower;

			colors.push_back(sideColor.r - colorMod0);
			colors.push_back(sideColor.g - colorMod0);
			colors.push_back(sideColor.b - colorMod0);
			colors.push_back(color.a);

			// index 1 (11, 12, 13)
			float count1 = static_cast<float>(shadowWeight.at(11) + shadowWeight.at(12) + shadowWeight.at(13));
			float colorMod1 = count1 * ShadePower;

			colors.push_back(sideColor.r - colorMod1);
			colors.push_back(sideColor.g - colorMod1);
			colors.push_back(sideColor.b - colorMod1);
			colors.push_back(color.a);

			// index 2 (1, 2, 3)
			float count2 = static_cast<float>(shadowWeight.at(1) + shadowWeight.at(2) + shadowWeight.at(3));
			float colorMod2 = count2 * ShadePower;

			colors.push_back(sideColor.r - colorMod2);
			colors.push_back(sideColor.g - colorMod2);
			colors.push_back(sideColor.b - colorMod2);
			colors.push_back(color.a);

			// index 3 (9, 10, 11)
			float count3 = static_cast<float>(shadowWeight.at(9) + shadowWeight.at(10) + shadowWeight.at(11));
			float colorMod3 = count3 * ShadePower;

			colors.push_back(sideColor.r - colorMod3);
			colors.push_back(sideColor.g - colorMod3);
			colors.push_back(sideColor.b - colorMod3);
			colors.push_back(color.a);
		}

		if (face & Cube::Face::LEFT)
		{
			glm::vec3 sideColor = color * LeftAndRightShadeRatio;

			// 4, 5, 0, 1
			// index 4 (5, 6, 7)
			float count4 = static_cast<float>(shadowWeight.at(5) + shadowWeight.at(6) + shadowWeight.at(7));
			float colorMod4 = count4 * ShadePower;

			colors.push_back(sideColor.r - colorMod4);
			colors.push_back(sideColor.g - colorMod4);
			colors.push_back(sideColor.b - colorMod4);
			colors.push_back(color.a);

			// index 5 (13, 14, 15)
			float count5 = static_cast<float>(shadowWeight.at(13) + shadowWeight.at(14) + shadowWeight.at(15));
			float colorMod5 = count5 * ShadePower;

			colors.push_back(sideColor.r - colorMod5);
			colors.push_back(sideColor.g - colorMod5);
			colors.push_back(sideColor.b - colorMod5);
			colors.push_back(color.a);

			// index 0 (3, 4, 5)
			float count0 = static_cast<float>(shadowWeight.at(3) + shadowWeight.at(4) + shadowWeight.at(5));
			float colorMod0 = count0 * ShadePower;

			colors.push_back(sideColor.r - colorMod0);
			colors.push_back(sideColor.g - colorMod0);
			colors.push_back(sideColor.b - colorMod0);
			colors.push_back(color.a);

			// index 1 (11, 12, 13)
			float count1 = static_cast<float>(shadowWeight.at(11) + shadowWeight.at(12) + shadowWeight.at(13));
			float colorMod1 = count1 * ShadePower;

			colors.push_back(sideColor.r - colorMod1);
			colors.push_back(sideColor.g - colorMod1);
			colors.push_back(sideColor.b - colorMod1);
			colors.push_back(color.a);
		}

		if (face & Cube::Face::BACK)
		{
			glm::vec3 sideColor = color * FrontAndBackShadeRatio;

			// 6, 7, 4, 5
			// index 6 (1, 0, 7)
			float count6 = static_cast<float>(shadowWeight.at(1) + shadowWeight.at(0) + shadowWeight.at(7));
			float colorMod6 = count6 * ShadePower;

			colors.push_back(sideColor.r - colorMod6);
			colors.push_back(sideColor.g - colorMod6);
			colors.push_back(sideColor.b - colorMod6);
			colors.push_back(color.a);

			// index 7 (9, 8, 15)
			float count7 = static_cast<float>(shadowWeight.at(9) + shadowWeight.at(8) + shadowWeight.at(15));
			float colorMod7 = count7 * ShadePower;

			colors.push_back(sideColor.r - colorMod7);
			colors.push_back(sideColor.g - colorMod7);
			colors.push_back(sideColor.b - colorMod7);
			colors.push_back(color.a);

			// index 4 (5, 6, 7)
			float count4 = static_cast<float>(shadowWeight.at(5) + shadowWeight.at(6) + shadowWeight.at(7));
			float colorMod4 = count4 * ShadePower;

			colors.push_back(sideColor.r - colorMod4);
			colors.push_back(sideColor.g - colorMod4);
			colors.push_back(sideColor.b - colorMod4);
			colors.push_back(color.a);

			// index 5 (13, 14, 15)
			float count5 = static_cast<float>(shadowWeight.at(13) + shadowWeight.at(14) + shadowWeight.at(15));
			float colorMod5 = count5 * ShadePower;

			colors.push_back(sideColor.r - colorMod5);
			colors.push_back(sideColor.g - colorMod5);
			colors.push_back(sideColor.b - colorMod5);
			colors.push_back(color.a);
		}

		if (face & Cube::Face::RIGHT)
		{
			glm::vec3 sideColor = color * LeftAndRightShadeRatio;

			//2, 3, 6, 7
			// index 2 (1, 2, 3)
			float count2 = static_cast<float>(shadowWeight.at(1) + shadowWeight.at(2) + shadowWeight.at(3));
			float colorMod2 = count2 * ShadePower;

			colors.push_back(sideColor.r - colorMod2);
			colors.push_back(sideColor.g - colorMod2);
			colors.push_back(sideColor.b - colorMod2);
			colors.push_back(color.a);

			// index 3 (9, 10, 11)
			float count3 = static_cast<float>(shadowWeight.at(9) + shadowWeight.at(10) + shadowWeight.at(11));
			float colorMod3 = count3 * ShadePower;

			colors.push_back(sideColor.r - colorMod3);
			colors.push_back(sideColor.g - colorMod3);
			colors.push_back(sideColor.b - colorMod3);
			colors.push_back(color.a);

			// index 6 (1, 0, 7)
			float count6 = static_cast<float>(shadowWeight.at(1) + shadowWeight.at(0) + shadowWeight.at(7));
			float colorMod6 = count6 * ShadePower;

			colors.push_back(sideColor.r - colorMod6);
			colors.push_back(sideColor.g - colorMod6);
			colors.push_back(sideColor.b - colorMod6);
			colors.push_back(color.a);

			// index 7 (9, 8, 15)
			float count7 = static_cast<float>(shadowWeight.at(9) + shadowWeight.at(8) + shadowWeight.at(15));
			float colorMod7 = count7 * ShadePower;

			colors.push_back(sideColor.r - colorMod7);
			colors.push_back(sideColor.g - colorMod7);
			colors.push_back(sideColor.b - colorMod7);
			colors.push_back(color.a);
		}

		if (face & Cube::Face::TOP)
		{
			glm::vec3 sideColor = color * TopShadeRatio;

			// 1, 5, 3, 7

			// index 1 (11, 12, 13)
			float count1 = static_cast<float>(shadowWeight.at(11) + shadowWeight.at(12) + shadowWeight.at(13));
			float colorMod1 = count1 * ShadePower;

			colors.push_back(sideColor.r - colorMod1);
			colors.push_back(sideColor.g - colorMod1);
			colors.push_back(sideColor.b - colorMod1);
			colors.push_back(color.a);

			// index 5 (13, 14, 15)
			float count5 = static_cast<float>(shadowWeight.at(13) + shadowWeight.at(14) + shadowWeight.at(15));
			float colorMod5 = count5 * ShadePower;

			colors.push_back(sideColor.r - colorMod5);
			colors.push_back(sideColor.g - colorMod5);
			colors.push_back(sideColor.b - colorMod5);
			colors.push_back(color.a);

			// index 3 (9, 10, 11)
			float count3 = static_cast<float>(shadowWeight.at(9) + shadowWeight.at(10) + shadowWeight.at(11));
			float colorMod3 = count3 * ShadePower;

			colors.push_back(sideColor.r - colorMod3);
			colors.push_back(sideColor.g - colorMod3);
			colors.push_back(sideColor.b - colorMod3);
			colors.push_back(color.a);

			// index 7 (9, 8, 15)
			float count7 = static_cast<float>(shadowWeight.at(9) + shadowWeight.at(8) + shadowWeight.at(15));
			float colorMod7 = count7 * ShadePower;

			colors.push_back(sideColor.r - colorMod7);
			colors.push_back(sideColor.g - colorMod7);
			colors.push_back(sideColor.b - colorMod7);
			colors.push_back(color.a);
		}

		if (face & Cube::Face::BOTTOM)
		{
			glm::vec3 sideColor = color * BottomShadeRatio;

			// 0, 4, 2, 6
			// index 0 (3, 4, 5)
			float count0 = static_cast<float>(shadowWeight.at(3) + shadowWeight.at(4) + shadowWeight.at(5));
			float colorMod0 = count0 * ShadePower;

			colors.push_back(sideColor.r - colorMod0);
			colors.push_back(sideColor.g - colorMod0);
			colors.push_back(sideColor.b - colorMod0);
			colors.push_back(color.a);

			// index 4 (5, 6, 7)
			float count4 = static_cast<float>(shadowWeight.at(5) + shadowWeight.at(6) + shadowWeight.at(7));
			float colorMod4 = count4 * ShadePower;

			colors.push_back(sideColor.r - colorMod4);
			colors.push_back(sideColor.g - colorMod4);
			colors.push_back(sideColor.b - colorMod4);
			colors.push_back(color.a);

			// index 2 (1, 2, 3)
			float count2 = static_cast<float>(shadowWeight.at(1) + shadowWeight.at(2) + shadowWeight.at(3));
			float colorMod2 = count2 * ShadePower;

			colors.push_back(sideColor.r - colorMod2);
			colors.push_back(sideColor.g - colorMod2);
			colors.push_back(sideColor.b - colorMod2);
			colors.push_back(color.a);

			// index 6 (1, 0, 7)
			float count6 = static_cast<float>(shadowWeight.at(1) + shadowWeight.at(0) + shadowWeight.at(7));
			float colorMod6 = count6 * ShadePower;

			colors.push_back(sideColor.r - colorMod6);
			colors.push_back(sideColor.g - colorMod6);
			colors.push_back(sideColor.b - colorMod6);
			colors.push_back(color.a);
		}
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

void Voxel::Cube::getIndices(Face face, const int cubeOffset, std::vector<unsigned int>& indices)
{
	if (face == Cube::Face::NONE)
	{
		return;
	}
	else
	{
		if (face == Cube::Face::ALL)
		{
			// Get all 
			indices.insert(indices.end(), Cube::indices.begin(), Cube::indices.end());
			return;
		}
		else
		{
			int num = countFaceBit(face);

			int offset = 0 + cubeOffset;

			for (int i = 0; i < num; i++)
			{
				for (int i = 0; i < 6; i++)
				{
					indices.push_back(faceIndices.at(i) + offset);
				}
				offset += 4;
			}
		}
	}
}

std::string Voxel::Cube::faceToString(const Face & face)
{
	switch (face)
	{
	case Cube::Face::FRONT:
		return "FRONT";
		break;
	case Cube::Face::LEFT:
		return "LEFT";
		break;
	case Cube::Face::BACK:
		return "BACK";
		break;
	case Cube::Face::RIGHT:
		return "RIGHT";
		break;
	case Cube::Face::TOP:
		return "TOP";
		break;
	case Cube::Face::BOTTOM:
		return "BOTTOM";
		break;
	default:
		return std::string();
		break;
	}
}
