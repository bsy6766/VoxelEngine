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
		unsigned int sideSize;
		glm::mat4 modelMat;
		glm::vec3 position;
		glm::vec2 rotation;
		glm::vec4 color;
		glm::vec4 sideColor;

		// Pointer to programs
		static Program* polygonProgram;
		static Program* sideProgram;

		RegionMesh();
		RegionMesh(const glm::mat4& modelMat, const glm::vec3& position);
		~RegionMesh();

		void buildMesh(const std::vector<float>& fillVertices, const std::vector<unsigned int>& fillIndices, const std::vector<float>& sideVertices, const std::vector<unsigned int>& sideIndices);
		void renderPolygon(const glm::mat4& worldModelMat);
		void renderPolygonSide(const glm::mat4& worldModelMat);
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

		// model matrix for world map. scales down, follows palyer.
		glm::mat4 modelMat;
		
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
		
		void updatePosition(const glm::vec3& playerPos);
		void updateViewMatrix(const glm::mat4& viewMat);
		
		// Clear all the meshes and data.
		void clear();

		// Render world map
		void render();
	};
}

#endif