#ifndef SCHEMATIC_H
#define SCHEMATIC_H

// gl
#include <GL\glew.h>

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
		friend class Schematic;
	private:
		SchematicBlock();
		~SchematicBlock() = default;

		// position of block
		glm::ivec3 pos;
		glm::vec3 color;
	};

	class SchematicMesh
	{
		friend class Schematic;
	private:
		// Only Schematic class can build mesh
		SchematicMesh();

		// gl
		GLuint vao;

		// Build mesh.
		void buildMesh(const std::vector<std::vector<std::vector<SchematicBlock*>>>& blocks);
	public:
		// Destructor
		~SchematicMesh();

		// Render mesh
		void render();
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
		// Dimension of schematic
		glm::ivec3 dimension;
		
		// Blocks. 3D vector in order of y -> x -> z. Basically stack of x, z.
		std::vector<std::vector<std::vector<SchematicBlock*>>> blocks;

		// Check if pos in range of blocks vector
		bool isPosInRange(const glm::ivec3& pos);
	public:
		// Constructor
		Schematic() = delete;
		Schematic(const glm::ivec3& dimension);

		// Destructor
		~Schematic();

		// Add block on pos with color. true if successfully adds. Else, false;
		bool addBlock(const glm::ivec3& pos, const glm::vec3& color);

		// build mesh of blocks.
		SchematicMesh* buildMesh();
	};
}

#endif