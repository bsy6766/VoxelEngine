#ifndef SCHEMATIC_H
#define SCHEMATIC_H

// glm
#include <glm\glm.hpp>

// cpp
#include <vector>

namespace Voxel
{
	/**
	*	@class SchematicBlock
	*	@brief Simple class that conatins block data.
	*
	*	This class is similar to Block class but with more simplified functionality.
	*/
	class SchematicBlock
	{
	public:
		SchematicBlock();

		// position of block
		glm::ivec3 pos;
		glm::vec3 color;
	};

	/**
	*	@class Schematic
	*	@brief Schematic that contains block data
	*
	*	Schematic contains block data of voxel object.
	*	Schematic can be used to paste block data to chunk map in game.
	*	Also can be saved in file format on user folder.
	*/
	class Schematic
	{
	private:
		glm::ivec3 dimension;

		// Blocks. 3D vector in order of y -> x -> z. Basically stack of x, z.
		std::vector<std::vector<std::vector<SchematicBlock*>>> blocks;
	public:
		Schematic() = delete;
		Schematic(const glm::ivec3& dimension);
		~Schematic();
	};
}

#endif