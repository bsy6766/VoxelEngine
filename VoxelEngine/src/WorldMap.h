#ifndef WORLD_MAP_H
#define WORLD_MAP_H

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <vector>
#include <Geometry.h>
#include <Ray.h>

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

		glm::vec4 color;
		glm::vec4 sideColor;

		std::vector<Geometry::Triangle> triangles;

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
	public:
		enum class State
		{
			IDLE = 0,
			PAN,
			ROTATE,
		};
	private:
		// State
		State state;

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

		// List of regionMeshes. index means region ID
		std::vector<RegionMesh*> regionMeshes;

		// For view matrix and smooth transition and rotation
		glm::vec3 position;
		glm::vec3 nextPosition;

		glm::vec2 rotation;
		glm::vec2 nextRotation;

		float zoomZ;
		float zoomZTarget;

		// Boundary of map panning.
		glm::vec3 posBoundary;

		// Last point where cursor clicked on screen
		glm::vec2 prevMouseClickedPos;
		
		// Boundary check
		void checkPosBoundary();
		void checkNextPosBoundary();

		void raycastRegion();

		glm::mat4 getViewMatrix();

		// Release all the mesh. This is called on destructor.
		void releaseMesh();


		// debug
		GLuint rayVao;
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
		
		void update(const float delta);

		void updatePosition(const glm::vec3& playerPos);
		void updateViewMatrix();
		void updateWithCamViewMatrix(const glm::mat4& viewMat);
		void updateModelMatrix();

		void updateMouseClick(const int button, const bool clicked, const glm::vec2& mousePos);
		void updateMouseMove(const glm::vec2& delta);

		void resetPosAndRot();

		void zoomIn(const float delta);
		void zoomOut(const float delta);
		
		// Clear all the meshes and data.
		void clear();

		void print();

		// Render world map
		void render();
	};
}

#endif