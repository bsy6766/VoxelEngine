#ifndef WORLD_MAP_H
#define WORLD_MAP_H

#include <GL\glew.h>
#include <glm\glm.hpp>

namespace Voxel
{
	// Foward declaration
	class World;
	namespace UI
	{
		class Canvas;
		class Text;
		class Image;
	}

	/**
	*	@struct RegionMesh
	*	@brief Mesh for single region
	*/
	struct RegionMesh
	{
	public:
		GLuint vao;
		glm::mat4 modelMat;
		glm::vec3 position;

		RegionMesh() : vao(0), modelMat(glm::mat4(1.0f)), position(glm::vec3(0.0f)) {}
		~RegionMesh()
		{

		}
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
		
		// UI canvas
		UI::Canvas* uiCanvas;
		// Images
		UI::Image* compass;
		UI::Image* cameraIcon;
		// Texts
		UI::Text* worldName;

		// position of the map
		glm::vec3 position;

		// Release all the mesh. This is called on destructor.
		void releaseMesh();
	public:
		WorldMap();
		~WorldMap();

		/**
		*	Initialize world map. 
		*	@param [in] world A world instance to access region data
		*/
		void init(World* world);
		
		void updateWorld(World* world);
	};
}

#endif