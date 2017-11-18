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
	class WorldMap;

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
		// Only WorldMap can create region mesh
		friend class WorldMap;
	private:
		// Constructor
		RegionMesh();
		// Destructor. Release mesh.
		~RegionMesh();

		// Polygon
		GLuint fillVao;
		unsigned int fillSize;

		// Polygon side
		GLuint sideVao;
		unsigned int sideSize;

		// Model matrix for region mesh
		glm::mat4 modelMat;

		// Current y offset if selected
		float curSelectY;
		// Next y offset for smooth translation if selected
		float selectYTarget;

		// Color of polygon
		glm::vec4 color;
		// color of polygon sides
		glm::vec4 sideColor;

		// Triangles in polygon. For raycasting.
		std::vector<Geometry::Triangle> triangles;

		// Pointer to programs
		static Program* polygonProgram;
		static Program* sideProgram;

	public:
		/**
		*	Updates region mesh. Smoothly moves.
		*	@param [in] delta Elapsed time for current frame.
		*/
		void update(const float delta);

		/**
		*	Build mesh for region. One polygon and side of polygon each.
		*	@param [in] fillVertices Vertices for polygon
		*	@param [in] fillIndices Indices for polygon
		*	@param [in] sideVertices Vertices for polygon
		*	@param [in] sideIndices Indices for polygon
		*/
		void buildMesh(const std::vector<float>& fillVertices, const std::vector<unsigned int>& fillIndices, const std::vector<float>& sideVertices, const std::vector<unsigned int>& sideIndices);

		/**
		*	Renders the polygon
		*	@param [in] worldModelMat Model matrix of entire world map.
		*/
		void renderPolygon(const glm::mat4& worldModelMat);

		/**
		*	Renders side of the polygon
		*	@param [in] worldModelMat Model matrix of entire world map.
		*/
		void renderPolygonSide(const glm::mat4& worldModelMat);

		/**
		*	Selects the region
		*/
		void select();

		/**
		*	Unselects the region
		*/
		void unSelect();
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
			PAN,				// Translate world map from bird eye view
			ROTATE,				// Rotates world map from itself
		};
	private:
		// State of world map. 
		State state;

		// OpenGL
		GLuint vao;

		// Model matrix of world map. Only uses position.
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

		// Current position of world map
		glm::vec3 position;
		// Next position of world map for smooth translation
		glm::vec3 nextPosition;

		// Current rotation of world map
		glm::vec2 rotation;
		// Next rotation of world map for smooth rotation
		glm::vec2 nextRotation;

		// Current zoom distance of world map
		float zoomZ;
		// Next zoom distance of world map for smooth zooming
		float zoomZTarget;

		// Current region ID that is clicked. -1 if nothing is clicked
		int selectedRegionID;
		int hoveringRegionID;

		// Boundary of map panning.
		glm::vec3 posBoundary;

		// Last point where cursor clicked on screen
		glm::vec2 prevMouseClickedPos;
		glm::vec2 prevMouseMoved;
		
		// Boundary check
		void checkPosBoundary();

		// Boundary check on next position
		void checkNextPosBoundary();

		// for debug
		RegionMesh* getRegionMesh(const unsigned int regionMeshIndex);

		// Ray cast region based on clicked cursor position
		void raycastRegion(const glm::vec2& cursorPos, const bool select);

		// Release all the mesh. This is called on destructor.
		void releaseMesh();
	public:
		// Constructor
		WorldMap();

		// Destructor
		~WorldMap();

		// Initialize world map
		void init();

		/**
		*	Initialize world map. 
		*	@param [in] world A world instance to access region data
		*/
		void buildMesh(World* world);
		
		/**
		*	updates world map.
		*	Smoothly pans and rotates world map. 
		*	@param [in] delta Elsapsed time for current frame
		*/
		void update(const float delta);

		/**
		*	Updates view matrix of world map.
		*	Updates shader uniform
		*/
		void updateViewMatrix();

		/**
		*	Updates view matrix with main camera's view matrix
		*	It's for debug mode
		*/
		void updateWithCamViewMatrix(const glm::mat4& viewMat);

		/**
		*	Updates world map's model matrix
		*	Doesn't updates shader uniform. 
		*/
		void updateModelMatrix();

		/**
		*	Updates mouse click on world map.
		*	@param [in] button Mouse button. 0 means left, 1 means right.
		*	@param [in] clicked True if mouse button is clicked. False if mouse button is released.
		*	@param [in] mousePos Position of mouse. 
		*/
		void updateMouseClick(const int button, const bool clicked, const glm::vec2& mousePos);

		/**
		*	Updates mouse move. Updates pan and rotation.
		*/
		void updateMouseMove(const glm::vec2& delta, const glm::vec2& mousePos);

		/**
		*	Resests position and rotation of world map. Call this when world map gets closed.
		*/
		void resetPosAndRot();

		/**
		*	Zoom in world map. Limits zoom if it's too close
		*/
		void zoomIn();

		/**
		*	Zoom out world map. Limits zoom if it's too far
		*/
		void zoomOut();

		/**
		*	Get world map's view matrix
		*/
		glm::mat4 getViewMatrix();
		
		/**
		*	Clears world map. Deletes everything.
		*/
		void clear();

		/**
		*	Prints world map info.
		*/
		void print();

		/**
		*	Renders world map. Renders side of the polygon and then the polygon.
		*/
		void render();

		/**
		*	Renders mouse picking ray. It's for debug.
		*/
		void renderRay();

		/**
		*	Renders line at the cetner of the world map. It's for debug.
		*/
		void renderCenterLine();

		// Debug------
		GLuint rayVao = 0;
		GLuint centerVao = 0;
		void initDebugCenterLine();
		void initDebugMousePickRayLine(const Ray& ray);
		//------------
	};
}

#endif