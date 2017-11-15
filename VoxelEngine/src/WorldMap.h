#ifndef WORLD_MAP_H
#define WORLD_MAP_H

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <vector>

namespace Voxel
{
	// Foward declaration
	class World;
	class Program;

	namespace UI
	{
		class Canvas;
		class Text;
		class Image;
	}

	/**
	*	@class RegionMesh
	*	@brief Mesh for single region
	*/
	class RegionMesh
	{
	public:
		GLuint fillVao;
		unsigned int fillSize;
		GLuint sideVao;
		glm::mat4 modelMat;
		glm::vec3 position;
		glm::vec4 color;

		// Pointer to programs
		static Program* polygonProgram;
		static Program* sideProgram;

		RegionMesh();
		RegionMesh(const glm::mat4& modelMat, const glm::vec3& position);
		~RegionMesh();

		void buildMesh(const std::vector<float>& fillVertices, const std::vector<unsigned int>& fillIndices);
		void render(const glm::mat4& worldMapMVPMat);
	};

	/**
	*	@class WorldMap
	*	@breif Renders world map
	*	@note This isn't the Universe map. This shows all the regions as a movable, zoomable and interactable map.
	*/
	class WorldMap
	{
	private:
		// OpenGL
		GLuint vao;

		// MVP matrix for world map
		glm::mat4 MVPMatrix;
		
		// UI canvas
		UI::Canvas* uiCanvas;
		// Images
		UI::Image* compass;
		UI::Image* cameraIcon;
		// Texts
		UI::Text* worldName;

		// position of the map
		glm::vec3 position;

		// List of regionMeshes. index means region ID
		std::vector<RegionMesh*> regionMeshes;
		
		// Release all the mesh. This is called on destructor.
		void releaseMesh();
	public:
		WorldMap();
		~WorldMap();

		// Initialize world map
		void init();

		/**
		*	Initialize world map. 
		*	@param [in] world A world instance to access region data
		*/
		void buildMesh(World* world);
		
		void updatePosition(const glm::vec2& playerXZPos);
		void updateMatrix(const glm::mat4& VPMatrix);
		
		// Clear all the meshes and data.
		void clear();

		// Render world map
		void render();
	};
}

#endif